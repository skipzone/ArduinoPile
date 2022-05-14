/*****************************************************************
 *                                                               *
 * Addressable LED Pixel Driver for Lampshade Hat                *
 *                                                               *
 * Platform:  Arduino Uno, Pro, Pro Mini                         *
 *                                                               *
 * Architecture and pattern functions by Ross Butler, Apr. 2015  *
 *                                                               *
 *****************************************************************/

#include <avr/pgmspace.h>

#include "FastLED.h"



/***************************
 * Configuration Constants *
 ***************************/

//#define DEBUG_SERIAL_PRINT

// I/O pin assignments
#define ONBOARD_LED_PIN         13
#define PIXEL_DATA_PIN           2
#define nPIXEL_STRIP_PWR_ON_PIN  4
#define PUSHBUTTON_PIN           8
#define RELAY_ON_PIN             7
#define VBATT_APIN               0

// power management
#define VBATT_READ_INTERVAL_MS 250
#define VBATT_FILTER_LENGTH 16          // must be a power of 2
#define VBATT_FILTER_DIV_SHIFT 4        // log2(VBATT_FILTER_LENGTH)
#define VBATT_LOW_ADC_READING 574       // 9.0 V:  1023 * (9.0 / 16.0) = 575
#define VBATT_SHUTDOWN_ADC_READING 536  // 8.4 V:  1023 * (8.4 / 16.0) = 537

// the total number of pixels in the strip
#define NUM_PHYSICAL_PIXELS 240

// the number of pixels at the start of the strip that should remain unused (off)
#define NUM_SKIP_PIXELS 0

// the number of active pixels
#define NUM_PIXELS (NUM_PHYSICAL_PIXELS - NUM_SKIP_PIXELS)

// the number of pixels attached to each panel
// (must be a factor of the number of physical pixels)
#define NUM_PANELS 1
#define PANEL_NUM_PIXELS (NUM_PIXELS / NUM_PANELS)

// colors for black WS2811S 12V pixel strips purchased from HolidayCoro 2014 presale.
#define BSU_BLUE           (CRGB::HTMLColorCode) 0x0000FF
#define BSU_ORANGE         (CRGB::HTMLColorCode) 0xD03000  // not tested--probably needs more red
#define HUE_BSU_BLUE       (HSVHue) 160
#define HUE_BSU_ORANGE     (HSVHue) 22

// BG_INTENSITY_SCALE_FACTOR sets the maximum brightness for backgrounds.  It also
// sets the brightness of the foreground of patterns that aren't doing some sort
// of flashing or sparkle effect.  Although the range is 0 to 255, practical
// values are more like 16 to 128.  48 is pretty darn bright in a darkened room.
// LampshadeHat should use 32 or less to limit current.
#define BG_INTENSITY_SCALE_FACTOR 32
#define BG_INTENSITY_SCALE_FACTOR_LOW 16

// MAX_WAVEFORMS is the maximum number of waveforms
// that can be configured for a multiWave pattern.
#define MAX_WAVEFORMS 3

// MAX_SPARKLE_DENSITY is the maximum number of foreground pixels
// that can be illuminated simultaneously in a sparkle pattern.
#define MAX_SPARKLE_DENSITY 5

// Approximate millisecond delay between sparkle changes.
#define SPARKLE_DELAY 15



/**************************************************
 * Pattern Configuration Constants and Structures *
 **************************************************/

// These are the waveforms that can be used in a multiWave pattern.  A good
// explanation of the shape and computation cost of the quadratic and cubic
// easing waveforms can be found here:
//
//     https://github.com/FastLED/FastLED/wiki/FastLED-Wave-Functions
//
// From that web page:
//
//     quadwave8(i) -- quadratic in/out easing applied to a triangle wave. This
//     makes nearly a sine wave, but takes only about 2/3rds of the CPU cycles
//     of FastLED's fastest sine function.
//
//     cubicwave8(i) -- cubic in/out easing applied to a triangle wave. This
//     makes a 'higher contrast' wave than the quad or sine wave. 10-20% faster
//     than FastLED's fastest sine function.
//
#define SQUARE_WAVE      0
#define TRIANGLE_WAVE    1
#define QUADRATIC_EASING 2
#define CUBIC_EASING     3
#define SINE_WAVE        4
  
// This structure holds a single waveform configuration
// that is part of a multiWave pattern.
typedef struct colorWaveParams colorWaveParams_t;
struct colorWaveParams {
  bool           randomHue;       // true if the waveform hue should be selected at random
  HSVHue         posHue;          // hue of the positive half of the waveform (if randomHue is false)
  HSVHue         negHue;          // hue of the negative half of the waveform (if randomHue is false)
  int8_t         numWaves;        // no. of complete waveforms in pattern; neg. for halfwave (neg. half black), 0 to disable wave 
  uint8_t        waveformType;    // a waveform type constant (SQUARE_WAVE, TRIANGLE_WAVE, etc.)
  bool           directionDown;   // true if the waves should move toward pixel 0, false if they should move the other direction
  uint8_t        delay;           // roughly, the number of milliseconds between each stepped movement of a wave
};

// This structure holds the configuration of a multiWave pattern.
// It contains MAX_WAVEFORMS single waveform configurations.
typedef struct multiWaveParams multiWaveParams_t;
struct multiWaveParams {
  bool              boundedByPanel;  // true if all the waveforms should appear across each panel, false if across the entire strip
  colorWaveParams_t waveParams[MAX_WAVEFORMS];
};


// This structure holds the configuration of a sparkle pattern.  Set both
// fgColorCode and bgColorCode to black for random color selection.
typedef struct sparkleParams sparkleParams_t;
struct sparkleParams {
  CRGB::HTMLColorCode fgColorCode;  // color code of the sparkle color
  CRGB::HTMLColorCode bgColorCode;  // color code of the background color
  uint8_t             density;      // number of foreground (sparkle) pixels on simultaneously (1 to MAX_SPARKLE_DENSITY)
};


