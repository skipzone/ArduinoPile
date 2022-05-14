/*****************************************************************
 *                                                               *
 * Addressable LED Pixel Driver for Tripper Stick II             *
 *                                                               *
 * Platform:  Arduino Uno, Pro, Pro Mini                         *
 *                                                               *
 * Architecture and pattern functions by Ross Butler, Mar. 2015  *
 *                                                               *
 *****************************************************************/


#include <avr/pgmspace.h>

#define ENABLE_DMX_AMBIENCE_LIGHTS

#ifdef ENABLE_DMX_AMBIENCE_LIGHTS
//  #define FASTSPI_USE_DMX_SIMPLE
  #define DMX_SIZE 27
  #include <DmxSimple.h>
#endif

#include "FastLED.h"


/***************************
 * Configuration Constants *
 ***************************/

#define PIXEL_DATA_PIN 2
#define DMX_DATA_PIN 3
#define MODE_PUSHBUTTON_PIN 12
#define ONBOARD_LED_PIN 13

// the total number of pixels in the strip
#define NUM_PHYSICAL_PIXELS 146

// the number of pixels at the start of the strip that should remain unused (off)
#define NUM_SKIP_PIXELS 0

// the number of active pixels
#define NUM_PIXELS (NUM_PHYSICAL_PIXELS - NUM_SKIP_PIXELS)

// the number of pixels attached to each panel
// (must be a factor of the number of physical pixels)
#define NUM_PANELS 1
#define PANEL_NUM_PIXELS (NUM_PIXELS / NUM_PANELS)

// BG_INTENSITY_SCALE_FACTOR sets the maximum brightness for backgrounds.  It also
// sets the brightness of the foreground of patterns that aren't doing some sort
// of flashing or sparkle effect.  Although the range is 0 to 255, practical
// values are more like 16 to 128.  48 is pretty darn bright in a darkened room.
#define BG_INTENSITY_SCALE_FACTOR 96

// MAX_WAVEFORMS is the maximum number of waveforms
// that can be configured for a multiWave pattern.
#define MAX_WAVEFORMS 3

// MAX_SPARKLE_DENSITY is the maximum number of foreground pixels
// that can be illuminated simultaneously in a sparkle pattern.
#define MAX_SPARKLE_DENSITY 15

// Approximate millisecond delay between sparkle changes.
#define SPARKLE_DELAY 15

#define PUSHBUTTON_DEBOUNCE_INTERVAL_MS 100

// colors for black WS2811S 12V pixel strips purchased from HolidayCoro 2014 presale.
#define BSU_BLUE           (CRGB::HTMLColorCode) 0x0000FF
#define BSU_ORANGE         (CRGB::HTMLColorCode) 0xD03000  // not tested--probably needs more red
#define HUE_BSU_BLUE       (HSVHue) 160
#define HUE_BSU_ORANGE     (HSVHue) 22

// These DMX configuration values control the ambiance lighting.
// They work like their similarly named pixel counterparts.
// They are ignored if ENABLE_DMX_AMBIENCE_LIGHTS is not defined.
#define NUM_DMX_RGB_CHANNELS 9          // each DMX channel is essentially one RGB led or strip
#define NUM_DMX_PANELS 2
#define DMX_PANEL_NUM_RGB_CHANNELS 3    // use the first 6 channels for 2 panels
#define DMX_AMBIANCE_INTENSITY_SCALE_FACTOR 48



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


