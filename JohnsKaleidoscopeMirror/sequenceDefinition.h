/*******************************************************************
 *                                                                 *
 * Pixel Pattern Library ESP8266 Web Page Test                     *
 * customized for Mzfit's Kaleidoscope Mirror                      *
 *                                                                 *
 * Patten Sequence Definitions                                     *
 *                                                                 *
 * by Ross Butler  Nov. 2016                                       *
 *                                                                 *
 *******************************************************************/

#pragma once

#include "patternDefinitions.h"

using namespace pixelPattern;

const PatternDef section0PatternDefs[] PROGMEM = {

  {MovingDot::id,         0L, &allOff                           , "Off"},

  // introductory twisting kaleidoscope
  {MultiWave::id,     20000L, &multiWave3SquareA                , "MultiWave3SquareA"},

  // Vikings tribute (with shitty purple, unfortunately)
  {Sparkle::id      ,  2000L, &sparkleVikings1                  , "SparkleVikings1"},
  {Sparkle::id      ,  2000L, &sparkleVikings2                  , "SparkleVikings2"},
  {Sparkle::id      ,  2000L, &sparkleVikings1                  , ""},
  {Sparkle::id      ,  2000L, &sparkleVikings2                  , ""},
  {SplitRotation::id,  5000L, &splitRotationVikings8Medium      , "SplitRotVikings8Med"},

  // BSU tribute
  {MultiWave::id,     10000L, &multiWaveBsu                     , "MultiWaveBsu"},

  // pretty white-on-blue sparkle
  {Sparkle::id,       15000L, &sparkleBluePretty                , "SparkleBluePretty"},

  // a random-color single waveform
  {MultiWave::id,     10000L, &multiWave1Random                 , "MultiWave1Random"},

  // John's favorite
  {MovingDot::id,     15000L, &movingDotRedRecoil               , "MovingDotRedRecoil"},

  // colors splashing into each other
  {MultiWave::id,     20000L, &multiWave3                       , "MultiWave3"},

  // brain ramp-up
  {SplitRotation::id,  5000L, &splitRotationRandom8RSlow        , "SplitRotRand8RSlow"},
  {SplitRotation::id,  5000L, &splitRotationRandom8LSlow        , "SplitRotRand8LSlow"},
  {SplitRotation::id,  5000L, &splitRotationRandom8RSlow        , ""},
  {SplitRotation::id,  5000L, &splitRotationRandom8LSlow        , ""},

  // brain spin
  {Rainbow::id,       10000L, &rainbowSpinRight              , "RainbowSpinRight"},

  // a hint of trippy things to come
  {MovingDot::id,     10000L, &movingDotRedFast                 , "MovingDotRedFast"},

  // firework-intensity white-on-blue sparkle
  {Sparkle::id,        7000L, &sparkleBlueCrazy                 , "SparkleBlueCrazy"},

  // back it off a little, let 'em catch up with some alternating-direction rotations
  {SplitRotation::id,  5000L, &splitRotationRandom4RSlow        , "SplitRotRand4RSlow"},
  {SplitRotation::id,  5000L, &splitRotationRandom4LSlow        , "SplitRotRand4LSlow"},
  {SplitRotation::id,  5000L, &splitRotationRandom4RSlow        , ""},
  {SplitRotation::id,  5000L, &splitRotationRandom4LSlow        , ""},

  {MovingDot::id,     10000L, &movingDotRandomPaintMed          , "MovingDotPaint"},

  // an attempt at a blocky kaleidoscope pattern
  {MultiWave::id,     15000L, &multiWave3SquareB                , "MultiWave3SquareB"},

  // rotating patterns look really cool in the infinite reflections... and straight up, too, if your brain is right
  {SplitRotation::id,  5000L, &splitRotationRandom8RMedium      , "SplitRotRand8RMed"},
  {SplitRotation::id,  5000L, &splitRotationRandom8LMedium      , "SplitRotRand8LMed"},
  {SplitRotation::id,  5000L, &splitRotationRandom8RMedium      , ""},
  {SplitRotation::id,  5000L, &splitRotationRandom8LMedium      , ""},

  {MovingDot::id,     10000L, &movingDotRandom                  , "MovingDotRandom"},

  // unicorn diarrhea
  {Rainbow::id,        8000L, &rainbowRollingRight              , "RainbowRollingRight"},
  {Rainbow::id,        7000L, &rainbowRollingLeft               , "RainbowRollingLeft"},
  {Rainbow::id,        6000L, &rainbowRollingRight              , ""},
  {Rainbow::id,        5000L, &rainbowRollingLeft               , ""},
  {Rainbow::id,        4000L, &rainbowRollingRight              , ""},
  {Rainbow::id,        3000L, &rainbowRollingLeft               , ""},
  {Rainbow::id,        2000L, &rainbowRollingRight              , ""},
  {Rainbow::id,        2000L, &rainbowRollingLeft               , ""},
  {Rainbow::id,        1000L, &rainbowRollingRight              , ""},
  {Rainbow::id,        1000L, &rainbowRollingLeft               , ""},
  {Rainbow::id,        1000L, &rainbowRollingRight              , ""},
  {Rainbow::id,        1000L, &rainbowRollingLeft               , ""},
  {Rainbow::id,        1000L, &rainbowRollingRight              , ""},
  {Rainbow::id,        1000L, &rainbowRollingLeft               , ""},

  // some more sparkly shit
  {Sparkle::id,        4000L, &sparkleRandom                    , "SparkleRandom"},
  {Sparkle::id,        3000L, &sparkleRandom                    , ""},
  {Sparkle::id,        3000L, &sparkleRandom                    , ""},
  {Sparkle::id,        2000L, &sparkleRandom                    , ""},
  {Sparkle::id,        1000L, &sparkleRandom                    , ""},
  {Sparkle::id,        1000L, &sparkleRandom                    , ""},
  {Sparkle::id,        1000L, &sparkleRandom                    , ""},
  {Sparkle::id,        1000L, &sparkleRandom                    , ""},
  {Sparkle::id,        1000L, &sparkleRandom                    , ""},

  {MovingDot::id,     10000L, &movingDotRandomFast              , "MovingDotRandomFast"},

  // chill it back a little
  {MultiWave::id,     30000L, &multiWave2Random                 , "MultiWave2Random"},

  // forgot what the hell this was supposed to be
  {SplitRotation::id, 10000L, &splitRotationRandom8RFast        , "SplitRotRand8RFast"},
  {SplitRotation::id, 10000L, &splitRotationRandom8LFast        , "SplitRotRand8LFast"},
  {SplitRotation::id,  5000L, &splitRotationRandom8RDizzy       , "SplitRotRand8RDizzy"},
  {SplitRotation::id,  5000L, &splitRotationRandom8LDizzy       , "SplitRotRand8LDizzy"},

  {MovingDot::id,     10000L, &movingDotRandomPaint             , "MovingDotRandomPaintFast"},

  // ramp up with some color tripping
  {MultiWave::id,     20000L, &multiWave3ClusterFuck            , "MultiWave3ClusterFuck"},

  // frenetic mindfuck
  {SplitRotation::id,  4000L, &splitRotationRed8RFast           , "SplitRotRed8RFast"},
  {Sparkle::id,        3000L, &sparkleRedCrazy1                 , ""},
  {Sparkle::id,        3000L, &sparkleRedCrazy2                 , ""},
  {Sparkle::id,        3000L, &sparkleRedCrazy3                 , ""},
  {MovingDot::id,      2000L, &movingDotRedDizzy                , "MovingDotRedDizzy"},
  {Sparkle::id,        3000L, &sparkleRedCrazy1                 , "SparkleRedCrazy1"},
  {Sparkle::id,        4000L, &sparkleRedCrazy2                 , "SparkleRedCrazy2"},
  {Sparkle::id,        5000L, &sparkleRedCrazy3                 , "SparkleRedCrazy3"},
  {MovingDot::id,      5000L, &movingDotRedEpilepsy             , "MovingDotRedEpilepsy"},

  // soothing coming-down rainbow
  {Rainbow::id,       15000L, &rainbowForComingDown             , "RainbowForComingDown"},

};