// This structure hold the configuration of a moving-dot pattern.  Set both
// fgColorCode and bgColorCode to black for random color selection.
typedef struct movingDotParams movingDotParams_t;
struct movingDotParams {
  CRGB::HTMLColorCode fgColorCode;    // dot color
  CRGB::HTMLColorCode bgColorCode;    // background color
  bool                randomFgColor;  // true if the foreground color should be selected at random
  bool                randomBgColor;  // true if the background color should be selected at random
  bool                changeFgColor;  // true if the foreground color should be re-selected at random for each movement cycle
  bool                zipperBg;       // zipper effect:  set the bg color when moving away from pixel 0 then clear it when moving toward pixel 0
  bool                bidirectional;  // true if the dot should bounce back and forth
  uint8_t             delay0;         // approx. ms delay between dot steps when moving away from pixel 0
  uint8_t             delay1;         // approx. ms delay between dot steps when moving toward pixel 0
  uint16_t            holdTimeFarMs;  // number of ms to hold the dot at the far end before it moves back toward pixel 0
  uint16_t            holdTimeNearMs; // number of ms to hold the dot at the near end before it moves toward the last pixel
};


// This structure holds the configuration of a split-rotation pattern.
typedef struct splitRotationParams splitRotationParams_t;
struct splitRotationParams {
  bool    randomHue;      // true if the hues should be selected at random
  HSVHue  fgHue;          // foreground (moving pixels) hue
  HSVHue  bgHue;          // background hue
  uint8_t fgInterval;     // number of background pixels minus one between each moving pixel
  bool    directionDown;  // true to rotate one way, false to rotate the other way
  uint8_t delay;          // approx. ms delay between rotation steps
};


// This structure holds the configuration of a rainbow pattern.
typedef struct rainbowParams rainbowParams_t;
struct rainbowParams {
  bool    directionDown;  // true to rotate one way, false to rotate the other way
  uint8_t delay;          // approx. ms delay between rotation steps
};


// This structure contains the definition of an actual pattern to be displayed.
// It associates a pattern type with the values that control the pattern's
// appearance and the length of time the pattern should be displayed.
typedef struct patternDef patternDef_t;
struct patternDef {
  uint8_t       patternType;                      // a pattern type constant, used as an index into the pattern metadata table
  unsigned long durationSec;
  const void*   patParams;
};


// This structure contains pattern parameter metadata
// and a pointer to the pattern function.
typedef struct patternMetadata patternMetadata_t;
struct patternMetadata {
  uint8_t paramStructSize;
  void    (*loopFun)(patternDef_t*);
};


// The pattern functions are declared here so that we
// can use them to build the pattern metadata table.
void rainbow(patternDef_t* patParams);
void multiWave(patternDef_t* patParams);
void movingDot(patternDef_t* patParams);
void sparkle(patternDef_t* patParams);
void splitRotation(patternDef_t* patParams);


// These are the pattern types.  The values are indexes into the pattern metadata table.
#define MOVING_DOT     0
#define MULTI_WAVE     1
#define RAINBOW        2
#define SPARKLE        3
#define SPLIT_ROTATION 4


// Each pattern metadata entry must be in the element
// position corresponding to its pattern type constant.
patternMetadata_t patternMetadataTable[] = {
  {sizeof(movingDotParams_t)    , movingDot},
  {sizeof(multiWaveParams_t)    , multiWave},
  {sizeof(rainbowParams_t)      , rainbow},
  {sizeof(sparkleParams_t)      , sparkle},
  {sizeof(splitRotationParams_t), splitRotation}
};


// the size of the buffer used when copying pattern parameters from flash to RAM
#define PAT_PARAMS_BUF_SIZE 32



/********************************
 * Helper Function Declarations *
 ********************************/

void selectRandomRgb(CRGB* rgbColor, CRGB* rgbColorInverse);
void selectRandomHue(HSVHue* hue, HSVHue* hueInverse);
void rotateCrgbRight(CRGB* a, uint8_t length);
void rotateCrgbLeft(CRGB* a, uint8_t length);
void replicatePixelPanels();



/***********
 * Globals *
 ***********/

CRGB pixelArray[NUM_PHYSICAL_PIXELS];
CRGB* pixels = pixelArray + NUM_SKIP_PIXELS;

bool lowBatteryShutdown;
byte bgIntensityScaleFactor;



/***********************
 * Pattern Definitions *
 ***********************/

// ---------- multi-wave patterns ----------

// Each multiwave element consists of one boundedByPanel element and MAX_WAVEFORMS
// wave definition elements.  When less than MAX_WAVEFORMS are needed, set numWaves
// to zero in unused wave definitions.

// Wave definition elements:
//     randomHue       true if the waveform hue should be selected at random
//     posHue          hue of the positive half of the waveform (if randomHue is false)
//     negHue          hue of the negative half of the waveform (if randomHue is false)
//     numWaves        no. of complete waveforms in pattern; neg. for halfwave (neg. half black), 0 to disable wave 
//     waveformType    a waveform type constant (SQUARE_WAVE, TRIANGLE_WAVE, etc.)
//     directionDown   true if the waves should move toward pixel 0, false if they should move the other direction
//     delay           roughly, the number of milliseconds between each stepped movement of a wave (0-255)

const multiWaveParams_t multiWaveBsu PROGMEM = {
  true,
  { {false, HUE_BSU_BLUE , HUE_BSU_ORANGE,  1, CUBIC_EASING    , false,  60},
    {false, HUE_RED      , HUE_RED       ,  0, SINE_WAVE       , false,   0},
    {false, HUE_RED      , HUE_RED       ,  0, SINE_WAVE       , false,   0} } };

const multiWaveParams_t multiWave1Random PROGMEM = {
  true,
  { {true , HUE_RED      , HUE_RED       ,  3, QUADRATIC_EASING, false,  30},
    {false, HUE_RED      , HUE_RED       ,  0, QUADRATIC_EASING, false,   0},
    {false, HUE_RED      , HUE_RED       ,  0, QUADRATIC_EASING, false,   0} } };