// This structure holds the configuration of the Red White and Blue (RWaB) pattern.
typedef struct rwabParams rwabParams_t;
struct rwabParams {
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
void redWhiteAndBlue(patternDef_t* patParams);


// These are the pattern types.  The values are indexes into the pattern metadata table.
#define MOVING_DOT         0
#define MULTI_WAVE         1
#define RAINBOW            2
#define SPARKLE            3
#define SPLIT_ROTATION     4
#define RED_WHITE_AND_BLUE 5


// Each pattern metadata entry must be in the element
// position corresponding to its pattern type constant.
patternMetadata_t patternMetadataTable[] = {
  {sizeof(movingDotParams_t)    , movingDot},
  {sizeof(multiWaveParams_t)    , multiWave},
  {sizeof(rainbowParams_t)      , rainbow},
  {sizeof(sparkleParams_t)      , sparkle},
  {sizeof(splitRotationParams_t), splitRotation},
  {sizeof(rwabParams_t)         , redWhiteAndBlue},
};


// the size of the buffer used when copying pattern parameters from flash to RAM
#define PAT_PARAMS_BUF_SIZE 32



/********************************
 * Helper Function Declarations *
 ********************************/

void selectRandomRgb(CRGB* rgbColor, CRGB* rgbColorInverse);
void selectRandomHue(HSVHue* hue, HSVHue* hueInverse);
void rotateCrgbRight(CRGB* a, uint16_t length);
void rotateCrgbLeft(CRGB* a, uint16_t length);
void replicatePixelPanels();



/***********
 * Globals *
 ***********/

CRGB pixelArray[NUM_PHYSICAL_PIXELS];
CRGB* pixels = pixelArray + NUM_SKIP_PIXELS;

#ifdef ENABLE_DMX_AMBIENCE_LIGHTS
CRGB dmxRgbArray[NUM_DMX_RGB_CHANNELS];
#endif



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
const sparkleParams_t sparkleBlueCrazy  PROGMEM = {CRGB::White   ,  CRGB::Blue    , MAX_SPARKLE_DENSITY};
const sparkleParams_t sparkleBlueCrazy3  PROGMEM = {CRGB::Blue     ,  CRGB::Black   , MAX_SPARKLE_DENSITY};
const sparkleParams_t sparkleGreenPretty PROGMEM = {CRGB::White   ,  CRGB::Green    , 1};
const sparkleParams_t sparkleGreenCrazy  PROGMEM = {CRGB::White   ,  CRGB::Green    , MAX_SPARKLE_DENSITY};
const sparkleParams_t sparkleGreenCrazy3  PROGMEM = {CRGB::Green     ,  CRGB::Black   , MAX_SPARKLE_DENSITY};
const sparkleParams_t sparkleRedCrazy1  PROGMEM = {CRGB::Red     ,  CRGB::Cyan    , 3};
const sparkleParams_t sparkleRedCrazy2  PROGMEM = {CRGB::Red     ,  CRGB::Blue    , 3};
const sparkleParams_t sparkleRedCrazy3  PROGMEM = {CRGB::Red     ,  CRGB::Black   , MAX_SPARKLE_DENSITY};
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
const movingDotParams_t movingDotRandomLift   PROGMEM = {CRGB::Black, CRGB::Black,  true, false, false, false, true , 30,  1, 500, 1500};
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

const splitRotationParams_t splitRotationRed8RFast      PROGMEM = {false, HUE_RED           , HUE_AQUA          , 8, false,  30};
const splitRotationParams_t splitRotationRandom4RSlow   PROGMEM = {true , HUE_RED           , HUE_RED           , 4, false, 120};
const splitRotationParams_t splitRotationRandom4LSlow   PROGMEM = {true , HUE_RED           , HUE_RED           , 4, true , 120};
const splitRotationParams_t splitRotationRandom8RSlow   PROGMEM = {true , HUE_RED           , HUE_RED           , 8, false, 120};
const splitRotationParams_t splitRotationRandom8LSlow   PROGMEM = {true , HUE_RED           , HUE_RED           , 8, true , 120};
const splitRotationParams_t splitRotationRandom8RMedium PROGMEM = {true , HUE_RED           , HUE_RED           , 8, false,  60};
const splitRotationParams_t splitRotationRandom8LMedium PROGMEM = {true , HUE_RED           , HUE_RED           , 8, true ,  60};
const splitRotationParams_t splitRotationRandom8RFast  PROGMEM = {true , HUE_RED           , HUE_RED           , 8, false,  30};
const splitRotationParams_t splitRotationRandom8LFast  PROGMEM = {true , HUE_RED           , HUE_RED           , 8, true ,  30};
const splitRotationParams_t splitRotationRandom8RDizzy PROGMEM = {true , HUE_RED           , HUE_RED           , 8, false,  15};
const splitRotationParams_t splitRotationRandom8LDizzy PROGMEM = {true , HUE_RED           , HUE_RED           , 8, true ,  15};


// ---------- rainbow patterns ----------

// Elements:
//     directionDown  true to rotate one way, false to rotate the other way
//     delay          approx. ms delay between rotation steps (0-255)

const rainbowParams_t rainbowSlowSoothing PROGMEM = {false, 10};
const rainbowParams_t rainbowManicRight   PROGMEM = {false,  0};
const rainbowParams_t rainbowManicLeft    PROGMEM = {true ,  0};


// ---------- Red White and Blue ----------

// Elements:
//     delay          approx. ms delay between rotation steps (0-255)

const rwabParams_t americaFuckYeah PROGMEM = {100};



/***********************
 *   T H E   S H O W   *
 ***********************/

// Elements in each pattern element:
//     pattern function
//     duration (number of seconds)
//     pattern configuration (prefix with ampersand)

const patternDef_t patternsDefs[] PROGMEM = {

//  {RED_WHITE_AND_BLUE,  30,  &americaFuckYeah},

  {MOVING_DOT    ,  20,  &movingDotRandomPaint},

  {SPARKLE      ,  10,  &sparkleBlueCrazy3},
  
  {RAINBOW      ,  20,  &rainbowManicRight},

  {MULTI_WAVE    ,  15,  &multiWave1Random},

  {SPLIT_ROTATION,  15,  &splitRotationRandom8RFast},
 
  {MOVING_DOT    ,  20,  &movingDotRandomPong},

  {SPARKLE      ,   10,  &sparkleBluePretty},

  {SPLIT_ROTATION,  15,  &splitRotationRandom8RMedium},

  {MULTI_WAVE    ,  20,  &multiWave1Random},

  {SPARKLE      ,   10,  &sparkleGreenCrazy3},

  {SPLIT_ROTATION,  10,  &splitRotationRandom8RDizzy},

  {MOVING_DOT    ,  20,  &movingDotRandomLift},

  {SPARKLE      ,   4,  &sparkleRandom},
  {SPARKLE      ,   4,  &sparkleRandom},
  {SPARKLE      ,   4,  &sparkleRandom},
  {SPARKLE      ,   4,  &sparkleRandom},
  {SPARKLE      ,   4,  &sparkleRandom},
  {SPARKLE      ,   4,  &sparkleRandom},
  {SPARKLE      ,   2,  &sparkleRandom},
  {SPARKLE      ,   2,  &sparkleRandom},
  {SPARKLE      ,   2,  &sparkleRandom},
  {SPARKLE      ,   2,  &sparkleRandom},
  {SPARKLE      ,   2,  &sparkleRandom},
  {SPARKLE      ,   2,  &sparkleRandom},
  {SPARKLE      ,   2,  &sparkleRandom},
  {SPARKLE      ,   2,  &sparkleRandom},
  {SPARKLE      ,   1,  &sparkleRandom},
  {SPARKLE      ,   1,  &sparkleRandom},
  {SPARKLE      ,   1,  &sparkleRandom},
  {SPARKLE      ,   1,  &sparkleRandom},
  {SPARKLE      ,   1,  &sparkleRandom},
  {SPARKLE      ,   1,  &sparkleRandom},
  {SPARKLE      ,   1,  &sparkleRandom},
  {SPARKLE      ,   1,  &sparkleRandom},
  {SPARKLE      ,   1,  &sparkleRandom},
  {SPARKLE      ,   1,  &sparkleRandom},
  {SPARKLE      ,   1,  &sparkleRandom},
  {SPARKLE      ,   1,  &sparkleRandom},

  {MULTI_WAVE    ,  30,  &multiWave3ClusterFuck},

  {SPARKLE      ,   15,  &sparkleBlueCrazy},

  {MULTI_WAVE    ,  30,  &multiWave3SquareA},

  {MOVING_DOT    ,  20,  &movingDotRandomZipper},

  {MULTI_WAVE    ,  30,  &multiWave3 },

  {SPARKLE      ,   10,  &sparkleRedCrazy1},
  {SPARKLE      ,   10,  &sparkleRedCrazy2},
  {SPARKLE      ,   10,  &sparkleRedCrazy3},

  {MOVING_DOT    ,  20,  &movingDotRandomPaint},

  //{MULTI_WAVE    ,  15,  &multiWaveBsu},

  {SPARKLE       ,  10,  &sparkleBlueCrazy},

  {MOVING_DOT    ,  20,  &movingDotRandomShoot},

  {MULTI_WAVE    ,  30,  &multiWave3SquareB},

  {SPARKLE      ,   10,  &sparkleGreenCrazy},

  {MULTI_WAVE    ,  30,  &multiWave2Random},

  // unicorn diarrhea
  {RAINBOW      ,   8,  &rainbowManicRight},
  {RAINBOW      ,   7,  &rainbowManicLeft},
  {RAINBOW      ,   6,  &rainbowManicRight},
  {RAINBOW      ,   5,  &rainbowManicLeft},
  {RAINBOW      ,   4,  &rainbowManicRight},
  {RAINBOW      ,   3,  &rainbowManicLeft},
  {RAINBOW      ,   2,  &rainbowManicRight},
  {RAINBOW      ,   2,  &rainbowManicLeft},
  {RAINBOW      ,   1,  &rainbowManicRight},
  {RAINBOW      ,   1,  &rainbowManicLeft},
  {RAINBOW      ,   1,  &rainbowManicRight},
  {RAINBOW      ,   1,  &rainbowManicLeft},
  {RAINBOW      ,   1,  &rainbowManicRight},
  {RAINBOW      ,   1,  &rainbowManicLeft},
  {RAINBOW      ,   1,  &rainbowManicRight},
  {RAINBOW      ,   1,  &rainbowManicLeft},

  // rotating patterns look really cool in the infinite reflections... and straight up, too, if your brain is right
  {SPLIT_ROTATION,   5,  &splitRotationRandom8RMedium},
  {SPLIT_ROTATION,   5,  &splitRotationRandom8LMedium},
  {SPLIT_ROTATION,   5,  &splitRotationRandom8RMedium},
  {SPLIT_ROTATION,   5,  &splitRotationRandom8LMedium},

  // -------------------------------------------

/*  
  // introductory twisting kaleidoscope

  // brain ramp-up
  {SPLIT_ROTATION,   5,  &splitRotationRandom8RSlow},
  {SPLIT_ROTATION,   5,  &splitRotationRandom8LSlow},
  {SPLIT_ROTATION,   5,  &splitRotationRandom8RSlow},
  {SPLIT_ROTATION,   5,  &splitRotationRandom8LSlow},

  // back it off a little, let 'em catch up with some alternating-direction rotations
  {SPLIT_ROTATION,   5,  &splitRotationRandom4RSlow},
  {SPLIT_ROTATION,   5,  &splitRotationRandom4LSlow},
  {SPLIT_ROTATION,   5,  &splitRotationRandom4RSlow},
  {SPLIT_ROTATION,   5,  &splitRotationRandom4LSlow},

  // rotating patterns look really cool in the infinite reflections... and straight up, too, if your brain is right
  {SPLIT_ROTATION,   5,  &splitRotationRandom8RMedium},
  {SPLIT_ROTATION,   5,  &splitRotationRandom8LMedium},
  {SPLIT_ROTATION,   5,  &splitRotationRandom8RMedium},
  {SPLIT_ROTATION,   5,  &splitRotationRandom8LMedium},

  // unicorn diarrhea
  {RAINBOW      ,   8,  &rainbowManicRight},
  {RAINBOW      ,   7,  &rainbowManicLeft},
  {RAINBOW      ,   6,  &rainbowManicRight},
  {RAINBOW      ,   5,  &rainbowManicLeft},
  {RAINBOW      ,   4,  &rainbowManicRight},
  {RAINBOW      ,   3,  &rainbowManicLeft},
  {RAINBOW      ,   2,  &rainbowManicRight},
  {RAINBOW      ,   2,  &rainbowManicLeft},
  {RAINBOW      ,   1,  &rainbowManicRight},
  {RAINBOW      ,   1,  &rainbowManicLeft},
  {RAINBOW      ,   1,  &rainbowManicRight},
  {RAINBOW      ,   1,  &rainbowManicLeft},

  // ramp up with some color tripping
  {MULTI_WAVE    ,  20,  &multiWave3ClusterFuck},

*/
};


// Calculate the number of patterns that make up the entire show.
#define NUM_PATTERNS (sizeof(patternsDefs) / sizeof(patternDef_t))



/*****************************
 * Pattern Drawing Functions *
 *****************************/


void redWhiteAndBlue(patternDef_t* patParams)
{
  static rwabParams_t* pp;

  if (patParams) {
    pp = (rwabParams_t*) patParams;

    CRGB rgbColor;
    uint16_t i;
    uint16_t j;
    uint16_t k;
    uint16_t pixelsPerSegment = 11;
    uint16_t numSegments = NUM_PIXELS / pixelsPerSegment;
    rgbColor = CRGB::Red;
    k = 0;
    rgbColor.nscale8_video(BG_INTENSITY_SCALE_FACTOR);
    for (i = 0; i < numSegments; ++i) {
      for (j = 0; j < pixelsPerSegment; ++j) {
        pixels[i * pixelsPerSegment + j] = rgbColor;
      }
      switch (k) {
        case 0:
          rgbColor = CRGB::White;
          k = 1;
          break;
        case 1:
          rgbColor = CRGB::Blue;
          k = 2;
          break;
        case 2:
          rgbColor = CRGB::Red;
          k = 0;
      }
      rgbColor.nscale8_video(BG_INTENSITY_SCALE_FACTOR);
    }
    return;
  }

  rotateCrgbRight(pixels, (uint16_t) NUM_PIXELS);
  FastLED.show();
  delay(pp->delay);

}


void rainbow(patternDef_t* patParams)
{
  static rainbowParams_t* pp;
  static uint16_t stepNum;

  uint16_t i;

  if (patParams) {
    pp = (rainbowParams_t*) patParams;
    stepNum = 0;
    return;
  }

  // TODO:  need to handle > 255 pixels
  fill_rainbow(pixels, PANEL_NUM_PIXELS, stepNum, PANEL_NUM_PIXELS > 255 ? 1 : 255 / PANEL_NUM_PIXELS);
  replicatePixelPanels();

  for (i = 0; i < NUM_PIXELS; pixels[i++].nscale8_video(BG_INTENSITY_SCALE_FACTOR));

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
  uint16_t i;
  uint16_t j;
  uint16_t t;
  uint16_t y;
  CHSV hsvColor;
  CHSV hsvBlended;
  uint16_t amountOfOverlay;
  uint16_t valueSum;
  CRGB rgbColor;
  uint16_t scaleFactor;
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
    pixels[i].nscale8_video(BG_INTENSITY_SCALE_FACTOR);
  }

