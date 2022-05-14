/*****************************************************************
 *                                                               *
 * Addressable LED Pixel Driver for Permanence                   *
 *                                                               *
 * Platform:  Arduino Uno, Pro, Pro Mini, Nano                   *
 *                                                               *
 * Architecture and pattern functions by Ross Butler, June 2015  *
 *                                                               *
 *****************************************************************/

#include <avr/pgmspace.h>

#include "FastLED.h"



/***************************
 * Configuration Constants *
 ***************************/

#define RGB_RED_PIN 3
#define RGB_GREEN_PIN 5
#define RGB_BLUE_PIN 9
//#define RGB_LOW_SIDE_SWITCHING

// the total number of pixels in the strip
#define NUM_PHYSICAL_PIXELS 150

// the number of pixels at the start of the strip that should remain unused (off)
#define NUM_SKIP_PIXELS 0

// the number of pixels at the end of the strip that shouldn't be part of patterns that depend on symmetry
#define NUM_NONSYMMETRICAL_PIXELS 0

// the number of symmetrical pixels
#define NUM_SYMMETRICAL_PIXELS (NUM_PHYSICAL_PIXELS - NUM_SKIP_PIXELS - NUM_NONSYMMETRICAL_PIXELS)

// the number of pixels attached to each panel
// (must be a factor of the number of physical pixels)
#define NUM_PANELS 1
#define PANEL_NUM_PIXELS (NUM_SYMMETRICAL_PIXELS / NUM_PANELS)

#define PIXEL_DATA_PIN 10
#define MODE_PUSHBUTTON_PIN 12
#define ONBOARD_LED_PIN 13

// BG_INTENSITY_SCALE_FACTOR sets the maximum brightness for backgrounds.  It also
// sets the brightness of the foreground of patterns that aren't doing some sort
// of flashing or sparkle effect.  Although the range is 0 to 255, practical
// values are more like 16 to 128.  48 is pretty darn bright in a darkened room.
#define BG_INTENSITY_SCALE_FACTOR 48

// MAX_WAVEFORMS is the maximum number of waveforms
// that can be configured for a multiWave pattern.
#define MAX_WAVEFORMS 3

// MAX_SPARKLE_DENSITY is the maximum number of foreground pixels
// that can be illuminated simultaneously in a sparkle pattern.
#define MAX_SPARKLE_DENSITY 5

// Approximate millisecond delay between sparkle changes.
#define SPARKLE_DELAY 15

#define PUSHBUTTON_DEBOUNCE_INTERVAL_MS 100


/***************************
 * Heartbeat Configuration *
 ***************************/

constexpr uint32_t lubRampUpStepDelayMs = 12;
constexpr uint32_t lubRampDownStepDelayMs = 12;
constexpr uint8_t lubMaxIntensity = 160;
constexpr uint32_t interLubDubDelayMs = 80;
constexpr uint8_t interLubDubIntensity = 24;
constexpr uint32_t dubRampUpStepDelayMs = 9;
constexpr uint32_t dubRampDownStepDelayMs = 9;
constexpr uint8_t dubMaxIntensity = 255;
constexpr uint32_t interBeatDelayMs = 1000;
constexpr uint8_t interBeatIntensity = 15;
constexpr uint8_t intensityStepSize = 12;
static const CRGB heartbeatColor = CRGB::Red;

enum class HeartbeatState {
  initialize,
  lubRampUp,
  lubRampDown,
  betweenLubAndDub,
  dubRampUp,
  dubRampDown,
  betweenBeats
};


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


// This structure holds the configuration of a solid color pattern.
typedef struct solidColorParams solidColorParams_t;
struct solidColorParams {
  HSVHue  startHue;       // starting hue
  HSVHue  endHue;         // end hue
  uint8_t delay;          // ms delay between rotation steps
};