const multiWaveParams_t multiWave2Random PROGMEM = { 
  false,
  { {true , HUE_RED      , HUE_RED       ,  1, QUADRATIC_EASING, false, 255},
    {false, HUE_RED      , HUE_RED       ,  5, QUADRATIC_EASING, false, 127},
    {false, HUE_RED      , HUE_RED       ,  0, QUADRATIC_EASING, false,   0} } };

const multiWaveParams_t multiWave3 PROGMEM = { 
  false,
  { {false, HUE_BLUE     , HUE_ORANGE    , -1, SINE_WAVE       , false,  30},
    {false, HUE_RED      , HUE_AQUA      , -2, SINE_WAVE       , true ,  40},
    {false, HUE_GREEN    , HUE_PINK      , -3, SINE_WAVE       , false,  50} } };

const multiWaveParams_t multiWave3SquareA PROGMEM = { 
  true,
  { {false, HUE_BLUE     , HUE_ORANGE    ,  1, SQUARE_WAVE     , true , 255},
    {false, HUE_RED      , HUE_AQUA      ,  2, SQUARE_WAVE     , true , 192},
    {false, HUE_GREEN    , HUE_PINK      ,  3, SQUARE_WAVE     , false, 128} } };

const multiWaveParams_t multiWave3SquareB PROGMEM = { 
  true,
  { {false, HUE_BLUE     , HUE_ORANGE    , -1, SQUARE_WAVE     , true ,  80},
    {false, HUE_RED      , HUE_AQUA      , -3, SQUARE_WAVE     , true ,  80},
    {false, HUE_GREEN    , HUE_PINK      , -5, SQUARE_WAVE     , false,  80} } };

const multiWaveParams_t multiWave3ClusterFuck PROGMEM = {
  false,
  { {false, HUE_BLUE     , HUE_BLUE      ,  1, QUADRATIC_EASING, true ,  60},
    {false, HUE_RED      , HUE_RED       ,  3, TRIANGLE_WAVE   , true ,  40},
    {false, HUE_GREEN    , HUE_GREEN     ,  5, QUADRATIC_EASING, true ,  20} } };


// ---------- sparkle patterns ----------

// Elements:
//     fgColorCode  color code of the sparkle color
//     bgColorCode  color code of the background color
//     density      number of foreground (sparkle) pixels on simultaneously (1 to MAX_SPARKLE_DENSITY)

const sparkleParams_t sparkleBluePretty PROGMEM = {CRGB::White   ,  CRGB::Blue    , 1};
const sparkleParams_t sparkleBlueCrazy  PROGMEM = {CRGB::White   ,  CRGB::Blue    , 5};
const sparkleParams_t sparkleBlueCrazy3  PROGMEM = {CRGB::Blue     ,  CRGB::Black   , 5};
const sparkleParams_t sparkleGreenPretty PROGMEM = {CRGB::White   ,  CRGB::Green    , 1};
const sparkleParams_t sparkleGreenCrazy  PROGMEM = {CRGB::White   ,  CRGB::Green    , 5};
const sparkleParams_t sparkleGreenCrazy3  PROGMEM = {CRGB::Green     ,  CRGB::Black   , 5};
const sparkleParams_t sparkleRedCrazy1  PROGMEM = {CRGB::Red     ,  CRGB::Cyan    , 3};
const sparkleParams_t sparkleRedCrazy2  PROGMEM = {CRGB::Red     ,  CRGB::Blue    , 3};
const sparkleParams_t sparkleRedCrazy3  PROGMEM = {CRGB::Red     ,  CRGB::Black   , 5};
const sparkleParams_t sparkleRandom     PROGMEM = {CRGB::Black   ,  CRGB::Black   , 2};


// ---------- moving-dot patterns ----------

// Elements:
//     fgColorCode    dot color
//     bgColorCode    background color
//     randomFgColor  true if the foreground color should be selected at random
//     randomBgColor  true if the background color should be selected at random
//     changeFgColor  true if the foreground color should be re-selected at random for each movement cycle
//     zipperBg;      zipper effect:  set the bg color when moving away from pixel 0 then clear it when moving toward pixel 0
//     bidirectional  true if the dot should bounce back and forth
//     delay0         approx. ms delay between dot steps when moving away from pixel 0 (0-255)
//     delay1         approx. ms delay between dot steps when moving toward pixel 0 (0-255)
//     holdTimeFarMs  number of ms to hold the dot at the far end before it moves back toward pixel 0 (0-255)
//     holdTimeNearMs number of ms to hold the dot at the near end before it moves toward the last pixel

//                                                       fg           bg           randFg randBg chngFg zipper biDir  d0  d1  hFar hNear
const movingDotParams_t movingDotRandomPong   PROGMEM = {CRGB::Black, CRGB::Black,  true, false, false, false, true ,  1,  1,  0,     0};
const movingDotParams_t movingDotRandomZipper PROGMEM = {CRGB::Black, CRGB::Black,  true,  true,  true,  true, true , 20,  1, 500, 1500};
const movingDotParams_t movingDotRandomPaint  PROGMEM = {CRGB::Black, CRGB::Black,  true,  true,  true,  true, false, 10,  1,   0,    0};
const movingDotParams_t movingDotRandomShoot  PROGMEM = {CRGB::Black, CRGB::Black,  true, false,  true, false, false,  1,  1,   0,    0};


// ---------- split-rotation patterns ----------

// Elements:
//     randomHue      true if the hues should be selected at random
//     fgHue          foreground (moving pixels) hue
//     bgHue          background hue
//     fgInterval     number of background pixels minus one between each moving pixel
//     directionDown  true to rotate one way, false to rotate the other way
//     delay          approx. ms delay between rotation steps (0-255)