  if (pp->boundedByPanel) {
    replicatePixelPanels();
  }
  
  FastLED.show();
}



void movingDot(patternDef_t* patParams)
{
  static movingDotParams_t* pp;
  static uint16_t stepNum;
  static uint8_t stepDir;    // 0 = away from pixel 0, 1 = toward pixel 0, 2 = hold at far end, 3 = hold at near end
  static unsigned long holdEndMs = 0;
  static CRGB fgColor;
  static CRGB bgColor;

  CRGB rgbColor;
  uint16_t prevStepNum;
  uint8_t prevStepDir;
  uint16_t j;

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
    bgColor.nscale8_video(BG_INTENSITY_SCALE_FACTOR);

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
      bgColor.nscale8_video(BG_INTENSITY_SCALE_FACTOR);
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
  static uint16_t selectedPixels[MAX_SPARKLE_DENSITY];
  static CRGB fgColor;
  static CRGB bgColor;

  uint16_t i;
  
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
    bgColor.nscale8_video(BG_INTENSITY_SCALE_FACTOR);
    
    // Fill with background color.
    fill_solid(pixels, NUM_PIXELS, bgColor);
    FastLED.show();
    return;
  }

  for (i = 0; i < density; ++i) {
    // Turn off the last sparkle pixel.
    pixels[selectedPixels[i]] = bgColor;
  
    // Turn on a random sparkle pixel.
    selectedPixels[i] = random16(NUM_PIXELS);
    pixels[selectedPixels[i]] = fgColor;
  }

  FastLED.show();
  delay(SPARKLE_DELAY);
}



