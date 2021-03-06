/*******************************************************************
 *                                                                 *
 * Pixel Pattern Library ESP8266 Web Page Test                     *
 * customized for Mzfit's Kaleidoscope Mirror                      *
 *                                                                 *
 * Pattern Definitions                                             *
 *                                                                 *
 * by Ross Butler  Nov. 2016                                       * 
 *                                                                 *
 *******************************************************************/

#pragma once

#include "FastLED.h"
#include "stockPatternConfigurations.h"

using namespace pixelPattern;

// colors for WS2812B 5V pixel strips purchased in 2013
//#define BSU_BLUE           (CRGB::HTMLColorCode) 0x0000FF
//#define BSU_ORANGE         (CRGB::HTMLColorCode) 0xC03000  
//#define HUE_BSU_BLUE       (HSVHue) 160
//#define HUE_BSU_ORANGE     (HSVHue) 28
#define VIKINGS_PURPLE     (CRGB::HTMLColorCode) 0x6000FF
#define VIKINGS_YELLOW     (CRGB::HTMLColorCode) 0x707000
#define HUE_VIKINGS_PURPLE (HSVHue) 192
#define HUE_VIKINGS_YELLOW (HSVHue) 64


// ---------- sparkle patterns ----------

// Elements:
//     fgColorCode  color code of the sparkle color (set both fg and bg to black for random colors)
//     bgColorCode  color code of the background color
//     density      number of foreground (sparkle) pixels on simultaneously (1 to 255)
//     dwellMs      period that a set of sparkles is on (lit)
//     changeMs     period between sparkle sets (changes)

//                                                        fg (sparkle)     bg             den  dwl   chng
const Sparkle::PatternConfig sparkleVikings1   PROGMEM = {VIKINGS_PURPLE,  VIKINGS_YELLOW,  8,  50L,  50L};
const Sparkle::PatternConfig sparkleVikings2   PROGMEM = {VIKINGS_YELLOW,  VIKINGS_PURPLE,  8,  50L,  50L};

const Sparkle::PatternConfig sparkleRedCrazy1  PROGMEM = {CRGB::Red     ,  CRGB::Cyan    ,  3,  15L,  15L};
const Sparkle::PatternConfig sparkleRedCrazy2  PROGMEM = {CRGB::Red     ,  CRGB::Blue    ,  3,  15L,  15L};
const Sparkle::PatternConfig sparkleRedCrazy3  PROGMEM = {CRGB::Red     ,  CRGB::Black   ,  5,  15L,  15L};


// ---------- split-rotation patterns ----------

// Elements:
//     mode           original or symmetric
//     randomHue      true if the hues should be selected at random
//     fgHue          foreground (moving pixels) hue
//     bgHue          background hue
//     fgInterval     number of background pixels between moving pixels in a rotation set
//     directionDown  true to rotate one way, false to rotate the other way
//     delayMs        delay between rotation steps

// The example delays below should approximately sync with 120 bpm on a test
// strip where each panel has 28 pixels, requiring 7 step to complete a rotation.

//                                                                             mode                        rand   fg                  bg                 fi  down   dly
const SplitRotation::PatternConfig splitRotationVikings8Medium      PROGMEM = {SplitRotation::original   , false, HUE_VIKINGS_YELLOW, HUE_VIKINGS_PURPLE, 8, false,  60};

const SplitRotation::PatternConfig splitRotationRed8RFast           PROGMEM = {SplitRotation::original   , false, HUE_RED           , HUE_AQUA          , 8, false,  30};

const SplitRotation::PatternConfig splitRotationRandom4RSlow        PROGMEM = {SplitRotation::original   , true , HUE_RED           , HUE_RED           , 4, false, 120};
const SplitRotation::PatternConfig splitRotationRandom4LSlow        PROGMEM = {SplitRotation::original   , true , HUE_RED           , HUE_RED           , 4, true , 120};
const SplitRotation::PatternConfig splitRotationRandom8RSlow        PROGMEM = {SplitRotation::original   , true , HUE_RED           , HUE_RED           , 8, false, 120};
const SplitRotation::PatternConfig splitRotationRandom8LSlow        PROGMEM = {SplitRotation::original   , true , HUE_RED           , HUE_RED           , 8, true , 120};

const SplitRotation::PatternConfig splitRotationRandom8RMedium      PROGMEM = {SplitRotation::original   , true , HUE_RED           , HUE_RED           , 8, false,  60};
const SplitRotation::PatternConfig splitRotationRandom8LMedium      PROGMEM = {SplitRotation::original   , true , HUE_RED           , HUE_RED           , 8, true ,  60};

const SplitRotation::PatternConfig splitRotationRandom8RFast        PROGMEM = {SplitRotation::original   , true , HUE_RED           , HUE_RED           , 8, false,  30};
const SplitRotation::PatternConfig splitRotationRandom8LFast        PROGMEM = {SplitRotation::original   , true , HUE_RED           , HUE_RED           , 8, true ,  30};
const SplitRotation::PatternConfig splitRotationRandom8RDizzy       PROGMEM = {SplitRotation::original   , true , HUE_RED           , HUE_RED           , 8, false,  15};
const SplitRotation::PatternConfig splitRotationRandom8LDizzy       PROGMEM = {SplitRotation::original   , true , HUE_RED           , HUE_RED           , 8, true ,  15};


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

const MovingDot::PatternConfig movingDotRedRecoil        PROGMEM = {CRGB::Red      , CRGB::Black , false, false, false, false, true ,  30L,  10L,    0,    0};
const MovingDot::PatternConfig movingDotRedFast          PROGMEM = {CRGB::Red      , CRGB::Black , false, false, false, false, false,  10L,   0L,    0,    0};
const MovingDot::PatternConfig movingDotRedDizzy         PROGMEM = {CRGB::Red      , CRGB::Black , false, false, false, false, false,   5L,   0L,    0,    0};
const MovingDot::PatternConfig movingDotRedEpilepsy      PROGMEM = {CRGB::Red      , CRGB::Black , false, false, false, false, false,   3L,   0L,    0,    0};

const MovingDot::PatternConfig movingDotRandomPaintMed   PROGMEM = {CRGB::Black    , CRGB::Black ,  true,  true,  true,  true, false,  30L,   0L,    0,    0};
const MovingDot::PatternConfig movingDotRandom           PROGMEM = {CRGB::Black    , CRGB::Black ,  true, false,  true, false, false,  30L,   0L,    0,    0};
const MovingDot::PatternConfig movingDotRandomFast       PROGMEM = {CRGB::Black    , CRGB::Black ,  true, false,  true, false, false,  10L,   0L,    0,    0};


// ---------- rainbow patterns ----------

// Elements:
//     directionDown  true to rotate one way, false to rotate the other way
//     delayMs        ms delay between rotation steps

//                                                           dirDn   dly
const Rainbow::PatternConfig rainbowForComingDown PROGMEM = {false,   25};
const Rainbow::PatternConfig rainbowSpinLeft      PROGMEM = {true ,   10};
const Rainbow::PatternConfig rainbowSpinRight     PROGMEM = {false,   10};
const Rainbow::PatternConfig rainbowRollingLeft   PROGMEM = {true ,    2};
const Rainbow::PatternConfig rainbowRollingRight  PROGMEM = {false,    2};