const splitRotationParams_t splitRotationRandom4RSlow   PROGMEM = {true , HUE_RED           , HUE_RED           , 4, false, 120};
const splitRotationParams_t splitRotationRandom4LSlow   PROGMEM = {true , HUE_RED           , HUE_RED           , 4, true , 120};
const splitRotationParams_t splitRotationRandom4RMedium PROGMEM = {true , HUE_RED           , HUE_RED           , 4, false,  60};
const splitRotationParams_t splitRotationRandom4LMedium PROGMEM = {true , HUE_RED           , HUE_RED           , 4, true ,  60};
const splitRotationParams_t splitRotationRandom4RFast   PROGMEM = {true , HUE_RED           , HUE_RED           , 4, false,  30};
const splitRotationParams_t splitRotationRandom4LFast   PROGMEM = {true , HUE_RED           , HUE_RED           , 4, true ,  30};
const splitRotationParams_t splitRotationRandom4RDizzy  PROGMEM = {true , HUE_RED           , HUE_RED           , 4, false,  15};
const splitRotationParams_t splitRotationRandom4LDizzy  PROGMEM = {true , HUE_RED           , HUE_RED           , 4, true ,  15};
const splitRotationParams_t splitRotationRandom8RSlow   PROGMEM = {true , HUE_RED           , HUE_RED           , 8, false, 120};
const splitRotationParams_t splitRotationRandom8LSlow   PROGMEM = {true , HUE_RED           , HUE_RED           , 8, true , 120};
const splitRotationParams_t splitRotationRandom8RMedium PROGMEM = {true , HUE_RED           , HUE_RED           , 8, false,  60};
const splitRotationParams_t splitRotationRandom8LMedium PROGMEM = {true , HUE_RED           , HUE_RED           , 8, true ,  60};
const splitRotationParams_t splitRotationRandom8RFast   PROGMEM = {true , HUE_RED           , HUE_RED           , 8, false,  30};
const splitRotationParams_t splitRotationRandom8LFast   PROGMEM = {true , HUE_RED           , HUE_RED           , 8, true ,  30};
const splitRotationParams_t splitRotationRandom8RDizzy  PROGMEM = {true , HUE_RED           , HUE_RED           , 8, false,  15};
const splitRotationParams_t splitRotationRandom8LDizzy  PROGMEM = {true , HUE_RED           , HUE_RED           , 8, true ,  15};


// ---------- rainbow patterns ----------

// Elements:
//     directionDown  true to rotate one way, false to rotate the other way
//     delay          approx. ms delay between rotation steps (0-255)

const rainbowParams_t rainbowSlowSoothing PROGMEM = {false, 10};
const rainbowParams_t rainbowManicRight   PROGMEM = {false,  0};
const rainbowParams_t rainbowManicLeft    PROGMEM = {true ,  0};



/***********************
 *   T H E   S H O W   *
 ***********************/

// Elements in each pattern element:
//     pattern function
//     duration (number of seconds)
//     pattern configuration (prefix with ampersand)

const patternDef_t patternsDefs[] PROGMEM = {

  {RAINBOW       ,  30,  &rainbowManicRight},
  
  {MULTI_WAVE    ,  20,  &multiWave1Random},

  {SPLIT_ROTATION,  20,  &splitRotationRandom8RFast},
 
  {MOVING_DOT    ,  20,  &movingDotRandomPong},

  {SPLIT_ROTATION,  20,  &splitRotationRandom8RMedium},

  {SPARKLE       ,  12,  &sparkleBluePretty},

  {SPLIT_ROTATION,  20,  &splitRotationRandom8RMedium},

  {MULTI_WAVE    ,  40,  &multiWave1Random},

  {SPARKLE       ,  12,  &sparkleGreenCrazy3},

  {SPLIT_ROTATION,  10,  &splitRotationRandom8RDizzy},

  {SPARKLE       ,   1,  &sparkleRandom},
  {SPARKLE       ,   1,  &sparkleRandom},
  {SPARKLE       ,   1,  &sparkleRandom},
  {SPARKLE       ,   1,  &sparkleRandom},
  {SPARKLE       ,   1,  &sparkleRandom},
  {SPARKLE       ,   1,  &sparkleRandom},
  {SPARKLE       ,   1,  &sparkleRandom},
  {SPARKLE       ,   1,  &sparkleRandom},
  {SPARKLE       ,   1,  &sparkleRandom},
  {SPARKLE       ,   1,  &sparkleRandom},
  {SPARKLE       ,   1,  &sparkleRandom},
  {SPARKLE       ,   1,  &sparkleRandom},
  {SPARKLE       ,   1,  &sparkleRandom},
  {SPARKLE       ,   1,  &sparkleRandom},
  {SPARKLE       ,   1,  &sparkleRandom},
  {SPARKLE       ,   1,  &sparkleRandom},
  {SPARKLE       ,   1,  &sparkleRandom},
  {SPARKLE       ,   1,  &sparkleRandom},
  {SPARKLE       ,   1,  &sparkleRandom},
  {SPARKLE       ,   1,  &sparkleRandom},

  {MULTI_WAVE    ,  30,  &multiWave3ClusterFuck},

  {SPARKLE       ,  12,  &sparkleBlueCrazy},

  {MULTI_WAVE    ,  30,  &multiWave3SquareA},

  {SPLIT_ROTATION,  10,  &splitRotationRandom4RDizzy},
  
  {MOVING_DOT    ,  44,  &movingDotRandomZipper},

  {MULTI_WAVE    ,  60,  &multiWave3 },

  {SPLIT_ROTATION,  10,  &splitRotationRandom8RFast},

  {SPARKLE       ,   5,  &sparkleRedCrazy1},
  {SPARKLE       ,   5,  &sparkleRedCrazy2},
  {SPARKLE       ,  15,  &sparkleRedCrazy3},

  {MOVING_DOT    ,  30,  &movingDotRandomPaint},

  {RAINBOW       ,  30,  &rainbowManicLeft},
  
  {SPLIT_ROTATION,  10,  &splitRotationRandom4RFast},
  
  {MULTI_WAVE    ,  30,  &multiWaveBsu},

  {SPARKLE       ,  20,  &sparkleBlueCrazy},

  {MOVING_DOT    ,  30,  &movingDotRandomShoot},

  {MULTI_WAVE    ,  30,  &multiWave3SquareB},

  {SPLIT_ROTATION,  20,  &splitRotationRandom4RMedium},

  {SPARKLE       ,  12,  &sparkleGreenCrazy},

  {MULTI_WAVE    ,  30,  &multiWave2Random},

  {MOVING_DOT    ,  20,  &movingDotRandomPaint},

  {SPARKLE       ,  12,  &sparkleBlueCrazy3},

  {SPLIT_ROTATION,  10,  &splitRotationRandom8RDizzy},

};