// This structure holds the configuration of a glint pattern.
typedef struct glintParams glintParams_t;
struct glintParams {
  HSVHue  bgHue;          // background hue
  uint8_t width;          // width (in pixels) of the glint
  float stepIncrement;    // pixels or fractions of a pixel to move the glint at each step
  uint16_t glintInterval; // interval between glints (ms)
  uint8_t delay;          // ms delay between glint steps
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
void solidColor(patternDef_t* patParams);
void glint(patternDef_t* patParams);


// These are the pattern types.  The values are indexes into the pattern metadata table.
#define MOVING_DOT     0
#define MULTI_WAVE     1
#define RAINBOW        2
#define SPARKLE        3
#define SPLIT_ROTATION 4
#define SOLID_COLOR    5
#define GLINT          6


// Each pattern metadata entry must be in the element
// position corresponding to its pattern type constant.
patternMetadata_t patternMetadataTable[] = {
  {sizeof(movingDotParams_t)    , movingDot},
  {sizeof(multiWaveParams_t)    , multiWave},
  {sizeof(rainbowParams_t)      , rainbow},
  {sizeof(sparkleParams_t)      , sparkle},
  {sizeof(splitRotationParams_t), splitRotation},
  {sizeof(solidColorParams_t)   , solidColor},
  {sizeof(glintParams_t)        , glint},
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

static CRGB pixelArray[NUM_PHYSICAL_PIXELS];
static CRGB* pixels = pixelArray + NUM_SKIP_PIXELS;
static unsigned long nextPatternUpdateMs = 0;



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

const multiWaveParams_t multiWave1Random PROGMEM = {
  true,
  { {true , HUE_RED      , HUE_RED       ,  1, SINE_WAVE, false,  100},
    {false, HUE_RED      , HUE_RED       ,  0, QUADRATIC_EASING, false,   0},
    {false, HUE_RED      , HUE_RED       ,  0, TRIANGLE_WAVE, false,   0} } };

const multiWaveParams_t multiWave3ClusterFuck PROGMEM = {
  false,
  { {false, HUE_BLUE     , HUE_BLUE      ,  1, QUADRATIC_EASING, true ,  60},
    {false, HUE_RED      , HUE_RED       ,  3, TRIANGLE_WAVE   , true ,  40},
    {false, HUE_GREEN    , HUE_GREEN     ,  5, QUADRATIC_EASING, true ,  20} } };

const multiWaveParams_t multiWave3 PROGMEM = { 
  false,
  { {false, HUE_BLUE     , HUE_ORANGE    , -1, SINE_WAVE       , false,  30},
    {false, HUE_RED      , HUE_AQUA      , -2, SINE_WAVE       , true ,  40},
    {false, HUE_GREEN    , HUE_PINK      , -3, SINE_WAVE       , false,  50} } };


// ---------- sparkle patterns ----------

// Elements:
//     fgColorCode  color code of the sparkle color
//     bgColorCode  color code of the background color
//     density      number of foreground (sparkle) pixels on simultaneously (1 to MAX_SPARKLE_DENSITY)

const sparkleParams_t sparkleBluePretty   PROGMEM = {CRGB::White   ,  CRGB::Blue    , 1};
const sparkleParams_t sparkleGreenPretty  PROGMEM = {CRGB::White   ,  CRGB::Green   , 1};
const sparkleParams_t sparkleRandom       PROGMEM = {CRGB::Black   ,  CRGB::Black   , 1};
const sparkleParams_t sparkleRed          PROGMEM = {CRGB::Red     ,  CRGB::Black   , 1};
const sparkleParams_t sparkleBlue         PROGMEM = {CRGB::Blue    ,  CRGB::Black   , 1};


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
//     holdTimeFarMs  number of ms to hold the dot at the far end before it moves back toward pixel 0 (0-65535)
//     holdTimeNearMs number of ms to hold the dot at the near end before it moves toward the last pixel (0-65535)

//                                                       fg           bg           randFg randBg chngFg zipper biDir   d0  d1  hFar hNear
const movingDotParams_t movingDotRandomZipper PROGMEM = {CRGB::Black, CRGB::Black,  true,  true,  true,  true, true , 500,  60, 3000, 1500};
//const movingDotParams_t movingDotRandomPong   PROGMEM = {CRGB::Black, CRGB::Black,  true, false, false, false, true , 100, 100,    0,    0};
//const movingDotParams_t movingDotRandomPaint  PROGMEM = {CRGB::Black, CRGB::Black,  true,  true,  true,  true, false, 200,   0,    0,    0};


// ---------- split-rotation patterns ----------

// Elements:
//     randomHue      true if the hues should be selected at random
//     fgHue          foreground (moving pixels) hue
//     bgHue          background hue
//     fgInterval     number of background pixels minus one between each moving pixel
//     directionDown  true to rotate one way, false to rotate the other way
//     delay          approx. ms delay between rotation steps (0-255)

//const splitRotationParams_t splitRotationRed8RFast      PROGMEM = {false, HUE_RED           , HUE_AQUA          , 8, false,  30};
//const splitRotationParams_t splitRotationRandom4RSlow   PROGMEM = {true , HUE_RED           , HUE_RED           , 4, false, 120};
//const splitRotationParams_t splitRotationRandom4LSlow   PROGMEM = {true , HUE_RED           , HUE_RED           , 4, true , 120};
//const splitRotationParams_t splitRotationRandom8RSlow   PROGMEM = {true , HUE_RED           , HUE_RED           , 8, false, 120};
//const splitRotationParams_t splitRotationRandom8LSlow   PROGMEM = {true , HUE_RED           , HUE_RED           , 8, true , 120};
//const splitRotationParams_t splitRotationRandom8RMedium PROGMEM = {true , HUE_RED           , HUE_RED           , 8, false,  60};
//const splitRotationParams_t splitRotationRandom8LMedium PROGMEM = {true , HUE_RED           , HUE_RED           , 8, true ,  60};
//const splitRotationParams_t splitRotationRandom8RFast  PROGMEM = {true , HUE_RED           , HUE_RED           , 8, false,  30};
//const splitRotationParams_t splitRotationRandom8LFast  PROGMEM = {true , HUE_RED           , HUE_RED           , 8, true ,  30};
//const splitRotationParams_t splitRotationRandom8RDizzy PROGMEM = {true , HUE_RED           , HUE_RED           , 8, false,  15};
//const splitRotationParams_t splitRotationRandom8LDizzy PROGMEM = {true , HUE_RED           , HUE_RED           , 8, true ,  15};


// ---------- rainbow patterns ----------

// Elements:
//     directionDown  true to rotate one way, false to rotate the other way
//     delay          approx. ms delay between rotation steps (0-255)

const rainbowParams_t rainbowSlow7Sec   PROGMEM = {false,  30};
const rainbowParams_t rainbowSlow30Sec  PROGMEM = {false, 120};


// ---------- solid color patterns ----------

// Elements:
//     startHue  starting hue
//     endHue    end hue
//     delay     approx. ms delay between rotation steps (0-255)

const solidColorParams_t solidBlues7Sec  PROGMEM = {HUE_AQUA, (HSVHue) 175, 149};


// ---------- glint patterns ----------

// Elements:
//     bgHue          starting hue
//     width;         width (in pixels) of the glint
//     stepIncrement  pixels or fractions of a pixel to move the glint at each step
//     glintInterval  interval between glints (ms, 0-65535)
//     delay          approx. ms delay between glint steps (0-255)

//    HUE_RED = 0,
//    HUE_ORANGE = 32,
//    HUE_YELLOW = 64,
//    HUE_GREEN = 96,
//    HUE_AQUA = 128,
//    HUE_BLUE = 160,
//    HUE_PURPLE = 192,
//    HUE_PINK = 224

const glintParams_t blueGlint  PROGMEM = {HUE_PURPLE, 20, 0.15, 1, 25};



/***********************
 *   T H E   S H O W   *
 ***********************/

// Elements in each pattern element:
//     pattern function
//     duration (number of seconds)
//     pattern configuration (prefix with ampersand)

const patternDef_t patternsDefs[] PROGMEM = {

// add the glint pattern

  {GLINT        , 1440 * 60,  &blueGlint},

//  {RAINBOW      ,  30,  &rainbowSlow7Sec},
//
//  {SOLID_COLOR  ,  60,  &solidBlues7Sec},
//
//  {MOVING_DOT   ,  30,  &movingDotRandomZipper},
//
//  {RAINBOW      ,  60,  &rainbowSlow30Sec},

//--------------------------------------------------
//  {SPARKLE      ,   6,  &sparkleBluePretty},
//  
//  {MOVING_DOT    ,  10,  &movingDotRandomPaint},
//  {MOVING_DOT    ,  10,  &movingDotRandomPong},
//
//  {MULTI_WAVE    ,  10,  &multiWave1Random},
//
//  {SPARKLE      ,   6,  &sparkleGreenPretty},
//
//  {SPARKLE      ,   6,  &sparkleRed},
//
//  {SPARKLE      ,   6,  &sparkleBlue},
//
//  {SPARKLE      ,   1,  &sparkleRandom},
//  {SPARKLE      ,   1,  &sparkleRandom},
//  {SPARKLE      ,   1,  &sparkleRandom},
//  {SPARKLE      ,   1,  &sparkleRandom},
//  {SPARKLE      ,   1,  &sparkleRandom},
//  {SPARKLE      ,   1,  &sparkleRandom},
//  {SPARKLE      ,   1,  &sparkleRandom},
//  {SPARKLE      ,   1,  &sparkleRandom},
//  {SPARKLE      ,   1,  &sparkleRandom},
//  {SPARKLE      ,   1,  &sparkleRandom},
//  {SPARKLE      ,   1,  &sparkleRandom},
//  {SPARKLE      ,   1,  &sparkleRandom},
//  {SPARKLE      ,   1,  &sparkleRandom},
//  {SPARKLE      ,   1,  &sparkleRandom},
//  {SPARKLE      ,   1,  &sparkleRandom},
//  {SPARKLE      ,   1,  &sparkleRandom},
//  {SPARKLE      ,   1,  &sparkleRandom},
//  {SPARKLE      ,   1,  &sparkleRandom},
//  {SPARKLE      ,   1,  &sparkleRandom},
//  {SPARKLE      ,   1,  &sparkleRandom},

//  {SPLIT_ROTATION,  5,  &splitRotationRandom8RFast}, 
//  {MULTI_WAVE    ,  30,  &multiWave3ClusterFuck},
//  {MULTI_WAVE    ,  30,  &multiWave3 },

};


// Calculate the number of patterns that make up the entire show.
#define NUM_PATTERNS (sizeof(patternsDefs) / sizeof(patternDef_t))



/*****************************
 * Pattern Drawing Functions *
 *****************************/

void glint(patternDef_t* patParams)
{
  static glintParams_t* pp;
  static unsigned long nextGlintMs;
  static bool doingGlint = false;
//  static uint8_t glintStepNum;

  // all these values are multiplied by 100 to get sub-pixel resolution without using floats
  static int n0;      // current position (pixel no.) of the start of the glint
  static int s;       // how far to move the glint at each step
  static int w;       // the width of the glint
  static int ns;      // the y (saturation decrease) interval between each pixel in the glint (1/255ths)

  unsigned long now = millis();

  if (patParams) {
    pp = (glintParams_t*) patParams;
    nextGlintMs = now + pp->glintInterval;
    s = pp->stepIncrement * 100;
    w = pp->width * 100;
    ns = 25500 / w;
    return;
  }


  nextPatternUpdateMs = millis() + pp->delay;

  if (!doingGlint && now >= nextGlintMs) {
    n0 = - w;
    doingGlint = true;
//    Serial.println("starting glint");
//    Serial.print("s=");
//    Serial.print(s);
//    Serial.print("  w=");
//    Serial.print(w);
//    Serial.print("  ns=");
//    Serial.print(ns);
//    Serial.print("  n0=");
//    Serial.println(n0);
  }

  CHSV hsvColor;
  CRGB rgbColor;
  hsvColor.h = pp->bgHue;
  hsvColor.v = BG_INTENSITY_SCALE_FACTOR;
  for (uint8_t i = 0; i < NUM_SYMMETRICAL_PIXELS; ++i) {

    int i100 = i * 100;
    if (doingGlint) {
      if (i100 < n0 || i100 > n0 + w) {
        hsvColor.s = 255;
      }
      else {
        int n = (i100 - n0) * ns;
        float y = quadwave8(n / 100);
        hsvColor.s = 255 - y;
//        Serial.print("i=");
//        Serial.print(i);
//        Serial.print("  n0=");
//        Serial.print(n0);
//        Serial.print("  n=");
//        Serial.print(n);
//        Serial.print("  y=");
//        Serial.print(y);
//        Serial.print("  hsvColor.s=");
//        Serial.print(hsvColor.s);
//        Serial.println("----------");
      }
    }
    else {
      hsvColor.s = 255;
    }

    hsv2rgb_rainbow(hsvColor, rgbColor);
    pixels[i] = rgbColor;  
  }

  // Make the non-symmetrical pixels the background color.
  hsvColor.s = 255;
  hsv2rgb_rainbow(hsvColor, rgbColor);
  for (uint8_t i = NUM_SYMMETRICAL_PIXELS; i < NUM_SYMMETRICAL_PIXELS + NUM_NONSYMMETRICAL_PIXELS; pixels[i++] = rgbColor);

  FastLED.show();

  if (doingGlint) {
    n0 += s;
    if (n0 >= NUM_SYMMETRICAL_PIXELS * 100) {
      doingGlint = false;
      nextGlintMs = now + pp->glintInterval;
//      Serial.println("glint done");
    }
  }
}



void solidColor(patternDef_t* patParams)
{
  static solidColorParams_t* pp;
  static uint8_t stepNum;
  static uint8_t stepDir;  // non-zero for start->end, zero for end->start

  if (patParams) {
    pp = (solidColorParams_t*) patParams;
    stepNum = pp->startHue;
    stepDir = 1;
    return;
  }


  nextPatternUpdateMs = millis() + pp->delay;

  CHSV hsvColor;
  hsvColor.h = stepNum;
  hsvColor.s = 255;
  hsvColor.v = BG_INTENSITY_SCALE_FACTOR;
  CRGB rgbColor;
  hsv2rgb_rainbow(hsvColor, rgbColor);
  fill_solid(pixels, NUM_SYMMETRICAL_PIXELS + NUM_NONSYMMETRICAL_PIXELS, rgbColor);
  FastLED.show();

  if (stepDir) {
    if (++stepNum == pp->endHue) {
      stepDir = 0;
    }
  }
  else {
    if (--stepNum == pp->startHue) {
      stepDir = 1;
    }
  }
}



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

  nextPatternUpdateMs = millis() + pp->delay;

  fill_rainbow(pixels, PANEL_NUM_PIXELS, stepNum, 255 / PANEL_NUM_PIXELS);
  replicatePixelPanels();

  for (i = 0; i < NUM_SYMMETRICAL_PIXELS; pixels[i++].nscale8_video(BG_INTENSITY_SCALE_FACTOR));

  FastLED.show();

  if (pp->directionDown) {
    ++stepNum;
  }
  else {
    --stepNum;
  }
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
  static uint8_t lastDelay;

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
      angleInterval[w] = 65535 / (pp->boundedByPanel ? PANEL_NUM_PIXELS : NUM_SYMMETRICAL_PIXELS) * abs(waveParam->numWaves);

      i0[w] = 0;
      
      delayCount[w] = waveParam->delay > 0 ? waveParam->delay : 1;
    }
    
