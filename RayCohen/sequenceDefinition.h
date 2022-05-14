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


const PatternDef patternDefs[] PROGMEM = {
  {SolidColor::id, 15000L, &solidBlues7Sec},
  {SolidColor::id,  2000L, &solidRed},
  {Sparkle::id,     3000L, &sparkleRandom},
  {SolidColor::id,  2000L, &solidGreen},
  {Sparkle::id,     3000L, &sparkleBluePretty},
  {SolidColor::id,  2000L, &solidBlue},
  {Sparkle::id,     3000L, &sparkleBlueCrazy},
  {Blocks::id,      3000L, &classicXmasLights},
  {Blocks::id,      6000L, &classicXmasLightsRotate},
  {Blocks::id,      6000L, &americaFuckYeah},
  {Rainbow::id,     6000L, &rainbowManicRight},
  {Rainbow::id,     6000L, &rainbowManicLeft},
  {Glint::id,      20000L, &blueGlint10Sec},
  {MovingDot::id,   6000L, &movingDotRandomPong},
  {MovingDot::id,   9000L, &movingDotRandomLift},
  {MovingDot::id,  10000L, &movingDotRandomZipper},
  {MovingDot::id,  10000L, &movingDotRandomPaint},
  {MovingDot::id,  20000L, &movingDotRandomPaintSlow},
  {MovingDot::id,  10000L, &movingDotRandomShoot},
  {SplitRotation::id, 12000L, &splitRotationOrigRandom6RMedium},
  {SplitRotation::id, 12000L, &splitRotationOrigRandom6LMedium},
  {SplitRotation::id, 12000L, &splitRotationSymRandom6RVerySlow},
  {SplitRotation::id, 12000L, &splitRotationSymRandom6LVerySlow},
  {SplitRotation::id, 12000L, &splitRotationSymRandom6LMedium},
  {SplitRotation::id, 12000L, &splitRotationSymRandom6LMedium},
  {SplitRotation::id, 12000L, &splitRotationSymRandom6RFast},
  {SplitRotation::id, 12000L, &splitRotationSymRandom6LFast},
  {MultiWave::id,     12000L, &multiWave1Random},
  {MultiWave::id,     12000L, &multiWaveBsu},
  {MultiWave::id,     12000L, &multiWaveXmas},
  {MultiWave::id,     12000L, &multiWaveXmas8},
  {MultiWave::id,     12000L, &multiWave2Random },
  {MultiWave::id,     12000L, &multiWave3 },
  {MultiWave::id,     12000L, &multiWave3SquareA },
  {MultiWave::id,     12000L, &multiWave3SquareB },
  {MultiWave::id,     12000L, &multiWave3ClusterFuck },
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


const PatternDef ambiencePatternDefs[] PROGMEM = {
  {SolidColor::id,      300L, &solidRed},
  {SolidColor::id,      300L, &solidGreen},
  {SolidColor::id,      300L, &solidBlue},
  {SolidColor::id,      300L, &solidBSUOrange},
  {Sparkle::id,        6000L, &sparkleBlueAmbience},
  {Rainbow::id,       30000L, &rainbowSlowSoothing},
  {MovingDot::id,     10000L, &movingDotRandomPaintSlow},
  {MultiWave::id,     12000L, &multiWaveBsu},
  {MultiWave::id,     12000L, &multiWave3ClusterFuck },
};


#endif  // #ifndef __SEQUENCE_DEFINITION_H

