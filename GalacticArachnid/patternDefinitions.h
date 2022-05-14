/*******************************************************************
 *                                                                 *
 * Pixel Pattern Library Test                                      *
 *                                                                 *
 * Pattern Definitions                                             *
 *                                                                 *
 * by Ross Butler  Nov. 2014 - Nov. 2015                           * 
 *                                                                 *
 *******************************************************************/

#ifndef __PATTERN_DEFINITIONS_H
#define __PATTERN_DEFINITIONS_H

#include "Blocks.h"
#include "MovingDot.h"
#include "MultiWave.h"
#include "SplitRotation.h"
#include "Rainbow.h"
#include "Sparkle.h"
#include "FastLED.h"
#include "SolidColor.h"
//#include "stockPatternDefinitions.h"

using namespace pixelPattern;

#define BSU_BLUE           (CRGB::HTMLColorCode) 0x0000FF
#define BSU_ORANGE         (CRGB::HTMLColorCode) 0xD03000  // not tested--probably needs more red
#define HUE_BSU_BLUE       (HSVHue) 160
#define HUE_BSU_ORANGE     (HSVHue) 16  // this is a good orange for the pixel strawberry strings


// ---------- moving-dot patterns ----------

// Elements:
//     fgColorCode    dot color
//     bgColorCode    background color
//     randomFgColor  true if the foreground color should be selected at random
//     randomBgColor  true if the background color should be selected at random
//     changeFgColor  true if the foreground color should be re-selected at random for each movement cycle
//     zipperBg;      zipper effect:  set the bg color when moving away from pixel 0 then clear it when moving toward pixel 0
//     bidirectional  true if the dot should bounce back and forth
//     delay0Ms       delay between dot steps when moving away from pixel 0
//     delay1Ms       delay between dot steps when moving toward pixel 0
//     holdTimeFarMs  how long to hold the dot at the far end before it moves back toward pixel 0
//     holdTimeNearMs how long to hold the dot at the near end before it moves toward the last pixel

//                                                                     fg               bg           randFg randBg chngFg zipper  biDir  d0     d1   hFar  hNear
const MovingDot::PatternConfig allOff                       PROGMEM = {CRGB::Black    , CRGB::Black , false, false, false, false, false, 1000, 1000, 1000, 1000};

const MovingDot::PatternConfig movingDotPongInverted        PROGMEM = {CRGB::Black    , CRGB::Black , false,  true, false, false,  true,   1L,   1L,   0,    0};

const MovingDot::PatternConfig movingDotRandomZipAndHold    PROGMEM = {CRGB::Black    , CRGB::Black , true , true , true ,  true, true ,   30,   10, 6000, 1000};
const MovingDot::PatternConfig movingDotRandomPaint         PROGMEM = {CRGB::Black    , CRGB::Black , true , true , true ,  true, false,    5,   10,  500,    0};
const MovingDot::PatternConfig movingDotSlowPongLargeEyes   PROGMEM = {CRGB::Black    , CRGB::Black , true , false, false, false, true ,   40,   40,    0,    0};
const MovingDot::PatternConfig movingDotSlowPongSmallEyes   PROGMEM = {CRGB::Black    , CRGB::Black , true , false, false, false, true ,   60,   60,    0,    0};
const MovingDot::PatternConfig movingDotFastPongLargeEyes   PROGMEM = {CRGB::Black    , CRGB::Black , true , false, false, false, true ,   24,   24,    0,    0};
const MovingDot::PatternConfig movingDotFastPongSmallEyes   PROGMEM = {CRGB::Black    , CRGB::Black , true , false, false, false, true ,   32,   32,    0,    0};

const MovingDot::PatternConfig movingDotSlowRotateLargeEyes PROGMEM = {CRGB::Black    , CRGB::Black , true , false, true , false, false,   24,   24,    0,    0};
const MovingDot::PatternConfig movingDotSlowRotateSmallEyes PROGMEM = {CRGB::Black    , CRGB::Black , true , false, true , false, false,   32,   32,    0,    0};
const MovingDot::PatternConfig movingDotMedRotateLargeEyes  PROGMEM = {CRGB::Black    , CRGB::Black , true , false, true , false, false,   12,   12,    0,    0};
const MovingDot::PatternConfig movingDotMedRotateSmallEyes  PROGMEM = {CRGB::Black    , CRGB::Black , true , false, true , false, false,   16,   16,    0,    0};
const MovingDot::PatternConfig movingDotFastRotateLargeEyes PROGMEM = {CRGB::Black    , CRGB::Black , true , false, true , false, false,    1,    1,    0,    0};
const MovingDot::PatternConfig movingDotFastRotateSmallEyes PROGMEM = {CRGB::Black    , CRGB::Black , true , false, true , false, false,    2,    2,    0,    0};