    lastDelay = 1;

    return;
  }


  unsigned long now = millis();

  bool needToDisplay = false;
  uint8_t lowestDelayCount = 255;
  for (w = 0; w < numWaveforms; ++w) {
    delayCount[w] -= lastDelay;
    if (0 == delayCount[w]) {
      delayCount[w] = pp->waveParams[w].delay > 0 ? pp->waveParams[w].delay : 1;
      needToDisplay = true;
      if (pp->waveParams[w].directionDown) {
        if (++i0[w] >= (pp->boundedByPanel ? PANEL_NUM_PIXELS : NUM_SYMMETRICAL_PIXELS)) {
          i0[w] = 0;
        }
      }
      else {
        if (0 == i0[w]--) {
          i0[w] = pp->boundedByPanel ? PANEL_NUM_PIXELS - 1 : NUM_SYMMETRICAL_PIXELS - 1;
        }
      }
    }
    // The lowest delay count is the number of ms after which we need to display again.
    if (delayCount[w] < lowestDelayCount) {
      lowestDelayCount = delayCount[w];
    }
  }
  nextPatternUpdateMs = now + lowestDelayCount;
  lastDelay = lowestDelayCount;

  if (!needToDisplay) {
    return;
  }

    
  for (i = 0; i < (pp->boundedByPanel ? PANEL_NUM_PIXELS : NUM_SYMMETRICAL_PIXELS); ++i) {

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
  static uint8_t stepNum;
  static uint8_t stepDir;    // 0 = away from pixel 0, 1 = toward pixel 0, 2 = hold at far end, 3 = hold at near end
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
    fgColor.nscale8_video(BG_INTENSITY_SCALE_FACTOR);
    bgColor.nscale8_video(BG_INTENSITY_SCALE_FACTOR);

    if (!pp->zipperBg) {
      fill_solid(pixelArray, NUM_SYMMETRICAL_PIXELS, bgColor);
    }

    return;
  }


  unsigned long now = millis();

  prevStepNum = stepNum;
  prevStepDir = stepDir;
  
  switch (stepDir) {

    // moving away from pixel 0
    case 0:
      if (++stepNum >= PANEL_NUM_PIXELS) {
        stepDir = 2;
        stepNum = PANEL_NUM_PIXELS - 1;
        nextPatternUpdateMs = now + pp->holdTimeFarMs;
      }
      else {
        nextPatternUpdateMs = now + pp->delay0;
      }
      break;

    // moving toward pixel 0
    case 1:
      if (--stepNum == 0) {
        stepDir = 3;
      }
      nextPatternUpdateMs = now + pp->delay1;
      break;

    // held at far end
    case 2:
      if (pp->bidirectional) {
        stepDir = 1;
        stepNum = PANEL_NUM_PIXELS - 2;
      }
      else {
        stepDir = 0;
        stepNum = 0;
      }
      break;

    // held at near end
    case 3:
      stepDir = 0;
      nextPatternUpdateMs = now + pp->holdTimeNearMs;
      break;
  }

  if (0 == stepDir && 0 != prevStepDir && pp->changeFgColor) {
    if (pp->zipperBg) {
      // When doing the zipper effect, change the background color, too.
      selectRandomRgb(&fgColor, &bgColor);
      fgColor.nscale8_video(BG_INTENSITY_SCALE_FACTOR);
      bgColor.nscale8_video(BG_INTENSITY_SCALE_FACTOR);
    }
    else {
      selectRandomRgb(&fgColor, NULL);
      fgColor.nscale8_video(BG_INTENSITY_SCALE_FACTOR);
    }
  }

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
    bgColor.nscale8_video(BG_INTENSITY_SCALE_FACTOR);
    
    // Fill with background color.
    fill_solid(pixels, NUM_SYMMETRICAL_PIXELS + NUM_NONSYMMETRICAL_PIXELS, bgColor);
    FastLED.show();
    return;
  }

  nextPatternUpdateMs = millis() + SPARKLE_DELAY;

  for (i = 0; i < density; ++i) {
    // Turn off the last sparkle pixel.
    pixels[selectedPixels[i]] = bgColor;
  
    // Turn on a random sparkle pixel.
    selectedPixels[i] = random8(NUM_SYMMETRICAL_PIXELS + NUM_NONSYMMETRICAL_PIXELS);
    pixels[selectedPixels[i]] = fgColor;
  }

  FastLED.show();
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


  nextPatternUpdateMs = millis() + pp->delay;

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