// Calculate the number of patterns that make up the entire show.
#define NUM_PATTERNS (sizeof(patternsDefs) / sizeof(patternDef_t))



/*****************************
 * Pattern Drawing Functions *
 *****************************/


void rainbow(patternDef_t* patParams)
{
  static rainbowParams_t* pp;
  static uint8_t stepNum;

  uint8_t i;

  if (patParams) {
    pp = (rainbowParams_t*) patParams;
    stepNum = 0;
    return;
  }

  fill_rainbow(pixels, PANEL_NUM_PIXELS, stepNum, 255 / PANEL_NUM_PIXELS);
  replicatePixelPanels();

  for (i = 0; i < NUM_PIXELS; pixels[i++].nscale8_video(bgIntensityScaleFactor));

  FastLED.show();

  if (pp->directionDown) {
    ++stepNum;
  }
  else {
    --stepNum;
  }
  delay(pp->delay);

}



void multiWave(patternDef_t* patParams)
{
  static multiWaveParams_t* pp;
  static uint8_t numWaveforms;
  static HSVHue posHue[MAX_WAVEFORMS];
  static HSVHue negHue[MAX_WAVEFORMS];
  static uint16_t angleInterval[MAX_WAVEFORMS];
  static uint16_t i0[MAX_WAVEFORMS];
  static uint8_t delayCount[MAX_WAVEFORMS];

  uint8_t w;
  colorWaveParams_t* waveParam;
  uint8_t i;
  uint8_t j;
  uint16_t t;
  uint8_t y;
  CHSV hsvColor;
  CHSV hsvBlended;
  uint8_t amountOfOverlay;
  uint8_t valueSum;
  CRGB rgbColor;
  uint8_t scaleFactor;
  bool needToDisplay;


  if (patParams) {
    pp = (multiWaveParams_t*) patParams;

    numWaveforms = 0;
    
    // Calculate and save the LED values for each wave at t = 0.    
    for (w = 0; w < MAX_WAVEFORMS; ++w)
    {
      waveParam = pp->waveParams + w;

      if (0 == waveParam->numWaves) {
        break;
      }
      ++numWaveforms;

      if (waveParam->randomHue) {
        selectRandomHue(&posHue[w], &negHue[w]);
      }
      else {
        posHue[w] = waveParam->posHue;
        negHue[w] = waveParam->negHue;
      }
  
      // We use 16 bits for the angle interval and "time" so that we have sufficient
      // resultion to fit a complete set of the requested number of waves.
      angleInterval[w] = 65535 / (pp->boundedByPanel ? PANEL_NUM_PIXELS : NUM_PIXELS) * abs(waveParam->numWaves);

      i0[w] = 0;
      
      delayCount[w] = waveParam->delay;
    }
    
    return;
  }


  needToDisplay = false;
  for (w = 0; w < numWaveforms; ++w) {
    if (0 == --delayCount[w]) {
      delayCount[w] = pp->waveParams[w].delay;
      needToDisplay = true;
      if (pp->waveParams[w].directionDown) {
        if (++i0[w] >= (pp->boundedByPanel ? PANEL_NUM_PIXELS : NUM_PIXELS)) {
          i0[w] = 0;
        }
      }
      else {
        if (0 == i0[w]--) {
          i0[w] = pp->boundedByPanel ? PANEL_NUM_PIXELS - 1 : NUM_PIXELS - 1;
        }
      }
    }
  }
  if (!needToDisplay) {
    delay(1);
    return;
  }

    
  for (i = 0; i < (pp->boundedByPanel ? PANEL_NUM_PIXELS : NUM_PIXELS); ++i) {

    for (w = 0; w < numWaveforms; ++w) {

      t = ((uint16_t) i + i0[w]) * angleInterval[w] / 256;
      switch (pp->waveParams[w].waveformType) {
        case SQUARE_WAVE:
          y = triwave8(t) >= 128 ? 255 : 0;
          break;
        case TRIANGLE_WAVE:
          y = triwave8(t);
          break;
        case QUADRATIC_EASING:
          y = quadwave8(t);
          break;
        case CUBIC_EASING:
          y = cubicwave8(t);
          break;
        case SINE_WAVE:
        default:
          y = sin8(t);
          break;
      }
      if (y >= 128) {
        hsvColor.h = posHue[w];
        hsvColor.v = (y - 128) * 2;
      }
      else {
        hsvColor.h = negHue[w];
        // A negative number of waves means make a half wave.
        hsvColor.v = (pp->waveParams[w].numWaves >= 0) ? (127 - y) * 2 : 0;
      }
      hsvColor.s = 255;
      
      if (0 == w) {
        hsvBlended = hsvColor;
      }
      else {
        amountOfOverlay = 255 * (uint16_t) hsvBlended.v / ((uint16_t) hsvColor.v + (uint16_t) hsvBlended.v);
        valueSum = qadd8(hsvBlended.v, hsvColor.v);
        nblend(hsvBlended, hsvColor, amountOfOverlay);
        hsvBlended.v = valueSum;
      }
    }
    
    hsv2rgb_rainbow(hsvBlended, pixels[i]);
    pixels[i].nscale8_video(bgIntensityScaleFactor);
  }

  if (pp->boundedByPanel) {
    replicatePixelPanels();
  }
  
  FastLED.show();
}



