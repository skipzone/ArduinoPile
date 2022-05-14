/*******************************************************************
 *                                                                 *
 * Pixel Pattern Library Test                                      *
 *                                                                 *
 * Patten Sequence Definitions                                     *
 *                                                                 *
 * by Ross Butler  Dec. 2015                                       * 
 *                                                                 *
 *******************************************************************/

#ifndef __SEQUENCE_DEFINITION_H
#define __SEQUENCE_DEFINITION_H

#include "patternDefinitions.h"

using namespace pixelPattern;

const PatternDef legsPatternDefs[] PROGMEM = {
  {MovingDot::id,         0L, &allOff},
  {MovingDot::id,     25000L, &movingDotRandomPaint},        // power up
  {Sparkle::id,       25000L, &greenLasers},                 // lasers
  {Rainbow::id,       25000L, &rainbowFastRight},            // rainbow
  {MultiWave::id,     25000L, &multiWaveOrangeUp},           // soul sucker
  {MovingDot::id,     25000L, &movingDotPongInverted},       // random inverted pong
  {Blocks::id,        25000L, &redShotsDown},                // bloodstream
  {MovingDot::id,     25000L, &movingDotRandomZipAndHold},   // zip and hold
  {MultiWave::id,     25000L, &multiWave1Random},            // multiwave 1 random
  {SplitRotation::id, 25000L, &splitRotationSymRandom6LMed}, // split rotation
  {MultiWave::id,     25000L, &multiWave2Random },           // multiwave 2 random
};

const PatternDef bodyPatternDefs[] PROGMEM = {
  {MovingDot::id,         0L, &allOff},
  {MultiWave::id,     25000L, &multiWave1RandomForBody},
  {Sparkle::id,       25000L, &greenLasers},
  {Rainbow::id,       25000L, &rainbowFastRight},
  {MultiWave::id,     25000L, &multiWaveOrangeUpForBody},
  {SolidColor::id,    25000L, &solidRainbow90Sec},
  {Blocks::id,        25000L, &redShotsDown},
  {SolidColor::id,    25000L, &solidRainbow9Sec},
  {MultiWave::id,     25000L, &multiWave1RandomForBody},
  {MultiWave::id,     25000L, &multiWave1RandomForBody},
  {SolidColor::id,    25000L, &solidRainbow1Sec},
  {SolidColor::id,        0L, &solidGreens14Sec},
};

const PatternDef headPatternDefs[] PROGMEM = {
  {MovingDot::id,         0L, &allOff},
  {MultiWave::id,     25000L, &multiWave1RandomForHead},
  {Sparkle::id,       25000L, &greenLasers},
  {Rainbow::id,       25000L, &rainbowFastRight},
  {MultiWave::id,     25000L, &multiWaveOrangeUpForHead},
  {SolidColor::id,    25000L, &solidRainbow90Sec},
  {SolidColor::id,    25000L, &solidRed},
  {SolidColor::id,    25000L, &solidRainbow9Sec},
  {MultiWave::id,     25000L, &multiWave1RandomForHead},
  {MultiWave::id,     25000L, &multiWave1RandomForHead},
  {SolidColor::id,    25000L, &solidRainbow1Sec},
  {SolidColor::id,        0L, &solidGreens14Sec},
};

const PatternDef largeEyesPatternDefs[] PROGMEM = {
  {MovingDot::id,         0L, &allOff},
  {SolidColor::id,    25000L, &solidAqua},
  {MovingDot::id,     25000L, &movingDotFastRotateLargeEyes},
  {MovingDot::id,     25000L, &movingDotSlowPongLargeEyes},
  {MovingDot::id,     25000L, &movingDotMedRotateLargeEyes},
  {MovingDot::id,     25000L, &movingDotSlowRotateLargeEyes},
  {SolidColor::id,    25000L, &solidRed},
  {MovingDot::id,     25000L, &movingDotSlowRotateLargeEyes},
  {MovingDot::id,     25000L, &movingDotMedRotateLargeEyes},
  {MovingDot::id,     25000L, &movingDotFastPongLargeEyes},
  {MovingDot::id,     25000L, &movingDotSlowRotateLargeEyes},
};