/*********************************
 * Functions called from loop()  *
 *********************************/

void doPatterns()
{
  static uint8_t patternSelection = 255;
  static uint8_t patternNum = 255;
  static unsigned long nextPatternChangeMs = 0;
  static void (*loopFun)(patternDef_t*);
  static byte patParamBuf[PAT_PARAMS_BUF_SIZE];  // used by pattern fn; must remain intact between calls to same fn
  static bool loopedWithoutPatternUpdate;
  static unsigned long lastPushbuttonEventMs = 0;
  static bool lastPushbuttonState = HIGH;
  static bool pushbuttonDebouncedState = HIGH;

  const patternDef_t* patDef;
  uint8_t patternType;
  unsigned long durationSec;
  patternDef_t* patParams;
  uint8_t paramStructSize;
  unsigned long now = millis();

//  digitalWrite(ONBOARD_LED_PIN, pushbuttonDebouncedState);
  bool pushbuttonState = digitalRead(MODE_PUSHBUTTON_PIN);
  if (pushbuttonState != lastPushbuttonState) {
    lastPushbuttonState = pushbuttonState;
    lastPushbuttonEventMs = now;
  }
  if (pushbuttonState != pushbuttonDebouncedState
        && now - lastPushbuttonEventMs > PUSHBUTTON_DEBOUNCE_INTERVAL_MS) {
      pushbuttonDebouncedState = pushbuttonState;

      if (pushbuttonDebouncedState == LOW) {
        if (++patternSelection >= NUM_PATTERNS) {
          patternSelection = 255;
          // Flash the entire string white so that the buttonpusher knows we're back on the show.
          CRGB rgbColor = CRGB::White;
          rgbColor.nscale8_video(BG_INTENSITY_SCALE_FACTOR);
          fill_solid(pixels, NUM_SYMMETRICAL_PIXELS, rgbColor);
          FastLED.show();
          delay(150);
        }
        nextPatternChangeMs = now;
      }
  }
  

  if (now < nextPatternChangeMs) {
    
    if (now >= nextPatternUpdateMs && loopFun != 0) {

      // If loop has been called at least once without having to call loopFun
      // then we are probably keeping up with the update frequency that the
      // pattern wants.  However, if we are calling loopFun every time loop
      // is called, we probably can't satisfy the pattern's timing.  In that
      // case, turn on the onboard LED to indicate that there is a problem.
      digitalWrite(ONBOARD_LED_PIN, !loopedWithoutPatternUpdate);
      loopedWithoutPatternUpdate = false;
      
      (*loopFun)(NULL);
    }
    else {
      loopedWithoutPatternUpdate = true;
    }
  }
  
  else {

    if (patternSelection == 255) {
      if (++patternNum >= NUM_PATTERNS) {
        patternNum = 0;
      }
    }
    else {
      patternNum = patternSelection;
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
      CRGB rgbColor = CRGB::Red;
      rgbColor.nscale8_video(BG_INTENSITY_SCALE_FACTOR);
      fill_solid(pixels, NUM_SYMMETRICAL_PIXELS, rgbColor);
      FastLED.show();
      loopFun = 0;
      nextPatternChangeMs = millis() + 1000;
      return;
    }

    // Get the pattern parameters from flash.    
    memcpy_P(patParamBuf, patParams, paramStructSize);

    // When a pattern is explicitly selected, it should run forever.
    nextPatternChangeMs = (patternSelection != 255) ? -1 : millis() + durationSec * 1000;
    
    // Turn off all the pixels, including skipped and non-pattern pixels.
    fill_solid(pixelArray, NUM_PHYSICAL_PIXELS, CRGB::Black);
    FastLED.show();

    // Let the pattern initialize itself.
    (*loopFun)((patternDef_t*) patParamBuf);
    
    // Give the pattern a chance to draw itself right away.
    nextPatternUpdateMs = millis() - 1;
    
    // Blip the can't-keep-up light.
    loopedWithoutPatternUpdate = false;
  }

}


