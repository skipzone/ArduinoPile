/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Stock Pattern Configurations                                    *
 *                                                                 *
 * by Ross Butler   Dec. 2015                                      *
 *                                                                 *
 *****************************************************************/

#pragma once

#include "FastLED.h"
#include "SolidColor.h"
#include "Sparkle.h"
#include "Blocks.h"
#include "Rainbow.h"
#include "Glint.h"
#include "MovingDot.h"
#include "SplitRotation.h"
#include "MultiWave.h"

#define BSU_BLUE           (CRGB::HTMLColorCode) 0x0000FF
#define BSU_ORANGE         (CRGB::HTMLColorCode) 0xD03000
#define BSU_ORANGER        (CRGB::HTMLColorCode) 0xF01600
#define HUE_BSU_BLUE       (HSVHue) 160
#define HUE_BSU_ORANGE     (HSVHue) 16  // this is a good orange for the pixel strawberry strings
#define HUE_BSU_ORANGER    (HSVHue) 9


namespace pixelPattern {


// ---------- solid color patterns ----------

// Elements:
//     startHue    starting hue
//     endHue      end hue
//     saturation  the S in HSV
//     delay       ms delay between rotation steps

//                                                             startingHue     endHue          Sat  Dly
const SolidColor::PatternConfig solidPink           PROGMEM = {HUE_PINK      , HUE_PINK      , 255, 500L};
const SolidColor::PatternConfig solidPurple         PROGMEM = {HUE_PURPLE    , HUE_PURPLE    , 255, 500L};
const SolidColor::PatternConfig solidBlue           PROGMEM = {HUE_BLUE      , HUE_BLUE      , 255, 500L};
const SolidColor::PatternConfig solidAqua           PROGMEM = {HUE_AQUA      , HUE_AQUA      , 255, 500L};
const SolidColor::PatternConfig solidGreen          PROGMEM = {HUE_GREEN     , HUE_GREEN     , 255, 500L};
const SolidColor::PatternConfig solidYellow         PROGMEM = {HUE_YELLOW    , HUE_YELLOW    , 255, 500L};
const SolidColor::PatternConfig solidOrange         PROGMEM = {HUE_ORANGE    , HUE_ORANGE    , 255, 500L};
const SolidColor::PatternConfig solidBSUOrange      PROGMEM = {HUE_BSU_ORANGE, HUE_BSU_ORANGE, 255, 500L};
const SolidColor::PatternConfig solidBSUOranger     PROGMEM = {HUE_BSU_ORANGER, HUE_BSU_ORANGER, 255, 500L};
const SolidColor::PatternConfig solidRed            PROGMEM = {HUE_RED       , HUE_RED       , 255, 500L};
const SolidColor::PatternConfig solidWhite          PROGMEM = {HUE_YELLOW    , HUE_YELLOW    ,   0, 500L};
const SolidColor::PatternConfig solidLightPink      PROGMEM = {HUE_PINK      , HUE_PINK      , 112, 500L};
const SolidColor::PatternConfig solidLightPurple    PROGMEM = {HUE_PURPLE    , HUE_PURPLE    , 144, 500L};
const SolidColor::PatternConfig solidLightBlue      PROGMEM = {HUE_BLUE      , HUE_BLUE      , 112, 500L};
const SolidColor::PatternConfig solidLightAqua      PROGMEM = {HUE_AQUA      , HUE_AQUA      , 112, 500L};
const SolidColor::PatternConfig solidLightGreen     PROGMEM = {HUE_GREEN     , HUE_GREEN     , 112, 500L};
const SolidColor::PatternConfig solidLightYellow    PROGMEM = {HUE_YELLOW    , HUE_YELLOW    , 144, 500L};
const SolidColor::PatternConfig solidLightOrange    PROGMEM = {HUE_ORANGE    , HUE_ORANGE    , 160, 500L};
const SolidColor::PatternConfig solidLightBSUOrange PROGMEM = {HUE_BSU_ORANGE, HUE_BSU_ORANGE, 160, 500L};
const SolidColor::PatternConfig solidLightRed       PROGMEM = {HUE_RED       , HUE_RED       , 144, 500L};
const SolidColor::PatternConfig solidWarmWhite      PROGMEM = {HUE_YELLOW    , HUE_YELLOW    , 128, 500L};


// ---------- sparkle patterns ----------

// Elements:
//     fgColorCode  color code of the sparkle color (set both fg and bg to black for random colors)
//     bgColorCode  color code of the background color
//     density      number of foreground (sparkle) pixels on simultaneously (1 to 255)
//     dwellMs      period that a set of sparkles is on (lit)
//     changeMs     period between sparkle sets (changes)

//                                                        fg (sparkle)  bg         den  dwl   chng
const Sparkle::PatternConfig sparkleBluePretty PROGMEM = {CRGB::White,  CRGB::Blue , 3,  15L,  15L};
const Sparkle::PatternConfig sparkleBlueBSU    PROGMEM = {BSU_ORANGE ,  CRGB::Blue,  8,  50L,  50L};
const Sparkle::PatternConfig sparkleBlueCrazy  PROGMEM = {CRGB::Blue ,  CRGB::Black, 6,  15L,  15L};
const Sparkle::PatternConfig sparkleRandom     PROGMEM = {CRGB::Black,  CRGB::Black, 3,  15L,  15L};


// ---------- Blocks Patterns ----------

// Block definition elements:
//     hue
//     saturation
//     numPixels    number of pixels at hue, 0 if none (i.e., an unused Block)
//
// Elements:
//     delayMs                delay between rotation steps (0 for no rotation)
//     Block[8]               an array of Block definitions

const Blocks::PatternConfig bsuBlock3 PROGMEM =
    {0, { {HUE_BSU_ORANGE, 255, 3},
          {HUE_BLUE      , 255, 3} } };

const Blocks::PatternConfig xmasClassic PROGMEM =
    {0, { {HUE_RED       , 255, 1},
          {HUE_GREEN     , 255, 1},
          {HUE_BSU_ORANGE, 255, 1},
          {HUE_PURPLE    , 255, 1},
          {HUE_ORANGE    , 255, 1},
          {HUE_BLUE      , 255, 1},
          {HUE_RED       , 255, 0},
          {HUE_RED       , 255, 0} } };

const Blocks::PatternConfig xmasRedGreen PROGMEM =
    {0, { {HUE_RED       , 255, 1},
          {HUE_GREEN     , 255, 1} } };


// ---------- rainbow patterns ----------

// Elements:
//     directionDown  true to rotate one way, false to rotate the other way
//     delayMs        ms delay between rotation steps

//                                                          dirDn   dly
const Rainbow::PatternConfig rainbowVerySlow     PROGMEM = {false,  300};
const Rainbow::PatternConfig rainbowSlowSoothing PROGMEM = {false,  100};
const Rainbow::PatternConfig rainbowManicLeft    PROGMEM = {true ,    5};
const Rainbow::PatternConfig rainbowManicRight   PROGMEM = {false,    5};


// ---------- glint patterns ----------

// Elements:
//     bgHue            background hue
//     glintIntervalMs  interval between glints
//     delayMs          delay between glint steps

//                                                   bgHue    intvl  dly
const Glint::PatternConfig blueGlint10Sec PROGMEM = {HUE_BLUE, 7000,  2};


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

//                                                                  fg               bg           randFg randBg chngFg zipper  biDir  d0    d1   hFar  hNear
const MovingDot::PatternConfig allOff                    PROGMEM = {CRGB::Black    , CRGB::Black , false, false, false, false, false, 1000, 1000, 1000, 1000};

const MovingDot::PatternConfig movingDotRandomPong       PROGMEM = {CRGB::Black    , CRGB::Black ,  true, false, false, false, true ,   6L,   6L,    0,    0};
const MovingDot::PatternConfig movingDotRandomLift       PROGMEM = {CRGB::Black    , CRGB::Black ,  true, false, false, false, true ,  30L,   6L,  500, 1500};
const MovingDot::PatternConfig movingDotRandomZipper     PROGMEM = {CRGB::Black    , CRGB::Black ,  true,  true,  true,  true, true ,  20L,   6L,  200, 1000};
const MovingDot::PatternConfig movingDotRandomPaint      PROGMEM = {CRGB::Black    , CRGB::Black ,  true,  true,  true,  true, false,  10L,   6L,    0,    0};
const MovingDot::PatternConfig movingDotRandomPaintSlow  PROGMEM = {CRGB::Black    , CRGB::Black ,  true,  true,  true,  true, false, 100L,   6L,    0,    0};
const MovingDot::PatternConfig movingDotRandomShoot      PROGMEM = {CRGB::Black    , CRGB::Black ,  true, false,  true, false, false,   6L,   6L,    0,    0};


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
const SplitRotation::PatternConfig splitRotationSymRandom6RVerySlow PROGMEM = {SplitRotation::symmetrical, true, HUE_RED, HUE_RED, 6, false, 500};
const SplitRotation::PatternConfig splitRotationSymRandom6LVerySlow PROGMEM = {SplitRotation::symmetrical, true, HUE_RED, HUE_RED, 6, true , 500};
const SplitRotation::PatternConfig splitRotationSymRandom6RSlow     PROGMEM = {SplitRotation::symmetrical, true, HUE_RED, HUE_RED, 6, false, 143};
const SplitRotation::PatternConfig splitRotationSymRandom6LSlow     PROGMEM = {SplitRotation::symmetrical, true, HUE_RED, HUE_RED, 6, true , 143};
const SplitRotation::PatternConfig splitRotationSymRandom6RMedium   PROGMEM = {SplitRotation::symmetrical, true, HUE_RED, HUE_RED, 6, false,  71};
const SplitRotation::PatternConfig splitRotationSymRandom6LMedium   PROGMEM = {SplitRotation::symmetrical, true, HUE_RED, HUE_RED, 6, true ,  71};
const SplitRotation::PatternConfig splitRotationSymRandom6RFast     PROGMEM = {SplitRotation::symmetrical, true, HUE_RED, HUE_RED, 6, false,  36};
const SplitRotation::PatternConfig splitRotationSymRandom6LFast     PROGMEM = {SplitRotation::symmetrical, true, HUE_RED, HUE_RED, 6, true ,  36};
const SplitRotation::PatternConfig splitRotationSymRandom6RDizzy    PROGMEM = {SplitRotation::symmetrical, true, HUE_RED, HUE_RED, 6, false,  18};
const SplitRotation::PatternConfig splitRotationSymRandom6LDizzy    PROGMEM = {SplitRotation::symmetrical, true, HUE_RED, HUE_RED, 6, true ,  18};
const SplitRotation::PatternConfig splitRotationOrigRandom6RMedium  PROGMEM = {SplitRotation::original   , true, HUE_RED, HUE_RED, 6, false,  71};
const SplitRotation::PatternConfig splitRotationOrigRandom6LMedium  PROGMEM = {SplitRotation::original   , true, HUE_RED, HUE_RED, 6, true ,  71};


// ---------- multi-wave patterns ----------

// Each multiwave element consists of one boundedByPanel element and MultiWave::maxWaveforms
// ColorWave elements.  When less than maxWaveforms are needed, set numWaves
// to zero in unused ColorWave definitions.

// ColorWave elements:
//     randomHue       true if the waveform hue should be selected at random
//     posHue          hue of the positive half of the waveform (if randomHue is false)
//     posSaturation   saturation of the positive half of the waveform (if randomHue is false)
//     negHue          hue of the negative half of the waveform (if randomHue is false)
//     negSaturation   saturation of the negative half of the waveform (if randomHue is false)
//     numWaves        no. of complete waveforms in pattern; neg. for halfwave (neg. half black); must be non-zero
//     waveformType    a WaveType enum value (ColorWave::square, ColorWave::triangle, etc.);
//                     ColorWave::none to indicate end of waveforms (this and subsequent waveforms are unused)
//     directionDown   true if the waves should move toward pixel 0, false if they should move the other direction
//     delayMs         the period between each stepped movement of a wave

const MultiWave::PatternConfig multiWaveXmas PROGMEM = {
  true,     // bounded by panel
  // rand   posHue       posSat negHue         negSat waves type                        down     dly
  { {false, HUE_RED      , 255, HUE_GREEN      ,  255,   1, ColorWave::cubicEasing    , false,    60},
    {false, HUE_RED      , 255, HUE_RED        ,  255,   0, ColorWave::none           , false,     0},
    {false, HUE_RED      , 255, HUE_RED        ,  255,   0, ColorWave::none           , false,     0} } };

const MultiWave::PatternConfig multiWaveXmas8 PROGMEM = {
  false,     // display across all symmetrical pixels
  // rand   posHue       posSat negHue         negSat waves type                        down     dly
  { {false, HUE_RED      , 255, HUE_GREEN      ,  255,   8, ColorWave::cubicEasing    , false,   100},
    {false, HUE_RED      , 255, HUE_RED        ,  255,   0, ColorWave::none           , false,     0},
    {false, HUE_RED      , 255, HUE_RED        ,  255,   0, ColorWave::none           , false,     0} } };

const MultiWave::PatternConfig multiWaveBsu PROGMEM = {
  true,     // bounded by panel
  // rand   posHue       posSat negHue         negSat waves type                        down     dly
  { {false, HUE_BSU_BLUE , 255, HUE_BSU_ORANGE ,  255,   1, ColorWave::cubicEasing    , false,    60},
    {false, HUE_RED      , 255, HUE_RED        ,  255,   0, ColorWave::none           , false,     0},
    {false, HUE_RED      , 255, HUE_RED        ,  255,   0, ColorWave::none           , false,     0} } };

const MultiWave::PatternConfig multiWaveBsuSlow PROGMEM = {
  true,     // bounded by panel
  // rand   posHue       posSat negHue         negSat waves type                        down     dly
  { {false, HUE_BSU_BLUE , 255, HUE_BSU_ORANGER,  255,   1, ColorWave::cubicEasing    , false,   180},
    {false, HUE_RED      , 255, HUE_RED        ,  255,   0, ColorWave::none           , false,     0},
    {false, HUE_RED      , 255, HUE_RED        ,  255,   0, ColorWave::none           , false,     0} } };

const MultiWave::PatternConfig multiWave1Random PROGMEM = {
  true,     // bounded by panel
  // rand   posHue       posSat negHue         negSat waves type                        down     dly
  { {true , HUE_RED      , 255, HUE_RED        ,  255,   3, ColorWave::quadraticEasing, false,    30},
    {false, HUE_RED      , 255, HUE_RED        ,  255,   0, ColorWave::none           , false,     0},
    {false, HUE_RED      , 255, HUE_RED        ,  255,   0, ColorWave::none           , false,     0} } };

const MultiWave::PatternConfig multiWave2Random PROGMEM = { 
  false,    // display across all symmetrical pixels
  // rand   posHue       posSat negHue         negSat waves type                        down     dly
  { {true , HUE_RED      , 255, HUE_RED        ,  255,   1, ColorWave::quadraticEasing, false,   255},
    {true , HUE_RED      , 255, HUE_RED        ,  255,   5, ColorWave::quadraticEasing, false,   127},
    {false, HUE_RED      , 255, HUE_RED        ,  255,   0, ColorWave::none           , false,     0} } };

const MultiWave::PatternConfig multiWave3 PROGMEM = { 
  false,    // display across all symmetrical pixels
  // rand   posHue       posSat negHue         negSat waves type                        down     dly
  { {false, HUE_BLUE     , 255, HUE_ORANGE     ,  255,  -1, ColorWave::sine           , false,    30},
    {false, HUE_RED      , 255, HUE_AQUA       ,  255,  -2, ColorWave::sine           , true ,    40},
    {false, HUE_GREEN    , 255, HUE_PINK       ,  255,  -3, ColorWave::sine           , false,    50} } };

const MultiWave::PatternConfig multiWave3SquareA PROGMEM = { 
  true,     // bounded by panel
  // rand   posHue       posSat negHue         negSat waves type                        down     dly
  { {false, HUE_BLUE     , 255, HUE_ORANGE     ,  255,   1, ColorWave::square         , true ,   255},
    {false, HUE_RED      , 255, HUE_AQUA       ,  255,   2, ColorWave::square         , true ,   192},
    {false, HUE_GREEN    , 255, HUE_PINK       ,  255,   3, ColorWave::square         , false,   128} } };

const MultiWave::PatternConfig multiWave3SquareB PROGMEM = { 
  true,     // bounded by panel
  // rand   posHue       posSat negHue         negSat waves type                        down     dly
  { {false, HUE_BLUE     , 255, HUE_ORANGE     ,  255,  -1, ColorWave::square         , true ,    80},
    {false, HUE_RED      , 255, HUE_AQUA       ,  255,  -3, ColorWave::square         , true ,    80},
    {false, HUE_GREEN    , 255, HUE_PINK       ,  255,  -5, ColorWave::square         , false,    80} } };

const MultiWave::PatternConfig multiWave3ClusterFuck PROGMEM = {
  false,    // display across all symmetrical pixels
  // rand   posHue       posSat negHue         negSat waves type                        down     dly
  { {false, HUE_BLUE     , 255, HUE_BLUE       ,  255,   2, ColorWave::cubicEasing    , true ,    80},
    {false, HUE_RED      , 255, HUE_RED        ,  255,   4, ColorWave::triangle       , true ,    60},
    {false, HUE_GREEN    , 255, HUE_GREEN      ,  255,   6, ColorWave::quadraticEasing, true ,    40} } };

}

