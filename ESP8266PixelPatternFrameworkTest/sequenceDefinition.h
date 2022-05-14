/*******************************************************************
 *                                                                 *
 * Pixel Pattern Library ESP8266 Web Page Test                     *
 *                                                                 *
 * Patten Sequence Definitions                                     *
 *                                                                 *
 * by Ross Butler  Oct. 2016                                       *
 *                                                                 *
 *******************************************************************/

#pragma once

#include "patternDefinitions.h"

using namespace pixelPattern;

const PatternDef section0PatternDefs[] PROGMEM = {
  {MovingDot::id,         0L, &allOff                           , "Off"},
  {MultiWave::id,     10000L, &multiWaveBsu                     , "MultiWaveBsu"},
  {Sparkle::id,       10000L, &sparkleBlueBSU                   , "SparkleBlueBSU"},
  {SolidColor::id,        0L, &solidGreen                       , "SolidGreen"},
  {Sparkle::id,       10000L, &sparkleBluePretty                , "SparkleBluePretty"},
  {SolidColor::id,        0L, &solidBlue                        , "SolidBlue"},
  {Sparkle::id,       10000L, &sparkleBlueCrazy                 , "SparkleBlueCrazy"},
  {Blocks::id,        10000L, &classicXmasLights                , "ClassicXmasLights"},
  {Sparkle::id,       10000L, &sparkleRandom                    , "SparkleRandom"},
  {Rainbow::id,       10000L, &rainbowManicRight                , "RainbowManicRight"},
  {Rainbow::id,       10000L, &rainbowManicLeft                 , "RainbowManicLeft"},
  {Blocks::id,        10000L, &bsuBlock3                        , "BSU Block (3)"},
  {Glint::id,         10000L, &blueGlint10Sec                   , "BlueGlint10Sec"},
  {SolidColor::id,        0L, &solidPink                        , "Pink"},
  {SolidColor::id,        0L, &solidPurple                      , "Purple"},
  {SolidColor::id,        0L, &solidBlue                        , "Blue"},
  {SolidColor::id,        0L, &solidAqua                        , "Aqua"},
  {SolidColor::id,        0L, &solidGreen                       , "Green"},
  {SolidColor::id,        0L, &solidYellow                      , "Yellow"},
  {SolidColor::id,        0L, &solidBSUOrange                   , "BSU Orange"},
  {SolidColor::id,        0L, &solidRed                         , "Red"},
  {SolidColor::id,        0L, &solidWhite                       , "White"},
};

const PatternDef section1PatternDefs[] PROGMEM = {
  {MovingDot::id,         0L, &allOff                           , "Off"},
  {SplitRotation::id, 12000L, &splitRotationOrigRandom6RMedium  , "SplitRotOrig6RMedium"},
  {SplitRotation::id, 12000L, &splitRotationOrigRandom6LMedium  , "SplitRotOrig6LMedium"},
  {SplitRotation::id, 12000L, &splitRotationSymRandom6RVerySlow , "SplitRotSym6RVerySlow"},
  {SplitRotation::id, 12000L, &splitRotationSymRandom6LVerySlow , "SplitRotSym6LVerySlow"},
  {SplitRotation::id, 12000L, &splitRotationSymRandom6LMedium   , "SplitRotSym6LMedium"},
  {SplitRotation::id, 12000L, &splitRotationSymRandom6RMedium   , "SplitRotSym6RMedium"},
  {SplitRotation::id, 12000L, &splitRotationSymRandom6RFast     , "SplitRotSym6RFast"},
  {SplitRotation::id, 12000L, &splitRotationSymRandom6LFast     , "SplitRotSym6LFast"},
};

const PatternDef section2PatternDefs[] PROGMEM = {
  {MovingDot::id,         0L, &allOff                           , "Off"},
  {MultiWave::id,     12000L, &multiWave1Random                 , "MultiWave1Random"},
  {MultiWave::id,     12000L, &multiWaveBsu                     , "MultiWaveBsu"},
  {MultiWave::id,     12000L, &multiWaveXmas                    , "MultiWaveXmas"},
  {MultiWave::id,     12000L, &multiWaveXmas8                   , "MultiWaveXmas8"},
  {MultiWave::id,     12000L, &multiWave2Random                 , "MultiWave2Random"},
  {MultiWave::id,     12000L, &multiWave3                       , "MultiWave3"},
  {MultiWave::id,     12000L, &multiWave3SquareA                , "MultiWave3SquareA"},
  {MultiWave::id,     12000L, &multiWave3SquareB                , "MultiWave3SquareB"},
  {MultiWave::id,     12000L, &multiWave3ClusterFuck            , "MultiWave3ClusterFuck"},
};

const PatternDef section3PatternDefs[] PROGMEM = {
  {MovingDot::id,         0L, &allOff                           , "Off"},
  {MovingDot::id,      6000L, &movingDotRandomPong              , "MovingDotRandomPong"},
  {MovingDot::id,      9000L, &movingDotRandomLift              , "MovingDotRandomLift"},
  {MovingDot::id,     10000L, &movingDotRandomZipper            , "MovingDotRandomZipper"},
  {MovingDot::id,     10000L, &movingDotRandomPaint             , "MovingDotRandomPaint"},
  {MovingDot::id,     20000L, &movingDotRandomPaintSlow         , "MovingDotRandomPaintSlow"},
  {MovingDot::id,     10000L, &movingDotRandomShoot             , "MovingDotRandomShoot"},
};

const PatternDef section4PatternDefs[] PROGMEM = {
  {MovingDot::id,         0L, &allOff                           , "Off"},
  {Rainbow::id,       10000L, &rainbowSlowSoothing              , "RainbowSlowSoothing"},
  {Sparkle::id,       10000L, &sparkleBlueCrazy                 , "SparkleBlueCrazy"},
  {Blocks::id,        10000L, &classicXmasLights                , "ClassicXmasLights"},
  {Sparkle::id,       10000L, &sparkleRandom                    , "SparkleRandom"},
  {Rainbow::id,       10000L, &rainbowManicRight                , "RainbowManicRight"},
  {MultiWave::id,         0L, &multiWaveBsuSlow                 , "MultiWaveBsuSlow"},
  {SolidColor::id,        0L, &solidPink                        , "Pink"},
  {SolidColor::id,        0L, &solidPurple                      , "Purple"},
  {SolidColor::id,        0L, &solidBlue                        , "Blue"},
  {SolidColor::id,        0L, &solidAqua                        , "Aqua"},
  {SolidColor::id,        0L, &solidGreen                       , "Green"},
  {SolidColor::id,        0L, &solidYellow                      , "Yellow"},
  {SolidColor::id,        0L, &solidBSUOranger                  , "BSU Orange"},
  {SolidColor::id,        0L, &solidRed                         , "Red"},
  {SolidColor::id,        0L, &solidWhite                       , "White"}
};