void doHeartbeat()
{
  static HeartbeatState state = HeartbeatState::initialize;
  static int16_t currentIntensity;
  static unsigned long lastStepMs;
  static unsigned long stepDelayMs;

  unsigned long now = millis();

  if (now - lastStepMs >= stepDelayMs) {
    lastStepMs = now;

    switch (state) {
      case HeartbeatState::initialize:
        currentIntensity = interBeatIntensity;
        stepDelayMs = lubRampUpStepDelayMs;
        state = HeartbeatState::lubRampUp;
        break;

      case HeartbeatState::lubRampUp:
        currentIntensity += intensityStepSize;
        if (currentIntensity >= lubMaxIntensity) {
          currentIntensity = lubMaxIntensity;
          stepDelayMs = lubRampDownStepDelayMs;
          state = HeartbeatState::lubRampDown;
        }
        break;

      case HeartbeatState::lubRampDown:
        currentIntensity -= intensityStepSize;
        if (currentIntensity <= interLubDubIntensity) {
          currentIntensity = interLubDubIntensity;
          stepDelayMs = interLubDubDelayMs;
          state = HeartbeatState::betweenLubAndDub;
        }
        break;

      case HeartbeatState::betweenLubAndDub:
        stepDelayMs = dubRampUpStepDelayMs;
        state = HeartbeatState::dubRampUp;
        break;

      case HeartbeatState::dubRampUp:
        currentIntensity += intensityStepSize;
        if (currentIntensity >= dubMaxIntensity) {
          currentIntensity = dubMaxIntensity;
          stepDelayMs = dubRampDownStepDelayMs;
          state = HeartbeatState::dubRampDown;
        }
        break;

      case HeartbeatState::dubRampDown:
        currentIntensity -= intensityStepSize;
        if (currentIntensity <= interBeatIntensity) {
          currentIntensity = interBeatIntensity;
          stepDelayMs = interBeatDelayMs;
          state = HeartbeatState::betweenBeats;
        }
        break;

      case HeartbeatState::betweenBeats:
        stepDelayMs = lubRampUpStepDelayMs;
        state = HeartbeatState::lubRampUp;
        break;
    }

    CRGB rgb = heartbeatColor;
    rgb.nscale8_video(currentIntensity);
#ifdef RGB_LOW_SIDE_SWITCHING
    analogWrite(RGB_RED_PIN, 255 - rgb.red);
    analogWrite(RGB_GREEN_PIN, 255 - rgb.green);
    analogWrite(RGB_BLUE_PIN, 255 - rgb.blue);
#else
    analogWrite(RGB_RED_PIN, rgb.red);
    analogWrite(RGB_GREEN_PIN, rgb.green);
    analogWrite(RGB_BLUE_PIN, rgb.blue);
#endif
  }

}



/***********************
 * Setup and Main Loop *
 ***********************/


void setup()
{
  // Let everything settle down after a rough power-up.
  delay(1000);
  
  pinMode(MODE_PUSHBUTTON_PIN, INPUT_PULLUP);
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);

  // make sure the RGB strip is off (PWM duty cycle 0% so that all pins are pulled high)
  analogWrite(RGB_RED_PIN, 0);  
  analogWrite(RGB_GREEN_PIN, 0);  
  analogWrite(RGB_BLUE_PIN, 0);  
  
  FastLED.addLeds<WS2812B, PIXEL_DATA_PIN, GRB>(pixelArray, NUM_PHYSICAL_PIXELS);  // white pixel strips

  // Initialize the pixel array so that any skipped pixels will remain off.
  fill_solid(pixelArray, NUM_PHYSICAL_PIXELS, CRGB::Black);

  // Analog input 0 should be disconnected, making it a good source of
  // random noise with which we can seed the random number generator.
  randomSeed(analogRead(0));  

//  Serial.begin(9600);
//  Serial.println("Starting");
}


void loop()
{
  doPatterns();
  
  // Here, we can do other stuff, like maybe a pattern that isn't part of the pattern framework.
  doHeartbeat();
}