/*

  {Sparkle::id,       10000L, &sparkleBlueBSU                   , "SparkleBlueBSU"},
  {SolidColor::id,        0L, &solidGreen                       , "SolidGreen"},
  {SolidColor::id,        0L, &solidBlue                        , "SolidBlue"},
  {Blocks::id,        10000L, &classicXmasLights                , "ClassicXmasLights"},
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

  {SplitRotation::id, 12000L, &splitRotationOrigRandom6RMedium  , "SplitRotOrig6RMedium"},
  {SplitRotation::id, 12000L, &splitRotationOrigRandom6LMedium  , "SplitRotOrig6LMedium"},
  {SplitRotation::id, 12000L, &splitRotationSymRandom6RVerySlow , "SplitRotSym6RVerySlow"},
  {SplitRotation::id, 12000L, &splitRotationSymRandom6LVerySlow , "SplitRotSym6LVerySlow"},
  {SplitRotation::id, 12000L, &splitRotationSymRandom6LMedium   , "SplitRotSym6LMedium"},
  {SplitRotation::id, 12000L, &splitRotationSymRandom6RMedium   , "SplitRotSym6RMedium"},
  {SplitRotation::id, 12000L, &splitRotationSymRandom6RFast     , "SplitRotSym6RFast"},
  {SplitRotation::id, 12000L, &splitRotationSymRandom6LFast     , "SplitRotSym6LFast"},

  {MultiWave::id,     12000L, &multiWaveBsu                     , "MultiWaveBsu"},
  {MultiWave::id,     12000L, &multiWaveXmas                    , "MultiWaveXmas"},
  {MultiWave::id,     12000L, &multiWaveXmas8                   , "MultiWaveXmas8"},

  {MovingDot::id,      6000L, &movingDotRandomPong              , "MovingDotRandomPong"},
  {MovingDot::id,      9000L, &movingDotRandomLift              , "MovingDotRandomLift"},
  {MovingDot::id,     10000L, &movingDotRandomZipper            , "MovingDotRandomZipper"},
  {MovingDot::id,     10000L, &movingDotRandomPaint             , "MovingDotRandomPaint"},
  {MovingDot::id,     20000L, &movingDotRandomPaintSlow         , "MovingDotRandomPaintSlow"},
  {MovingDot::id,     10000L, &movingDotRandomShoot             , "MovingDotRandomShoot"},

  {MultiWave::id,         0L, &multiWaveBsuSlow                 , "MultiWaveBsuSlow"},

*/

