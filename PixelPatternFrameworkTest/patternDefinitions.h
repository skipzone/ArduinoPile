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

#include "FastLED.h"
#include "SolidColor.h"
#include "stockPatternDefinitions.h"

using namespace pixelPattern;


// ---------- solid color patterns ----------

// Elements:
//     startHue    starting hue
//     endHue      end hue
//     saturation  the S in HSV
//     delay       ms delay between rotation steps

//                                                             startingHue     endHue          Sat  Dly
const SolidColor::PatternConfig solidBlues7Sec      PROGMEM = {HUE_AQUA      , HUE_PURPLE    , 255, 7000 / (192 - 128)};


// ---------- sparkle patterns ----------

// Elements:
//     fgColorCode  color code of the sparkle color (set both fg and bg to black for random colors)
//     bgColorCode  color code of the background color
//     density      number of foreground (sparkle) pixels on simultaneously (1 to 255)
//     dwellMs      period that a set of sparkles is on (lit)
//     changeMs     period between sparkle sets (changes)

//                                                          fg (sparkle)  bg         den  dwl   chng
const Sparkle::PatternConfig sparkleBlueAmbience PROGMEM = {CRGB::White,  CRGB::Blue , 1,  15L, 150L};


// ---------- Blocks Patterns ----------

// Block definition elements:
//     hue
//     numPixels    number of pixels at hue, 0 if none (i.e., an unused Block)
//
// Elements:
//     delayMs                delay between rotation steps (0 for no rotation)
//     Block[8]               an array of Block definitions

const Blocks::PatternConfig classicXmasLightsRotate PROGMEM =
    { 500, { {HUE_RED       , 255, 1},
             {HUE_GREEN     , 255, 1},
             {HUE_BSU_ORANGE, 255, 1},
             {HUE_PURPLE    , 255, 1},
             {HUE_ORANGE    , 255, 1},
             {HUE_BLUE      , 255, 1},
             {HUE_RED       , 255, 0},
             {HUE_RED       , 255, 0} } };

const Blocks::PatternConfig americaFuckYeah PROGMEM =
    { 100, { {HUE_RED       , 255, 10},
             {HUE_RED       ,   0, 10}, // this is really white
             {HUE_BLUE      , 255, 10},
             {HUE_RED       , 255, 0},
             {HUE_RED       , 255, 0},
             {HUE_RED       , 255, 0},
             {HUE_RED       , 255, 0},
             {HUE_RED       , 255, 0} } };



#endif  // #ifndef __PATTERN_DEFINITIONS_H

