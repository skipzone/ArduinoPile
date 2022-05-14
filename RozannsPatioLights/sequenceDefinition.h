/*******************************************************************
 *                                                                 *
 * Patten Sequence Definition for Rozann's Patio Lights            *
 *                                                                 *
 * Sequence by Joshua Anderson  Nov. 2015                          * 
 *                                                                 *
 *******************************************************************/

#ifndef __SEQUENCE_DEFINITION_H
#define __SEQUENCE_DEFINITION_H

// Elements in each pattern element:
//     pattern function
//     duration (number of seconds)
//     pattern configuration (prefix with ampersand)

const patternDef_t patternsDefs[] PROGMEM = {


/*********************************
 * House Patio and Soffit Lights *
 *********************************/

#if defined(HOUSE_PATIO_AND_SOFFIT_LIGHTS) || defined(DEVELOPMENT_STRIP)

  {XMAS_LIGHTS    ,  1,  &xmasLightsClassic},

  {MULTI_WAVE    ,  30,  &multiWaveXmas8},

  {XMAS_LIGHTS    ,  30,  &xmasLightsClassic},

  {SHINE        ,  30,  &blueShine10Sec},

  {RED_WHITE_AND_BLUE,  30,  &americaFuckYeah},

  {RAINBOW      ,  30,  &rainbowVerySlow},

  {MULTI_WAVE    ,  30,  &multiWave1Random},
  
  {XMAS_LIGHTS    ,  30,  &xmasLightsRedGreen},

  {MULTI_WAVE    ,  30,  &multiWave3SquareA},

  {MOVING_DOT    ,  30,  &movingDotRandomZipper},

  {MOVING_DOT    ,  30,  &movingDotRandomPaintSlow},

  {SPARKLE      ,   30,  &sparkleBluePretty},

  {MULTI_WAVE    ,  30,  &multiWaveBsu},

  {MULTI_WAVE    ,  30,  &multiWave3Cluster},

  // ---------- Patterns below must be manually selected. ----------

  {END_OF_AUTO_PATTERNS, 0, NULL},

  {SOLID_COLOR  ,  0,  &solidPink},
  {SOLID_COLOR  ,  0,  &solidPurple},
  {SOLID_COLOR  ,  0,  &solidBlue},
  {SOLID_COLOR  ,  0,  &solidAqua},
  {SOLID_COLOR  ,  0,  &solidGreen},
  {SOLID_COLOR  ,  0,  &solidYellow},
  {SOLID_COLOR  ,  0,  &solidBSUOrange},
  {SOLID_COLOR  ,  0,  &solidRed},
  {SOLID_COLOR  ,  0,  &solidWhite},


/*************
 * Santa Hat *
 *************/

#elif defined(SANTA_HAT)

  {XMAS_LIGHTS    ,  1,  &xmasLightsClassic},

  {MULTI_WAVE    ,  60,  &multiWaveXmas},

  {SHINE        ,  60,  &blueShine10Sec},

  {XMAS_LIGHTS    ,  60,  &xmasLightsRed4Green4},

  {RAINBOW      ,  60,  &rainbowManicRight},

  {MULTI_WAVE    ,  60,  &multiWave1Random},
  
  {SPARKLE      ,   60,  &sparkleBluePretty},

  {MULTI_WAVE    ,  60,  &multiWave3SquareA},

  {MOVING_DOT    ,  60,  &movingDotRandomPaint},

  {MULTI_WAVE    ,  60,  &multiWave1Random},
  
  {SPARKLE      ,   60,  &sparkleGreenPretty},

  {MULTI_WAVE    ,  60,  &multiWave3Cluster},

  {RAINBOW      ,  60,  &rainbowSlowSoothing},

  // ---------- Patterns below must be manually selected. ----------

  {END_OF_AUTO_PATTERNS, 0, NULL},

  {SOLID_COLOR  ,  0,  &solidPink},
  {SOLID_COLOR  ,  0,  &solidPurple},
  {SOLID_COLOR  ,  0,  &solidBlue},
  {SOLID_COLOR  ,  0,  &solidAqua},
  {SOLID_COLOR  ,  0,  &solidGreen},
  {SOLID_COLOR  ,  0,  &solidYellow},
  {SOLID_COLOR  ,  0,  &solidBSUOrange},
  {SOLID_COLOR  ,  0,  &solidRed},
  {SOLID_COLOR  ,  0,  &solidWhite},


/*************************
 * Rozann's Patio Lights *
 *************************/

#elif defined(ROZANNS_PATIO_LIGHTS)

  {MOVING_DOT    ,  10,  &movingDotJoshua1},

  {RED_WHITE_AND_BLUE,  30,  &americaYeahJoshua},

  {RAINBOW      ,  30,  &rainbowRightJoshua},

  {SPLIT_ROTATION,  15,  &splitRotationRandom8RFast},
 
  {MULTI_WAVE    ,  10,  &multiWave1Random},
  
  {SPLIT_ROTATION,  20,  &splitRotationRandom8RDizzy},

  {MULTI_WAVE    ,  30,  &multiWave3ClusterJoshua},

  {MULTI_WAVE    ,  30,  &multiWave3SquareA},

  {MOVING_DOT    ,  33,  &movingDotRandomZipper},

  {MOVING_DOT    ,  30,  &movingDotRandomPaint},

  {SPARKLE      ,   6,  &sparkleBlueJoshua},

 {SPARKLE      ,   6,  &sparkleBlueCrazy},

 {MULTI_WAVE    ,  15,  &multiWaveBsu},

  {SPARKLE      ,   6,  &sparkleGreenCrazy},

  {MULTI_WAVE    ,  20,  &multiWave3Cluster},

  // ---------- Patterns below must be manually selected. ----------

  {END_OF_AUTO_PATTERNS, 0, NULL},

  {SOLID_COLOR  ,  0,  &solidPink},
  {SOLID_COLOR  ,  0,  &solidPurple},
  {SOLID_COLOR  ,  0,  &solidBlue},
  {SOLID_COLOR  ,  0,  &solidAqua},
  {SOLID_COLOR  ,  0,  &solidGreen},
  {SOLID_COLOR  ,  0,  &solidYellow},
//  {SOLID_COLOR  ,  0,  &solidOrange},
  {SOLID_COLOR  ,  0,  &solidBSUOrange},
  {SOLID_COLOR  ,  0,  &solidRed},
  {SOLID_COLOR  ,  0,  &solidWhite},
  {SOLID_COLOR  ,  0,  &solidLightPink},
  {SOLID_COLOR  ,  0,  &solidLightPurple},
  {SOLID_COLOR  ,  0,  &solidLightBlue},
  {SOLID_COLOR  ,  0,  &solidLightAqua},
  {SOLID_COLOR  ,  0,  &solidLightGreen},
  {SOLID_COLOR  ,  0,  &solidLightYellow},
  {SOLID_COLOR  ,  0,  &solidLightOrange},
//  {SOLID_COLOR  ,  0,  &solidLightBSUOrange},
  {SOLID_COLOR  ,  0,  &solidLightRed},
  {SOLID_COLOR  ,  0,  &solidWarmWhite},

#endif


/* ----- unused -----

  //{SHINE        ,  10,  &blueShine10Sec},

  //{SOLID_COLOR  ,  10,  &solidBlues7Sec},
//  {MOVING_DOT    ,  9,  &movingDotRandomPong},

//  {SPLIT_ROTATION,  15,  &splitRotationRandom8RMedium},

//  {MULTI_WAVE    ,  20,  &multiWave1Random},

//  {MOVING_DOT    ,  20,  &movingDotRandomLift},

//  {MULTI_WAVE    ,  30,  &multiWave3 },

//  {MOVING_DOT    ,  15,  &movingDotRandomShoot},

//  {MULTI_WAVE    ,  20,  &multiWave3SquareB},

//  {MULTI_WAVE    ,  30,  &multiWave2Random},
  // {SPARKLE      ,   6,  &sparkleGreenCrazy3},

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

//  {SPARKLE      ,   3,  &sparkleRedCrazy1},
//  {SPARKLE      ,   3,  &sparkleRedCrazy2},
//  {SPARKLE      ,   10,  &sparkleRedCrazy3},

//  {SPARKLE      ,  10,  &sparkleBlueCrazy},

//  {MOVING_DOT    ,  10,  &movingDotRandomPaint},

  //{SPARKLE      ,   6,  &sparkleBlueCrazy3},

*/

};


#endif  // #ifndef __SEQUENCE_DEFINITION_H