void movingDot(patternDef_t* patParams)
{
  static movingDotParams_t* pp;
  static uint8_t stepNum;
  static uint8_t stepDir;    // 0 = away from pixel 0, 1 = toward pixel 0, 2 = hold at far end, 3 = hold at near end
  static unsigned long holdEndMs = 0;
  static CRGB fgColor;
  static CRGB bgColor;

  CRGB rgbColor;
  uint8_t prevStepNum;
  uint8_t prevStepDir;
  uint8_t j;

  if (patParams) {
    pp = (movingDotParams_t*) patParams;
    
    stepNum = 0;
    stepDir = 0;

    selectRandomRgb(&fgColor, &bgColor);
    if (!pp->randomFgColor) {
        fgColor = pp->fgColorCode;
    }
    if (!pp->randomBgColor) {
        bgColor = pp->bgColorCode;
    }
    bgColor.nscale8_video(bgIntensityScaleFactor);

    if (!pp->zipperBg) {
      fill_solid(pixelArray, NUM_PIXELS, bgColor);
    }

    return;
  }

  prevStepNum = stepNum;
  prevStepDir = stepDir;
  
  switch (stepDir) {

    // moving away from pixel 0
    case 0:
      if (++stepNum >= PANEL_NUM_PIXELS) {
        stepDir = 2;
        stepNum = PANEL_NUM_PIXELS - 1;
        holdEndMs = pp->holdTimeFarMs;
        if (holdEndMs != 0) {
          holdEndMs += millis();
        }
      }
      break;

    // moving toward pixel 0
    case 1:
      if (--stepNum == 0) {
        stepDir = 3;
        holdEndMs = pp->holdTimeNearMs;
        if (holdEndMs != 0) {
           holdEndMs += millis();
        }
      }
      break;

    // hold at far end
    case 2:
      if (0 == holdEndMs || millis() >= holdEndMs) {
        if (pp->bidirectional) {
          stepDir = 1;
          stepNum = PANEL_NUM_PIXELS - 2;
        }
        else {
          stepDir = 0;
          stepNum = 0;
        }
      }
      break;

    // hold at near end
    case 3:
      if (0 == holdEndMs || millis() >= holdEndMs) {
        stepDir = 0;
      }
      break;
  }

  if (0 == stepDir && 0 != prevStepDir && pp->changeFgColor) {
    if (pp->zipperBg) {
      // When doing the zipper effect, change the background color, too.
      selectRandomRgb(&fgColor, &bgColor);
      bgColor.nscale8_video(bgIntensityScaleFactor);
    }
    else {
      selectRandomRgb(&fgColor, NULL);
    }
  }

  if (stepNum != prevStepNum) {

    rgbColor = ((stepDir == 1 || prevStepDir == 1) && pp->zipperBg) ? CRGB::Black : bgColor;
    pixels[prevStepNum] = rgbColor;
    for (j = 1; j < NUM_PANELS; ++j) {
      pixels[prevStepNum + PANEL_NUM_PIXELS * j] = rgbColor;
    }

    pixels[stepNum] = fgColor;
    for (j = 1; j < NUM_PANELS; ++j) {
      pixels[stepNum + PANEL_NUM_PIXELS * j] = fgColor;
    }
    
    FastLED.show();
    delay(stepDir ? pp->delay1 : pp->delay0);
  }
  
}



void sparkle(patternDef_t* patParams)
{
  static sparkleParams_t* pp;
  static uint8_t density;
  static uint8_t selectedPixels[MAX_SPARKLE_DENSITY];
  static CRGB fgColor;
  static CRGB bgColor;

  uint8_t i;
  
  if (patParams) {
    pp = (sparkleParams_t*) patParams;
    
    density = pp->density <= MAX_SPARKLE_DENSITY ? pp->density : MAX_SPARKLE_DENSITY;
    for (i = 0; i < density; ++i) {
      selectedPixels[i] = 0;
    }

    if (pp->fgColorCode != CRGB::Black || pp->bgColorCode != CRGB::Black) {
      fgColor = pp->fgColorCode;
      bgColor = pp->bgColorCode;
    }
    else {
      selectRandomRgb(&fgColor, &bgColor);
    }
    bgColor.nscale8_video(bgIntensityScaleFactor);
    
    // Fill with background color.
    fill_solid(pixels, NUM_PIXELS, bgColor);
    FastLED.show();
    return;
  }

  for (i = 0; i < density; ++i) {
    // Turn off the last sparkle pixel.
    pixels[selectedPixels[i]] = bgColor;
  
    // Turn on a random sparkle pixel.
    selectedPixels[i] = random8(NUM_PIXELS);
    pixels[selectedPixels[i]] = fgColor;
  }

  FastLED.show();
  delay(SPARKLE_DELAY);
}



void splitRotation(patternDef_t* patParams)
{
  static splitRotationParams_t* pp;
  
  uint8_t i;
  HSVHue fgHue;
  HSVHue bgHue;
  CHSV fgHsv;
  CHSV bgHsv;
  CRGB rgbColor;
  

  if (patParams) {
    pp = (splitRotationParams_t*) patParams;

    if (pp->randomHue) {
      selectRandomHue(&fgHue, &bgHue);
    }
    else {
      fgHue = pp->fgHue;
      bgHue = pp->bgHue;
    }
    fgHsv.h = fgHue;
    fgHsv.s = 255;
    fgHsv.v = 255;
    bgHsv.h = bgHue;
    bgHsv.s = 255;
    bgHsv.v = 255;

    // Fill with background color.
    hsv2rgb_rainbow(bgHsv, rgbColor);
    rgbColor.nscale8_video(bgIntensityScaleFactor);
    fill_solid(pixels, PANEL_NUM_PIXELS, rgbColor);

    // Set every nth pixel to the foreground color.
    hsv2rgb_rainbow(fgHsv, rgbColor);
    rgbColor.nscale8_video(bgIntensityScaleFactor);
    for (i = 0; i < PANEL_NUM_PIXELS; i += pp->fgInterval) {
        pixels[i] = rgbColor;
    }

    replicatePixelPanels();

    FastLED.show();

    return;
  }


  i = PANEL_NUM_PIXELS / 4;
  if (pp->directionDown) {
    rotateCrgbLeft(pixels, i);
    rotateCrgbRight(pixels + i, i);
    rotateCrgbRight(pixels + i * 3, i);
    rotateCrgbLeft(pixels + i * 2, i);
  }
  else {
    rotateCrgbRight(pixels, i);
    rotateCrgbLeft(pixels + i, i);
    rotateCrgbLeft(pixels + i * 3, i);
    rotateCrgbRight(pixels + i * 2, i);
  }

  replicatePixelPanels();

  FastLED.show();

  delay(pp->delay);
}