const PatternDef smallEyesPatternDefs[] PROGMEM = {
  {MovingDot::id,         0L, &allOff},
  {SolidColor::id,    25000L, &solidAqua},
  {MovingDot::id,     25000L, &movingDotFastRotateSmallEyes},
  {MovingDot::id,     25000L, &movingDotSlowPongSmallEyes},
  {MovingDot::id,     25000L, &movingDotMedRotateSmallEyes},
  {MovingDot::id,     25000L, &movingDotSlowRotateSmallEyes},
  {SolidColor::id,    25000L, &solidRed},
  {MovingDot::id,     25000L, &movingDotSlowPongSmallEyes},
  {MovingDot::id,     25000L, &movingDotMedRotateSmallEyes},
  {MovingDot::id,     25000L, &movingDotFastPongSmallEyes},
  {MovingDot::id,     25000L, &movingDotSlowRotateSmallEyes},
};


/*
const PatternDef patternDefs[] PROGMEM = {
  {SolidColor::id, 15000L, &solidBlues7Sec},
  {SolidColor::id,  2000L, &solidRed},
  {Sparkle::id,     3000L, &sparkleRandom},
  {SolidColor::id,  2000L, &solidGreen},
  {Sparkle::id,     3000L, &sparkleBluePretty},
  {SolidColor::id,  2000L, &solidBlue},
  {Sparkle::id,     3000L, &sparkleBlueCrazy},
//  {Blocks::id,      3000L, &classicXmasLights},
//  {Blocks::id,      6000L, &classicXmasLightsRotate},
//  {Blocks::id,      6000L, &americaFuckYeah},
  {Rainbow::id,     6000L, &rainbowManicRight},
  {Rainbow::id,     6000L, &rainbowManicLeft},
  {Glint::id,      20000L, &blueGlint10Sec},
  {MovingDot::id,   6000L, &movingDotRandomPong},
  {MovingDot::id,   9000L, &movingDotRandomLift},
  {MovingDot::id,  10000L, &movingDotRandomZipper},
  {MovingDot::id,  10000L, &movingDotRandomPaint},
  {MovingDot::id,  20000L, &movingDotRandomPaintSlow},
  {MovingDot::id,  10000L, &movingDotRandomShoot},
//  {SplitRotation::id, 12000L, &splitRotationOrigRandom6RMedium},
//  {SplitRotation::id, 12000L, &splitRotationOrigRandom6LMedium},
//  {SplitRotation::id, 12000L, &splitRotationSymRandom6RVerySlow},
//  {SplitRotation::id, 12000L, &splitRotationSymRandom6LVerySlow},
//  {SplitRotation::id, 12000L, &splitRotationSymRandom6LMedium},
//  {SplitRotation::id, 12000L, &splitRotationSymRandom6LMedium},
//  {SplitRotation::id, 12000L, &splitRotationSymRandom6RFast},
//  {SplitRotation::id, 12000L, &splitRotationSymRandom6LFast},
//  {MultiWave::id,     12000L, &multiWave1Random},
//  {MultiWave::id,     12000L, &multiWaveBsu},
//  {MultiWave::id,     12000L, &multiWaveXmas},
//  {MultiWave::id,     12000L, &multiWaveXmas8},
//  {MultiWave::id,     12000L, &multiWave2Random },
//  {MultiWave::id,     12000L, &multiWave3 },
//  {MultiWave::id,     12000L, &multiWave3SquareA },
//  {MultiWave::id,     12000L, &multiWave3SquareB },
//  {MultiWave::id,     12000L, &multiWave3ClusterFuck },
  // ----- manual-selection-only patterns -----
  {SolidColor::id,        0L, &solidPink},
  {SolidColor::id,        0L, &solidPurple},
  {SolidColor::id,        0L, &solidBlue},
  {SolidColor::id,        0L, &solidAqua},
  {SolidColor::id,        0L, &solidGreen},
  {SolidColor::id,        0L, &solidYellow},
  {SolidColor::id,        0L, &solidBSUOrange},
  {SolidColor::id,        0L, &solidRed},
  {SolidColor::id,        0L, &solidWhite},
};


const PatternDef ambiancePatternDefs[] PROGMEM = {
//  {SolidColor::id,      300L, &solidRed},
//  {SolidColor::id,      300L, &solidGreen},
//  {SolidColor::id,      300L, &solidBlue},
//  {SolidColor::id,      300L, &solidBSUOrange},
  {Rainbow::id,       30000L, &rainbowSlowSoothing},
  {MovingDot::id,     10000L, &movingDotRandomPaintSlow},
  {MultiWave::id,     12000L, &multiWaveBsu},
  {MultiWave::id,     12000L, &multiWave3ClusterFuck },
};
*/

#endif  // #ifndef __SEQUENCE_DEFINITION_H

