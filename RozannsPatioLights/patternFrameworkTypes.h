/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Pattern Configuration Constants and Structures                  *
 *                                                                 *
 * by Ross Butler   Nov. 2014 - Nov. 2015                          *
 *                                                                 *
 *******************************************************************/

#ifndef __PATTERN_FRAMEWORK_TYPES_H
#define __PATTERN_FRAMEWORK_TYPES_H


#include "FastLED.h"


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
  uint16_t       delay;           // roughly, the number of milliseconds between each stepped movement of a wave
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
typedef struct xmasLightsParams xmasLightsParams_t;
struct xmasLightsParams {
  uint8_t numHues;
  HSVHue  hues[8];
};


// This structure holds the configuration of a solid color pattern.
typedef struct solidColorParams solidColorParams_t;
struct solidColorParams {
  HSVHue  startHue;       // starting hue
  HSVHue  endHue;         // end hue
  uint8_t saturation;     // the S in HSV
  uint8_t delay;          // ms delay between rotation steps
};


// This structure holds the configuration of a shine pattern.
typedef struct shineParams shineParams_t;
struct shineParams {
  HSVHue  bgHue;          // background hue
  uint16_t shineInterval; // interval between shines (ms)
  uint8_t delay;          // ms delay between shine steps
};


// This structure holds the configuration of a rainbow pattern.
typedef struct rainbowParams rainbowParams_t;
struct rainbowParams {
  bool    directionDown;  // true to rotate one way, false to rotate the other way
  uint16_t delay;         // approx. ms delay between rotation steps
};


// This structure holds the configuration of the Red White and Blue (RWaB) pattern.
typedef struct rwabParams rwabParams_t;
struct rwabParams {
  uint16_t delay;                 // approx. ms delay between rotation steps
  uint8_t  numPixelsPerSegment;   // number of pixels per color segment
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
  void    (*loopFun)(void*);
};


// The pattern functions are declared here so that we
// can use them to build the pattern metadata table.
void rainbow(void* patParams);
void multiWave(void* patParams);
void movingDot(void* patParams);
void sparkle(void* patParams);
void splitRotation(void* patParams);
void xmasLights(void* patParams);
void solidColor(void* patParams);
void shine(void* patParams);
void redWhiteAndBlue(void* patParams);


// These are the pattern types.  Except for END_OF_AUTO_PATTERNS,
// the values are indexes into the pattern metadata table.
#define MOVING_DOT             0
#define MULTI_WAVE             1
#define RAINBOW                2
#define SPARKLE                3
#define SPLIT_ROTATION         4
#define XMAS_LIGHTS            5
#define SOLID_COLOR            6
#define SHINE                  7
#define RED_WHITE_AND_BLUE     8
#define END_OF_AUTO_PATTERNS 255


// Each pattern metadata entry must be in the element
// position corresponding to its pattern type constant.
patternMetadata_t patternMetadataTable[] = {
  {sizeof(movingDotParams_t)    , movingDot},
  {sizeof(multiWaveParams_t)    , multiWave},
  {sizeof(rainbowParams_t)      , rainbow},
  {sizeof(sparkleParams_t)      , sparkle},
  {sizeof(splitRotationParams_t), splitRotation},
  {sizeof(xmasLightsParams_t)   , xmasLights},
  {sizeof(solidColorParams_t)   , solidColor},
  {sizeof(shineParams_t)        , shine},
  {sizeof(rwabParams_t)         , redWhiteAndBlue},
};


// the size of the buffer used when copying pattern parameters from flash to RAM
#define PAT_PARAMS_BUF_SIZE 32


#endif  // #ifndef __PATTERN_FRAMEWORK_TYPES_H