/***********
 * Helpers *
 ***********/


void selectRandomRgb(CRGB* rgbColor, CRGB* rgbColorInverse)
{
  CHSV hsvColor;

  // Choose a fully saturated random color.
  hsvColor.h = random(256);
  hsvColor.s = 255;
  hsvColor.v = 255;
  hsv2rgb_rainbow(hsvColor, *rgbColor);
  
  if (NULL != rgbColorInverse) {
    switch (random(6)) {
      case 0:
        hsvColor.h += 64;    // 1/4 the way around the color wheel
        break;
      case 1:
        hsvColor.h += 85;    // 1/3 the way around the color wheel
        break;
      case 2:
        hsvColor.h -= 64;    // 1/4 the other way around the color wheel
        break;
      case 3:
        hsvColor.h -= 85;    // 1/3 the other way around the color wheel
        break;
      default:
        hsvColor.h += 128;   // complementary color
        break;
    }
    hsv2rgb_rainbow(hsvColor, *rgbColorInverse);
  }
}


void selectRandomHue(HSVHue* hue, HSVHue* hueInverse)
{
  uint8_t hueValue;

  // Choose a fully saturated random color.
  hueValue = random(256);
  *hue = (HSVHue) hueValue;
  
  if (NULL != hueInverse) {
    switch (random(6)) {
      case 0:
        hueValue += 64;    // 1/4 the way around the color wheel
        break;
      case 1:
        hueValue += 85;    // 1/3 the way around the color wheel
        break;
      case 2:
        hueValue -= 64;    // 1/4 the other way around the color wheel
        break;
      case 3:
        hueValue -= 85;    // 1/3 the other way around the color wheel
        break;
      default:
        hueValue += 128;   // complementary color
        break;
    }
    *hueInverse = (HSVHue) hueValue;
  }
}


void rotateCrgbRight(CRGB* a, uint8_t length)
{
  uint8_t i;
  CRGB temp;

  temp = a[length - 1];

  for (i = length - 1; i > 0; --i) {
    a[i] = a[i - 1];
  }

  a[0] = temp;
}


void rotateCrgbLeft(CRGB* a, uint8_t length)
{
  uint8_t i;
  CRGB temp;

  temp = a[0];

  for (i = 0; i < length - 1; ++i) {
    a[i] = a[i + 1];
  }

  a[length - 1] = temp;
}


void replicatePixelPanels()
{
  if (NUM_PANELS <= 1)
    return;
  
  uint8_t i;
  uint8_t j;
  
  for (i = 0; i < PANEL_NUM_PIXELS; ++i) {
    for (j = 1; j < NUM_PANELS; ++j) {
      pixels[i + PANEL_NUM_PIXELS * j] = pixels[i];
    }
  }
}


uint16_t freeRam() 
{
  // Based on code retrieved on 1 April 2015 from
  // https://learn.adafruit.com/memories-of-an-arduino/measuring-free-memory

  extern int __heap_start, *__brkval; 
  int v; 
  return (uint16_t) &v - (__brkval == 0 ? (uint16_t) &__heap_start : (uint16_t) __brkval); 
}


void checkVBatt()
{
  static bool filterIsFull = false;
  static unsigned int vBattAdcReadings[VBATT_FILTER_LENGTH];
  static unsigned int vBattSum;
  static byte oldestReadingIdx;
  
//#define VBATT_READ_INTERVAL_MS 250
//#define VBATT_FILTER_LENGTH 8  // must be a power of 2
//#define VBATT_FILTER_DIV_SHIFT 3
//#define VBATT_LOW_ADC_READING 575       // 9.0 V:  1023 * (9.0 / 16.0) = 575
//#define VBATT_SHUTDOWN_ADC_READING 537  // 8.4 V:  1023 * (8.4 / 16.0) = 537

  unsigned int vBattAdcReading = analogRead(VBATT_APIN);

#ifdef DEBUG_SERIAL_PRINT
  Serial.println(vBattAdcReading);
#endif

  // Initialize the filter by filling it with the first reading.
  if (!filterIsFull) {
    filterIsFull = true;
    for (byte i = 0; i < VBATT_FILTER_LENGTH; vBattAdcReadings[i++] = vBattAdcReading);
    vBattSum = vBattAdcReading * VBATT_FILTER_LENGTH;
    oldestReadingIdx = 0;
  }
  
  vBattSum = vBattSum - vBattAdcReadings[oldestReadingIdx] + vBattAdcReading;
  vBattAdcReadings[oldestReadingIdx] = vBattAdcReading;
  if (++oldestReadingIdx == VBATT_FILTER_LENGTH)
    oldestReadingIdx = 0;
  unsigned int vBattFilteredAdcReading = vBattSum >> VBATT_FILTER_DIV_SHIFT;

#ifdef DEBUG_SERIAL_PRINT
  Serial.println(vBattFilteredAdcReading);
#endif

  if (vBattFilteredAdcReading <= VBATT_SHUTDOWN_ADC_READING) {
    lowBatteryShutdown = true;
  }
  if (vBattFilteredAdcReading <= VBATT_LOW_ADC_READING) {
    bgIntensityScaleFactor = BG_INTENSITY_SCALE_FACTOR_LOW;
  }

}