// ---------- multi-wave patterns ----------

// Each multiwave element consists of one boundedByPanel element and MultiWave::maxWaveforms
// ColorWave elements.  When less than maxWaveforms are needed, set numWaves
// to zero in unused ColorWave definitions.

// ColorWave elements:
//     randomHue       true if the waveform hue should be selected at random
//     posHue          hue of the positive half of the waveform (if randomHue is false)
//     negHue          hue of the negative half of the waveform (if randomHue is false)
//     numWaves        no. of complete waveforms in pattern; neg. for halfwave (neg. half black); must be non-zero
//     waveformType    a WaveType enum value (ColorWave::square, ColorWave::triangle, etc.);
//                     ColorWave::none to indicate end of waveforms (this and subsequent waveforms are unused)
//     directionDown   true if the waves should move toward pixel 0, false if they should move the other direction
//     delayMs         the period between each stepped movement of a wave

const MultiWave::PatternConfig multiWaveOrangeUp PROGMEM = {
  true,     // bounded by panel
  // rand   posHue         negHue       waves  type                        down     dly
  { {false, HUE_BSU_ORANGE, HUE_RED       ,  -1, ColorWave::triangle, true,    10},
    {false, HUE_RED      , HUE_RED       ,  0, ColorWave::none           , false,     0},
    {false, HUE_RED      , HUE_RED       ,  0, ColorWave::none           , false,     0} } };

const MultiWave::PatternConfig multiWaveOrangeUpForBody PROGMEM = {
  true,     // bounded by panel
  // rand   posHue         negHue       waves  type                        down     dly
  { {false, HUE_BSU_ORANGE, HUE_RED       ,  -1, ColorWave::triangle, true,    30},
    {false, HUE_RED      , HUE_RED       ,  0, ColorWave::none           , false,     0},
    {false, HUE_RED      , HUE_RED       ,  0, ColorWave::none           , false,     0} } };

const MultiWave::PatternConfig multiWaveOrangeUpForHead PROGMEM = {
  true,     // bounded by panel
  // rand   posHue         negHue       waves  type                        down     dly
  { {false, HUE_BSU_ORANGE, HUE_RED       ,  -1, ColorWave::triangle, true,    60},
    {false, HUE_RED      , HUE_RED       ,  0, ColorWave::none           , false,     0},
    {false, HUE_RED      , HUE_RED       ,  0, ColorWave::none           , false,     0} } };

const MultiWave::PatternConfig multiWave1Random PROGMEM = {
  true,     // bounded by panel
  // rand   posHue         negHue       waves  type                        down     dly
  { {true , HUE_RED      , HUE_RED       ,  3, ColorWave::quadraticEasing, false,    60},
    {false, HUE_RED      , HUE_RED       ,  0, ColorWave::none           , false,     0},
    {false, HUE_RED      , HUE_RED       ,  0, ColorWave::none           , false,     0} } };

const MultiWave::PatternConfig multiWave1RandomForBody PROGMEM = {
  true,     // bounded by panel
  // rand   posHue         negHue       waves  type                        down     dly
  { {true , HUE_RED      , HUE_RED       ,  2, ColorWave::quadraticEasing, false,    60},
    {false, HUE_RED      , HUE_RED       ,  0, ColorWave::none           , false,     0},
    {false, HUE_RED      , HUE_RED       ,  0, ColorWave::none           , false,     0} } };

const MultiWave::PatternConfig multiWave1RandomForHead PROGMEM = {
  true,     // bounded by panel
  // rand   posHue         negHue       waves  type                        down     dly
  { {true , HUE_RED      , HUE_RED       ,  1, ColorWave::quadraticEasing, false,   180},
    {false, HUE_RED      , HUE_RED       ,  0, ColorWave::none           , false,     0},
    {false, HUE_RED      , HUE_RED       ,  0, ColorWave::none           , false,     0} } };

const MultiWave::PatternConfig multiWave2Random PROGMEM = { 
  false,    // display across all symmetrical pixels
  // rand   posHue         negHue       waves  type                        down     dly
  { {true , HUE_RED      , HUE_RED       ,  1, ColorWave::quadraticEasing, false,   255},
    {false, HUE_RED      , HUE_RED       ,  5, ColorWave::quadraticEasing, false,   127},
    {false, HUE_RED      , HUE_RED       ,  0, ColorWave::none           , false,     0} } };