void splitRotation(patternDef_t* patParams)
{
  static splitRotationParams_t* pp;
  
  uint16_t i;
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
    rgbColor.nscale8_video(BG_INTENSITY_SCALE_FACTOR);
    fill_solid(pixels, PANEL_NUM_PIXELS, rgbColor);

    // Set every nth pixel to the foreground color.
    hsv2rgb_rainbow(fgHsv, rgbColor);
    rgbColor.nscale8_video(BG_INTENSITY_SCALE_FACTOR);
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


void rotateCrgbRight(CRGB* a, uint16_t length)
{
  uint16_t i;
  CRGB temp;

  temp = a[length - 1];

  for (i = length - 1; i > 0; --i) {
    a[i] = a[i - 1];
  }

  a[0] = temp;
}


void rotateCrgbLeft(CRGB* a, uint16_t length)
{
  uint16_t i;
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
  
  uint16_t i;
  uint16_t j;
  
  for (i = 0; i < PANEL_NUM_PIXELS; ++i) {
    for (j = 1; j < NUM_PANELS; ++j) {
      pixels[i + PANEL_NUM_PIXELS * j] = pixels[i];
    }
  }
}


#ifdef ENABLE_DMX_AMBIENCE_LIGHTS
void replicateDMXPanels()
{
  if (NUM_DMX_PANELS <= 1)
    return;
  
  uint8_t i;
  uint8_t j;
  
  for (i = 0; i < DMX_PANEL_NUM_RGB_CHANNELS; ++i) {
    for (j = 1; j < NUM_DMX_PANELS; ++j) {
      dmxRgbArray[i + DMX_PANEL_NUM_RGB_CHANNELS * j] = dmxRgbArray[i];
    }
  }
}
#endif


uint16_t freeRam() 
{
  // Based on code retrieved on 1 April 2015 from
  // https://learn.adafruit.com/memories-of-an-arduino/measuring-free-memory

  extern int __heap_start, *__brkval; 
  int v; 
  return (uint16_t) &v - (__brkval == 0 ? (uint16_t) &__heap_start : (uint16_t) __brkval); 
}



/***********************************
 * DMX Ambiance Lighting Functions *
 ***********************************/

#ifdef ENABLE_DMX_AMBIENCE_LIGHTS


void dmxAmbianceRainbow(bool doInit)
{
  static uint8_t stepNum;
  static unsigned long nextStepMs = 0;

  CRGB rgbColor;
  CHSV hsvColor;
  uint8_t i;

  if (doInit) {
    stepNum = 0;
    return;
  }

  if (millis() < nextStepMs)
    return;

  // Rotate rainbow through all the channels in the panel.
  fill_rainbow(dmxRgbArray, DMX_PANEL_NUM_RGB_CHANNELS, stepNum, 255 / DMX_PANEL_NUM_RGB_CHANNELS);

//  // Step all channels in the panel together through the rainbow (Dr. Naked's suggestion).
//  hsvColor.h = stepNum;
//  hsvColor.s = 255;
//  hsvColor.v = 255;
//  hsv2rgb_rainbow(hsvColor, rgbColor);
//  fill_solid(dmxRgbArray, DMX_PANEL_NUM_RGB_CHANNELS, rgbColor);

  for (i = 0; i < DMX_PANEL_NUM_RGB_CHANNELS; dmxRgbArray[i++].nscale8_video(DMX_AMBIANCE_INTENSITY_SCALE_FACTOR));
  replicateDMXPanels();
  
  // TODO:  formalize this
  dmxRgbArray[NUM_DMX_RGB_CHANNELS - 1] = CRGB::Blue;  // controller box internal LED strip
  
  // We don't call FastLED.show() here.  The DMX LEDs will get updated
  // when the pixels get updated, which will be soon enough.

  ++stepNum;

  nextStepMs = millis() + 100;
}


#endif  // #ifdef ENABLE_DMX_AMBIENCE_LIGHTS



/***********************
 * Setup and Main Loop *
 ***********************/


void setup()
{
  FastLED.addLeds<WS2812B, PIXEL_DATA_PIN, GRB>(pixelArray, NUM_PHYSICAL_PIXELS);  // white pixel strips
  //FastLED.addLeds<WS2812B, PIXEL_DATA_PIN, RGB>(pixelArray, NUM_PHYSICAL_PIXELS);  // 5 V pixel strings

#ifdef ENABLE_DMX_AMBIENCE_LIGHTS
  FastLED.addLeds<DMXSIMPLE, DMX_DATA_PIN, RGB>(dmxRgbArray, NUM_DMX_RGB_CHANNELS);

  // Turn off all the DMX LEDs.
  fill_solid(dmxRgbArray, NUM_DMX_RGB_CHANNELS, CRGB::Black);
#endif

  // Initialize the pixel array so that any skipped pixels will remain off.
  fill_solid(pixelArray, NUM_PIXELS, CRGB::Black);

  // Analog input 0 should be disconnected, making it a good source of
  // random noise with which we can seed the random number generator.
  randomSeed(analogRead(0));  

  //Serial.begin(9600);
  //Serial.println("Starting");

  // Using the pixels, display the relative amount of free RAM.
  uint32_t numFreeBytes = freeRam();
  //Serial.println(numFreeBytes);
  uint32_t memDisplayNumPixelsOn = numFreeBytes * NUM_PIXELS / 2048;
  fill_solid(pixels, memDisplayNumPixelsOn, CRGB::Cyan);
  for (uint16_t i = 0; i < memDisplayNumPixelsOn; pixels[i++].nscale8_video(BG_INTENSITY_SCALE_FACTOR));
  FastLED.show();
  delay(2000);
  fill_solid(pixels, NUM_PIXELS, CRGB::Black);
  FastLED.show();
  
#ifdef ENABLE_DMX_AMBIENCE_LIGHTS
  dmxAmbianceRainbow(true);
#endif
}


void loop()
{
  static uint8_t patternNum = -1;
  static unsigned long nextPatternChangeMs = 0;
  static void (*loopFun)(patternDef_t*);
  static byte patParamBuf[PAT_PARAMS_BUF_SIZE];  // used by pattern fn; must remain intact between calls to same fn

  const patternDef_t* patDef;
  uint8_t patternType;
  unsigned long durationSec;
  patternDef_t* patParams;
  uint8_t paramStructSize;

#ifdef ENABLE_DMX_AMBIENCE_LIGHTS
  dmxAmbianceRainbow(false);
#endif

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