/***********************
 * Setup and Main Loop *
 ***********************/


void setup()
{
  pinMode(nPIXEL_STRIP_PWR_ON_PIN, OUTPUT);
  pinMode(RELAY_ON_PIN, OUTPUT);
  pinMode(PUSHBUTTON_PIN, INPUT_PULLUP);
  pinMode(ONBOARD_LED_PIN, OUTPUT);
  
  // Initialize outputs:  onboard LED on, pixel power off, relay off.
  digitalWrite(ONBOARD_LED_PIN, 1);
  digitalWrite(nPIXEL_STRIP_PWR_ON_PIN, 1);
  digitalWrite(RELAY_ON_PIN, 0);

  FastLED.addLeds<WS2812B, PIXEL_DATA_PIN, GRB>(pixelArray, NUM_PHYSICAL_PIXELS);  // white pixel strips

  // Initialize the pixel array so that any skipped pixels will remain off.
  fill_solid(pixelArray, NUM_PIXELS, CRGB::Black);

  // TODO:  use a #define for the unconnected analog pin
  // Analog input 1 should be disconnected, making it a good source of
  // random noise with which we can seed the random number generator.
  randomSeed(analogRead(1));

#ifdef DEBUG_SERIAL_PRINT
  Serial.begin(9600);
  Serial.println("Starting");
#endif

  // Initialize power management.
  lowBatteryShutdown = false;
  bgIntensityScaleFactor = BG_INTENSITY_SCALE_FACTOR;

  // Let things settle down a bit then turn on the pixel strip power.
  delay(250);
  digitalWrite(nPIXEL_STRIP_PWR_ON_PIN, 0);
  delay(250);

  // Do an R, G, B self test.
  fill_solid(pixels, NUM_PIXELS, CRGB::Red);
  for (uint8_t i = 0; i < NUM_PIXELS; pixels[i++].nscale8_video(bgIntensityScaleFactor));
  FastLED.show();
  delay(500);
  fill_solid(pixels, NUM_PIXELS, CRGB::Green);
  for (uint8_t i = 0; i < NUM_PIXELS; pixels[i++].nscale8_video(bgIntensityScaleFactor));
  FastLED.show();
  delay(500);
  fill_solid(pixels, NUM_PIXELS, CRGB::Blue);
  for (uint8_t i = 0; i < NUM_PIXELS; pixels[i++].nscale8_video(bgIntensityScaleFactor));
  FastLED.show();
  delay(500);
  fill_solid(pixels, NUM_PIXELS, CRGB::Black);
  FastLED.show();
  delay(500);

  // Using the pixels, display the relative amount of free RAM.
  uint32_t numFreeBytes = freeRam();
#ifdef DEBUG_SERIAL_PRINT
  Serial.println(numFreeBytes);
#endif
  uint32_t memDisplayNumPixelsOn = numFreeBytes * NUM_PIXELS / 2048;
  fill_solid(pixels, memDisplayNumPixelsOn, CRGB::Cyan);
  for (uint8_t i = 0; i < memDisplayNumPixelsOn; pixels[i++].nscale8_video(bgIntensityScaleFactor));
  FastLED.show();
  delay(2000);
  fill_solid(pixels, NUM_PIXELS, CRGB::Black);
  FastLED.show();
}


void loop()
{
  // power control
  static unsigned long nextVBattCheckMs = 0;
  
  // pattern control
  static uint8_t patternNum = -1;
  static unsigned long nextPatternChangeMs = 0;
  static void (*loopFun)(patternDef_t*);
  static byte patParamBuf[PAT_PARAMS_BUF_SIZE];  // used by pattern fn; must remain intact between calls to same fn
  const patternDef_t* patDef;
  uint8_t patternType;
  unsigned long durationSec;
  patternDef_t* patParams;
  uint8_t paramStructSize;

  // Do power management.
  if (lowBatteryShutdown) {
    // TODO:  Need to implement some sort of low-power sleep.
    return;
  }
  if (millis() > nextVBattCheckMs) {
    nextVBattCheckMs = millis() + VBATT_READ_INTERVAL_MS;
    checkVBatt();
    if (lowBatteryShutdown) {
      // Turn everything off.
      digitalWrite(ONBOARD_LED_PIN, 0);
      digitalWrite(nPIXEL_STRIP_PWR_ON_PIN, 1);
      digitalWrite(RELAY_ON_PIN, 0);
      return;
    }
  }

///  bool hsvMode = !digitalRead(MODE_PIN);

  if (millis() < nextPatternChangeMs) {
    (*loopFun)(NULL);
  }
  else {
    if (++patternNum >= NUM_PATTERNS) {
      patternNum = 0;
    }

    patDef = patternsDefs + patternNum;

    // Get the pattern definition from flash.
    patternType = (uint8_t) pgm_read_byte(&patDef->patternType);
    durationSec = (unsigned long) pgm_read_dword(&patDef->durationSec);
    patParams = (patternDef_t*) pgm_read_word(&patDef->patParams);

    // Get the pattern metadata.
    paramStructSize = patternMetadataTable[patternType].paramStructSize;
    loopFun = patternMetadataTable[patternType].loopFun;

    // If the pattern params buffer isn't large enough, turn all
    // pixels red for one second then go on to the next pattern.
    if (paramStructSize > PAT_PARAMS_BUF_SIZE) {
      fill_solid(pixels, NUM_PIXELS, CRGB::Red);
      FastLED.show();
      delay(1000);
      fill_solid(pixels, NUM_PIXELS, CRGB::Black);
      FastLED.show();
      nextPatternChangeMs = millis() - 1;
      return;
    }

    // Get the pattern parameters from flash.    
    memcpy_P(patParamBuf, patParams, paramStructSize);

    nextPatternChangeMs = millis() + durationSec * 1000;
    
    fill_solid(pixels, NUM_PIXELS, CRGB::Black);
    FastLED.show();

    (*loopFun)((patternDef_t*) patParamBuf);
  }  
}