// ---------- split-rotation patterns ----------

// Elements:
//     mode           original or symmetric
//     randomHue      true if the hues should be selected at random
//     fgHue          foreground (moving pixels) hue
//     bgHue          background hue
////     fgInterval     number of background pixels minus one between each moving pixel
//     fgInterval     number of background pixels between moving pixels in a rotation set
//     directionDown  true to rotate one way, false to rotate the other way
//     delayMs        delay between rotation steps

// The example delays below should approximately sync with 120 bpm on a test
// strip where each panel has 28 pixels, requiring 7 step to complete a rotation.

//                                                                             mode                        rand  fg       bg      fi  down   dly
const SplitRotation::PatternConfig splitRotationSymRandom6LSlow     PROGMEM = {SplitRotation::symmetrical, true, HUE_RED, HUE_RED, 6, true , 143};
const SplitRotation::PatternConfig splitRotationSymRandom6LMed      PROGMEM = {SplitRotation::symmetrical, true, HUE_RED, HUE_RED, 6, true ,  50};


// ---------- rainbow patterns ----------

// Elements:
//     directionDown  true to rotate one way, false to rotate the other way
//     delayMs        ms delay between rotation steps

//                                                          dirDn   dly
const Rainbow::PatternConfig rainbowVerySlow     PROGMEM = {false,  300};
const Rainbow::PatternConfig rainbowSlowSoothing PROGMEM = {false,  100};
const Rainbow::PatternConfig rainbowFastRight    PROGMEM = {false,   20};


// ---------- sparkle patterns ----------

// Elements:
//     fgColorCode  color code of the sparkle color (set both fg and bg to black for random colors)
//     bgColorCode  color code of the background color
//     density      number of foreground (sparkle) pixels on simultaneously (1 to 255)
//     dwellMs      period that a set of sparkles is on (lit)
//     changeMs     period between sparkle sets (changes)

//                                                    fg (sparkle)  bg         den  dwl   chng
const Sparkle::PatternConfig greenLasers   PROGMEM = {CRGB::Green,  CRGB::Black, 6,  30L,  30L};



// ---------- solid color patterns ----------

// Elements:
//     startHue    starting hue
//     endHue      end hue
//     saturation  the S in HSV
//     delay       ms delay between rotation steps

//                                                            startingHue     endHue          Sat  Dly
const SolidColor::PatternConfig solidGreens14Sec   PROGMEM = {HUE_GREEN     , HUE_AQUA      , 255, 14000 / (192 - 128)};
const SolidColor::PatternConfig solidAqua          PROGMEM = {HUE_AQUA      , HUE_AQUA      , 255,  100               };
const SolidColor::PatternConfig solidRed           PROGMEM = {HUE_RED       , HUE_RED       , 255,  100               };
const SolidColor::PatternConfig solidRainbow90Sec  PROGMEM = {(HSVHue) 0    , (HSVHue) 255  , 255, 90000 / 255        };
const SolidColor::PatternConfig solidRainbow9Sec   PROGMEM = {(HSVHue) 0    , (HSVHue) 255  , 255, 9000 / 255         };
const SolidColor::PatternConfig solidRainbow1Sec   PROGMEM = {(HSVHue) 0    , (HSVHue) 255  , 255, 1000 / 255         };


// ---------- Blocks Patterns ----------

// Block definition elements:
//     hue
//     saturation
//     numPixels    number of pixels at hue, 0 if none (i.e., an unused Block)
//
// Elements:
//     delayMs                delay between rotation steps (0 for no rotation)
//     Block[8]               an array of Block definitions

const Blocks::PatternConfig redShotsDown PROGMEM =
    {   1, { {HUE_RED       , 255,  5},
             {HUE_RED       ,   0, 40},
             {HUE_RED       , 255, 0},
             {HUE_PURPLE    , 255, 0},
             {HUE_ORANGE    , 255, 0},
             {HUE_BLUE      , 255, 0},
             {HUE_RED       , 255, 0},
             {HUE_RED       , 255, 0} } };

//const Blocks::PatternConfig americaFuckYeah PROGMEM =
//    { 100, { {HUE_RED       , 255, 10},
//             {HUE_RED       ,   0, 10}, // this is really white
//             {HUE_BLUE      , 255, 10},
//             {HUE_RED       , 255, 0},
//             {HUE_RED       , 255, 0},
//             {HUE_RED       , 255, 0},
//             {HUE_RED       , 255, 0},
//             {HUE_RED       , 255, 0} } };


#endif  // #ifndef __PATTERN_DEFINITIONS_H

