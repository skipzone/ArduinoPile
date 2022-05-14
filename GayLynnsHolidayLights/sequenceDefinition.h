/*******************************************************************
 *                                                                 *
 * Gay Lynn's Patio and Soffit Holiday Lights                      *
 *                                                                 *
 * Patten Sequence Definitions                                     *
 *                                                                 *
 * by Ross Butler  Oct. 2016                                       *
 *                                                                 *
 *******************************************************************/

#pragma once

#include "patternConfigurations.h"

using namespace pixelPattern;

const PatternDef houseSoffitPatternDefs[] PROGMEM = {
  {MovingDot::id,         0L, &allOff                           , "Off"},
  {MultiWave::id,         0L, &multiWaveXmas                    , "MW Xmas"},
  {MultiWave::id,         0L, &multiWaveXmas8                   , "MW Xmas 8"},
  {MultiWave::id,         0L, &multiWaveCandyCane               , "MW Candy Cane"},
  {Blocks::id,       600000L, &xmasClassic                      , "Xmas Classic"},
  {Blocks::id,       600000L, &xmasRedGreen                     , "Xmas Red Green"},
  {Blocks::id,       600000L, &xmasRed4Green4                   , "Xmas Red 4 Green 4"},
  {Blocks::id   ,    600000L, &xmasRedWhite                     , "Xmas Red White"},
  {Blocks::id   ,    600000L, &xmasGreenWhite                   , "Xmas Green White"},
  {Blocks::id   ,    600000L, &xmasBlueWhite                    , "Xmas Blue White"},
  {Blocks::id   ,    600000L, &xmasBlue3White                   , "Xmas Blue 3 White"},
  {Blocks::id   ,         0L, &xmasCandyStripe                  , "Xmas Candy Stripe"},
  {Glint::id,             0L, &blueGlint10Sec                   , "Blue Glint"},
  {Blocks::id,            0L, &bsuBlock3                        , "BSU Block (3)"},
  {Blocks::id,            0L, &americaFuckYeah                  , "Murica"},
  {Blocks::id,            0L, &ukraineBlock3                    , "Ukraine Block (3)"},
  {MultiWave::id,         0L, &multiWave2Random                 , "MW2 Random"},
  {Rainbow::id  ,         0L, &rainbowVerySlow                  , "Rainbow Slow"},
  {MultiWave::id,         0L, &multiWave1Random                 , "MW1 Random"},
  {MultiWave::id,         0L, &multiWave3SquareA                , "MW3 SquareA"},
  {MovingDot::id,         0L, &movingDotRandomZipper            , "Random Zipper"},
  {MultiWave::id,         0L, &multiWave3                       , "MW3"},
  {MovingDot::id,         0L, &movingDotRandomPaintSlow         , "Random Paint"},
  {Sparkle::id,           0L, &sparkleBluePretty                , "Pretty Sparkle"},
  {MultiWave::id,         0L, &multiWaveBsu                     , "MW BSU"},
  {MultiWave::id,         0L, &multiWave3ClusterFuck            , "MW3 Clusterfuck"},
  {Sparkle::id,           0L, &sparkleBlueBSU                   , "BSU Sparkle"},
  {Rainbow::id,           0L, &rainbowManicRight                , "Rainbow Manic"},
  {Sparkle::id,           0L, &sparkleBlueCrazy                 , "Crazy Sparkle"},
  {MultiWave::id,         0L, &multiWave3SquareB                , "MW3 SquareB"},
  {SplitRotation::id,     0L, &splitRotationSymRandom6RMedium   , "SR Sym6RMedium"},
  {SolidColor::id,        0L, &solidPink                        , "Pink"},
  {SolidColor::id,        0L, &solidPurple                      , "Purple"},
  {SolidColor::id,   600000L, &solidBlue                        , "Blue"},
  {SolidColor::id,        0L, &solidAqua                        , "Aqua"},
  {SolidColor::id,        0L, &solidGreen                       , "Green"},
  {SolidColor::id,        0L, &solidYellow                      , "Yellow"},
  {SolidColor::id,        0L, &solidBSUOrange                   , "Orange"},
  {SolidColor::id,        0L, &solidRed                         , "Red"},
// causes light string to lock up  {SolidColor::id,        0L, &solidWhite                       , "White"},
};

const PatternDef patioStripsPatternDefs[] PROGMEM = {
  {MovingDot::id,         0L, &allOff                           , "Off"},
  {Blocks::id,       600000L, &xmasRedGreenPatio                , "Xmas Red/Green"},
  {Blocks::id,       600000L, &xmasCandyStripePatio             , "Xmas Candy Stripe"},
  {Blocks::id,       600000L, &xmasBlue2WhitePatio              , "Xmas Blue/White"},
  {Rainbow::id,           0L, &rainbowSlowSoothing              , "Rainbow Slow"},
  {MultiWave::id,         0L, &multiWaveBsuSlow                 , "MW BSU"},
  {Blocks::id,            0L, &bsuOrangeBluePatio               , "BSU Orange/Blue"},
  {Blocks::id,            0L, &americaFuckYeahPatio             , "Murica"},
  {Blocks::id,            0L, &ukraineBlueYellowPatio           , "Ukraine"},
  {SolidColor::id,        0L, &solidPink                        , "Pink"},
  {SolidColor::id,        0L, &solidPurple                      , "Purple"},
  {SolidColor::id,        0L, &solidBlue                        , "Blue"},
  {SolidColor::id,        0L, &solidAqua                        , "Aqua"},
  {SolidColor::id,        0L, &solidGreen                       , "Green"},
  {SolidColor::id,        0L, &solidYellow                      , "Yellow"},
  {SolidColor::id,        0L, &solidBSUOranger                  , "BSU Orange"},
  {SolidColor::id,        0L, &solidRed                         , "Red"},
  {SolidColor::id,        0L, &solidAdjWhite                    , "White"}
};


/*
  {Sparkle::id,           0L, &sparkleRandom                    , "SparkleRandom"},
  {Rainbow::id,           0L, &rainbowManicLeft                 , "RainbowManicLeft"},
  {SplitRotation::id, 12000L, &splitRotationOrigRandom6RMedium  , "SplitRotOrig6RMedium"},
  {SplitRotation::id, 12000L, &splitRotationOrigRandom6LMedium  , "SplitRotOrig6LMedium"},
  {SplitRotation::id, 12000L, &splitRotationSymRandom6RVerySlow , "SplitRotSym6RVerySlow"},
  {SplitRotation::id, 12000L, &splitRotationSymRandom6LVerySlow , "SplitRotSym6LVerySlow"},
  {SplitRotation::id, 12000L, &splitRotationSymRandom6LMedium   , "SplitRotSym6LMedium"},
  {SplitRotation::id, 12000L, &splitRotationSymRandom6RMedium   , "SplitRotSym6RMedium"},
  {SplitRotation::id, 12000L, &splitRotationSymRandom6RFast     , "SplitRotSym6RFast"},
  {SplitRotation::id, 12000L, &splitRotationSymRandom6LFast     , "SplitRotSym6LFast"},
  {MovingDot::id,      6000L, &movingDotRandomPong              , "MovingDotRandomPong"},
  {MovingDot::id,      9000L, &movingDotRandomLift              , "MovingDotRandomLift"},
  {MovingDot::id,     10000L, &movingDotRandomPaint             , "MovingDotRandomPaint"},
  {MovingDot::id,     10000L, &movingDotRandomShoot             , "MovingDotRandomShoot"},
*/

