/*******************************************************************
 *                                                                 *
 * Johns Tripper Stick                                             *
 *                                                                 *
 * Patten Sequence Definitions                                     *
 *                                                                 *
 * by Ross Butler  June 2016                                       * 
 *                                                                 *
 *******************************************************************/

#ifndef __SEQUENCE_DEFINITION_H
#define __SEQUENCE_DEFINITION_H

#include "patternDefinitions.h"

using namespace pixelPattern;


const PatternDef patternDefs[] PROGMEM = {
//  {SolidColor::id, 15000L, &solidBlues7Sec},
  {Sparkle::id,    15000L, &sparkleRandom},
  {Sparkle::id,    15000L, &sparkleBluePretty},
  {Sparkle::id,    15000L, &sparkleBlueCrazy},
//  {Blocks::id,     15000L, &classicXmasLightsRotate},
  {Blocks::id,     15000L, &americaFuckYeah},
  {Rainbow::id,    15000L, &rainbowManicRight},
  {Rainbow::id,    15000L, &rainbowManicLeft},
//  {Glint::id,      15000L, &blueGlint10Sec},
//  {MovingDot::id,  15000L, &movingDotRandomPong},
  {MovingDot::id,  15000L, &movingDotRandomLift},
  {MovingDot::id,  15000L, &movingDotRandomZipper},
  {MovingDot::id,  15000L, &movingDotRandomPaint},
  {MovingDot::id,  15000L, &movingDotRandomPaintSlow},
//  {MovingDot::id,  15000L, &movingDotRandomShoot},
  {SplitRotation::id, 15000L, &splitRotationOrigRandom6RMedium},
  {SplitRotation::id, 15000L, &splitRotationOrigRandom6LMedium},
  {SplitRotation::id, 15000L, &splitRotationSymRandom6RVerySlow},
  {SplitRotation::id, 15000L, &splitRotationSymRandom6LVerySlow},
  {SplitRotation::id, 15000L, &splitRotationSymRandom6RMedium},
  {SplitRotation::id, 15000L, &splitRotationSymRandom6LMedium},
  {SplitRotation::id, 15000L, &splitRotationSymRandom6RFast},
  {SplitRotation::id, 15000L, &splitRotationSymRandom6LFast},
  {MultiWave::id,     15000L, &multiWave1Random},
//  {MultiWave::id,     15000L, &multiWaveBsu},
//  {MultiWave::id,     15000L, &multiWaveXmas},
//  {MultiWave::id,     15000L, &multiWaveXmas8},
  {MultiWave::id,     15000L, &multiWave2Random },
  {MultiWave::id,     15000L, &multiWave3 },
  {MultiWave::id,     15000L, &multiWave3SquareA },
  {MultiWave::id,     15000L, &multiWave3SquareB },
  {MultiWave::id,     15000L, &multiWave3ClusterFuck },
  // ----- manual-selection-only patterns -----
  {Blocks::id    ,        0L, &classicXmasLights},
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


#endif  // #ifndef __SEQUENCE_DEFINITION_H

