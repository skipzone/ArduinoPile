/*******************************************************************
 *                                                                 *
 * Gay Lynn's Patio and Soffit Holiday Lights                      *
 *                                                                 *
 * Pattern Configurations                                          *
 *                                                                 *
 * by Ross Butler  Oct. 2016                                       * 
 *                                                                 *
 *******************************************************************/

#pragma once

#include "FastLED.h"
#include "stockPatternConfigurations.h"

using namespace pixelPattern;


// ---------- Blocks Patterns ----------

// Block definition elements:
//     hue
//     saturation
//     numPixels    number of pixels at hue, 0 if none (i.e., an unused Block)
//
// Elements:
//     delayMs                delay between rotation steps (0 for no rotation)
//     Block[8]               an array of Block definitions

const Blocks::PatternConfig xmasRed4Green4 PROGMEM =
    {  0, { {HUE_RED        , 255, 4},
            {HUE_GREEN      , 255, 4} } };

const Blocks::PatternConfig xmasRedWhite PROGMEM =
    {  0, { {HUE_RED        , 255, 1},
            {HUE_RED        ,   0, 1} } };

const Blocks::PatternConfig xmasGreenWhite PROGMEM =
    {  0, { {HUE_GREEN      , 255, 1},
            {HUE_GREEN      ,   0, 1} } };

const Blocks::PatternConfig xmasBlueWhite PROGMEM =
    {  0, { {HUE_BLUE       , 255, 1},
            {HUE_BLUE       ,   0, 1} } };

const Blocks::PatternConfig xmasBlue3White PROGMEM =
    {  0, { {HUE_BLUE       , 255, 3},
            {HUE_BLUE       ,   0, 1} } };

const Blocks::PatternConfig xmasCandyStripe PROGMEM =
    {150, { {HUE_RED        , 255, 3},
            {HUE_RED        ,   0, 3} } };

const Blocks::PatternConfig americaFuckYeah PROGMEM =
    {150, { {HUE_RED        , 255, 9},
            {HUE_RED        ,   0, 9},
            {HUE_BLUE       , 255, 9} } };

const Blocks::PatternConfig americaFuckYeahPatio PROGMEM =
    {  0, { {HUE_RED        , 255, 3},
            {HUE_RED        ,   0, 3},
            {HUE_BLUE       , 255, 3} } };

const Blocks::PatternConfig bsuOrangeBluePatio PROGMEM =
    {  0, { {HUE_BSU_ORANGER, 255, 4},
            {HUE_BLUE       , 255, 5} } };

const Blocks::PatternConfig xmasRedGreenPatio PROGMEM =
    {  0, { {HUE_RED        , 255, 5},
            {HUE_GREEN      , 255, 4} } };

const Blocks::PatternConfig xmasCandyStripePatio PROGMEM =
    {  0, { {HUE_RED        , 255, 1},
            {HUE_RED        , 100, 1} } };

const Blocks::PatternConfig xmasBlue2WhitePatio PROGMEM =
    {  0, { {HUE_RED        , 100, 1},
            {HUE_BLUE       , 255, 2} } };

const Blocks::PatternConfig ukraineBlueYellowPatio PROGMEM =
    {  0, { {HUE_BLUE       , 255, 5},
            {HUE_YELLOW     , 255, 4} } };


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

const MultiWave::PatternConfig multiWaveCandyCane PROGMEM = {
  false,     // display across all symmetrical pixels
  // rand   posHue       posSat negHue         negSat waves type                        down     dly
  { {false, HUE_RED      , 255, HUE_RED        ,    0,  12, ColorWave::cubicEasing    , false,   100},
    {false, HUE_RED      , 255, HUE_RED        ,  255,   0, ColorWave::none           , false,     0},
    {false, HUE_RED      , 255, HUE_RED        ,  255,   0, ColorWave::none           , false,     0} } };


// ---------- solid color patterns ----------

// Elements:
//     startHue    starting hue
//     endHue      end hue
//     saturation  the S in HSV
//     delay       ms delay between rotation steps

//                                                             startingHue     endHue          Sat  Dly
const SolidColor::PatternConfig solidAdjWhite       PROGMEM = {HUE_RED       , HUE_RED       , 100, 500L};


