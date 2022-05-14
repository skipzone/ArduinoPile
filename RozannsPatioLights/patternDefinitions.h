/*******************************************************************
 *                                                                 *
 * Pattern Definitions for Rozann's Patio Lights                   *
 *                                                                 *
 * Patterns by Ross Butler  Nov. 2014 - Nov. 2015                  * 
 * Patterns by Joshua Anderson  Nov. 2015                          * 
 *                                                                 *
 *****************************************************************/

#ifndef __PATTERN_DEFINITIONS_H
#define __PATTERN_DEFINITIONS_H


// colors for black WS2811S 12V pixel strips purchased from HolidayCoro 2014 presale.
#define BSU_BLUE           (CRGB::HTMLColorCode) 0x0000FF
#define BSU_ORANGE         (CRGB::HTMLColorCode) 0xD03000  // not tested--probably needs more red
#define HUE_BSU_BLUE       (HSVHue) 160
#define HUE_BSU_ORANGE     (HSVHue) 16  // this is a good orange for the pixel strawberry strings


// ---------- multi-wave patterns ----------

// Each multiwave element consists of one boundedByPanel element and MAX_WAVEFORMS
// wave definition elements.  When less than MAX_WAVEFORMS are needed, set numWaves
// to zero in unused wave definitions.

// Wave definition elements:
//     randomHue       true if the waveform hue should be selected at random
//     posHue          hue of the positive half of the waveform (if randomHue is false)
//     negHue          hue of the negative half of the waveform (if randomHue is false)
//     numWaves        no. of complete waveforms in pattern; neg. for halfwave (neg. half black), 0 to disable wave 
//     waveformType    a waveform type constant (SQUARE_WAVE, TRIANGLE_WAVE, etc.)
//     directionDown   true if the waves should move toward pixel 0, false if they should move the other direction
//     delay           roughly, the number of milliseconds between each stepped movement of a wave (0-65535s)

const multiWaveParams_t multiWaveXmas PROGMEM = {
  true,
  { {false, HUE_RED      , HUE_GREEN     ,  1, CUBIC_EASING    , false,  60},
    {false, HUE_RED      , HUE_RED       ,  0, SINE_WAVE       , false,   0},
    {false, HUE_RED      , HUE_RED       ,  0, SINE_WAVE       , false,   0} } };

const multiWaveParams_t multiWaveXmas8 PROGMEM = {
  true,
  { {false, HUE_RED      , HUE_GREEN     ,  8, CUBIC_EASING    , false, 100},
    {false, HUE_RED      , HUE_RED       ,  0, SINE_WAVE       , false,   0},
    {false, HUE_RED      , HUE_RED       ,  0, SINE_WAVE       , false,   0} } };

const multiWaveParams_t multiWaveBsu PROGMEM = {
  true,
  { {false, HUE_BSU_BLUE , HUE_BSU_ORANGE,  1, CUBIC_EASING    , false,  60},
    {false, HUE_RED      , HUE_RED       ,  0, SINE_WAVE       , false,   0},
    {false, HUE_RED      , HUE_RED       ,  0, SINE_WAVE       , false,   0} } };

const multiWaveParams_t multiWave1Random PROGMEM = {
  true,
  { {true , HUE_RED      , HUE_RED       ,  3, QUADRATIC_EASING, false,  30},
    {false, HUE_RED      , HUE_RED       ,  0, QUADRATIC_EASING, false,   0},
    {false, HUE_RED      , HUE_RED       ,  0, QUADRATIC_EASING, false,   0} } };

const multiWaveParams_t multiWave2Random PROGMEM = { 
  false,
  { {true , HUE_RED      , HUE_RED       ,  1, QUADRATIC_EASING, false, 255},
    {false, HUE_RED      , HUE_RED       ,  5, QUADRATIC_EASING, false, 127},
    {false, HUE_RED      , HUE_RED       ,  0, QUADRATIC_EASING, false,   0} } };

const multiWaveParams_t multiWave3 PROGMEM = { 
  false,
  { {false, HUE_BLUE     , HUE_ORANGE    , -1, SINE_WAVE       , false,  30},
    {false, HUE_RED      , HUE_AQUA      , -2, SINE_WAVE       , true ,  40},
    {false, HUE_GREEN    , HUE_PINK      , -3, SINE_WAVE       , false,  50} } };

const multiWaveParams_t multiWave3SquareA PROGMEM = { 
  true,
  { {false, HUE_BLUE     , HUE_ORANGE    ,  1, SQUARE_WAVE     , true , 255},
    {false, HUE_RED      , HUE_AQUA      ,  2, SQUARE_WAVE     , true , 192},
    {false, HUE_GREEN    , HUE_PINK      ,  3, SQUARE_WAVE     , false, 128} } };

const multiWaveParams_t multiWave3SquareB PROGMEM = { 
  true,
  { {false, HUE_BLUE     , HUE_ORANGE    , -1, SQUARE_WAVE     , true ,  80},
    {false, HUE_RED      , HUE_AQUA      , -3, SQUARE_WAVE     , true ,  80},
    {false, HUE_GREEN    , HUE_PINK      , -5, SQUARE_WAVE     , false,  80} } };

const multiWaveParams_t multiWave3Cluster PROGMEM = {
  false,
  { {false, HUE_BLUE     , HUE_BLUE      ,  1, QUADRATIC_EASING, true , 64},
    {false, HUE_RED      , HUE_RED       ,  3, TRIANGLE_WAVE   , true , 128},
    {false, HUE_GREEN    , HUE_GREEN     ,  5, QUADRATIC_EASING, true , 256} } };

const multiWaveParams_t multiWave3ClusterJoshua PROGMEM = {
  false,
  { {false, HUE_BLUE     , HUE_BLUE      ,  1, QUADRATIC_EASING, true ,  60},
    {false, HUE_RED      , HUE_RED       ,  3, TRIANGLE_WAVE   , true ,  40},
    {false, HUE_GREEN    , HUE_GREEN     ,  5, QUADRATIC_EASING, true ,  20} } };


// ---------- sparkle patterns ----------

// Elements:
//     fgColorCode  color code of the sparkle color
//     bgColorCode  color code of the background color
//     density      number of foreground (sparkle) pixels on simultaneously (1 to MAX_SPARKLE_DENSITY)

const sparkleParams_t sparkleBluePretty PROGMEM = {CRGB::White        ,  CRGB::Blue       , MAX_SPARKLE_DENSITY};
const sparkleParams_t sparkleBlueJoshua PROGMEM = {CRGB::MidnightBlue ,  CRGB::DarkRed    , MAX_SPARKLE_DENSITY};
const sparkleParams_t sparkleBlueCrazy  PROGMEM = {CRGB::DarkOrange   ,  CRGB::Blue   , MAX_SPARKLE_DENSITY};
const sparkleParams_t sparkleBlueCrazy3  PROGMEM = {CRGB::DarkGoldenrod     ,  CRGB::DarkGreen   , MAX_SPARKLE_DENSITY};
const sparkleParams_t sparkleGreenPretty PROGMEM = {CRGB::White   ,  CRGB::Green    , 1};
const sparkleParams_t sparkleGreenCrazy  PROGMEM = {CRGB::White   ,  CRGB::Green    , MAX_SPARKLE_DENSITY};
const sparkleParams_t sparkleGreenCrazy3  PROGMEM = {CRGB::Blue     ,  CRGB::Red   , MAX_SPARKLE_DENSITY};
const sparkleParams_t sparkleRedCrazy1  PROGMEM = {CRGB::Red     ,  CRGB::Cyan    , 3};
const sparkleParams_t sparkleRedCrazy2  PROGMEM = {CRGB::Red     ,  CRGB::Blue    , 3};
const sparkleParams_t sparkleRedCrazy3  PROGMEM = {CRGB::Red     ,  CRGB::Black   , MAX_SPARKLE_DENSITY};
const sparkleParams_t sparkleRandom     PROGMEM = {CRGB::Black   ,  CRGB::Black   , 2};


// ---------- moving-dot patterns ----------

// Elements:
//     fgColorCode    dot color
//     bgColorCode    background color
//     randomFgColor  true if the foreground color should be selected at random
//     randomBgColor  true if the background color should be selected at random
//     changeFgColor  true if the foreground color should be re-selected at random for each movement cycle
//     zipperBg;      zipper effect:  set the bg color when moving away from pixel 0 then clear it when moving toward pixel 0
//     bidirectional  true if the dot should bounce back and forth
//     delay0         approx. ms delay between dot steps when moving away from pixel 0 (0-255)
//     delay1         approx. ms delay between dot steps when moving toward pixel 0 (0-255)
//     holdTimeFarMs  number of ms to hold the dot at the far end before it moves back toward pixel 0 (0-255)
//     holdTimeNearMs number of ms to hold the dot at the near end before it moves toward the last pixel

//                                                           fg               bg           randFg randBg chngFg zipper  biDir   d0   d1 hFar hNear
const movingDotParams_t movingDotJoshua1          PROGMEM = {CRGB::Chocolate, CRGB::Indigo, false, false, false, false, false,  20,  20,  0,     0};
const movingDotParams_t movingDotRandomPong       PROGMEM = {CRGB::Black    , CRGB::Black ,  true, false, false, false, true ,   1,   1,  0,     0};
const movingDotParams_t movingDotRandomLift       PROGMEM = {CRGB::Black    , CRGB::Black ,  true, false, false, false, true ,  30,   1, 500, 1500};
const movingDotParams_t movingDotRandomZipper     PROGMEM = {CRGB::Black    , CRGB::Black ,  true,  true,  true,  true, true ,  20,   1, 200, 1000};
const movingDotParams_t movingDotRandomPaint      PROGMEM = {CRGB::Black    , CRGB::Black ,  true,  true,  true,  true, false,  10,   1,   0,    0};
const movingDotParams_t movingDotRandomPaintSlow  PROGMEM = {CRGB::Black    , CRGB::Black ,  true,  true,  true,  true, false, 100,   1,   0,    0};
const movingDotParams_t movingDotRandomShoot      PROGMEM = {CRGB::Black    , CRGB::Black , false, false, false, false, false,   1,   1,   0,    0};


// ---------- split-rotation patterns ----------

// Elements:
//     randomHue      true if the hues should be selected at random
//     fgHue          foreground (moving pixels) hue
//     bgHue          background hue
//     fgInterval     number of background pixels minus one between each moving pixel
//     directionDown  true to rotate one way, false to rotate the other way
//     delay          approx. ms delay between rotation steps (0-255)

const splitRotationParams_t splitRotationRed8RFast      PROGMEM = {false, HUE_RED           , HUE_AQUA          , 8, false,  30};
const splitRotationParams_t splitRotationRandom4RSlow   PROGMEM = {true , HUE_RED           , HUE_RED           , 4, false, 120};
const splitRotationParams_t splitRotationRandom4LSlow   PROGMEM = {true , HUE_RED           , HUE_RED           , 4, true , 120};
const splitRotationParams_t splitRotationRandom8RSlow   PROGMEM = {true , HUE_RED           , HUE_RED           , 8, false, 120};
const splitRotationParams_t splitRotationRandom8LSlow   PROGMEM = {true , HUE_RED           , HUE_RED           , 8, true , 120};
const splitRotationParams_t splitRotationRandom8RMedium PROGMEM = {true , HUE_RED           , HUE_RED           , 8, false,  60};
const splitRotationParams_t splitRotationRandom8LMedium PROGMEM = {true , HUE_RED           , HUE_RED           , 8, true ,  60};
const splitRotationParams_t splitRotationRandom8RFast   PROGMEM = {false, HUE_ORANGE        , HUE_PURPLE        , 8, false,  30};
const splitRotationParams_t splitRotationRandom8LFast   PROGMEM = {true , HUE_RED           , HUE_RED           , 8, true ,  30};
const splitRotationParams_t splitRotationRandom8RDizzy  PROGMEM = {true , HUE_RED           , HUE_RED           , 8, false,  15};
const splitRotationParams_t splitRotationRandom8LDizzy  PROGMEM = {true , HUE_RED           , HUE_RED           , 8, true ,  15};


// ---------- rainbow patterns ----------

// Elements:
//     directionDown  true to rotate one way, false to rotate the other way
//     delay          approx. ms delay between rotation steps (0-65535)

const rainbowParams_t rainbowVerySlow     PROGMEM = {false,  300};
const rainbowParams_t rainbowSlowSoothing PROGMEM = {false,  100};
const rainbowParams_t rainbowRightJoshua  PROGMEM = {false,  100};
const rainbowParams_t rainbowManicLeft    PROGMEM = {true  ,   1};
const rainbowParams_t rainbowManicRight   PROGMEM = {false ,   1};


// ---------- xmas lights patterns ----------
const xmasLightsParams_t xmasLightsClassic  PROGMEM = {6, {HUE_RED, HUE_GREEN, HUE_BSU_ORANGE, HUE_PURPLE, HUE_ORANGE, HUE_BLUE, HUE_RED, HUE_RED} };
const xmasLightsParams_t xmasLightsRedGreen  PROGMEM = {2, {HUE_RED, HUE_GREEN, HUE_PURPLE, HUE_YELLOW, HUE_BLUE, HUE_ORANGE, HUE_RED, HUE_RED} };
const xmasLightsParams_t xmasLightsRed4Green4  PROGMEM = {6, {HUE_RED, HUE_RED, HUE_RED, HUE_RED, HUE_GREEN, HUE_GREEN, HUE_GREEN, HUE_GREEN} };


// ---------- solid color patterns ----------

// Elements:
//     startHue    starting hue
//     endHue      end hue
//     saturation  the S in HSV
//     delay       approx. ms delay between rotation steps (0-255)

const solidColorParams_t solidBlues7Sec      PROGMEM = {HUE_BLUE      , (HSVHue) 175  , 255, 149};
const solidColorParams_t solidPink           PROGMEM = {HUE_PINK      , HUE_PINK      , 255, 255};
const solidColorParams_t solidPurple         PROGMEM = {HUE_PURPLE    , HUE_PURPLE    , 255, 255};
const solidColorParams_t solidBlue           PROGMEM = {HUE_BLUE      , HUE_BLUE      , 255, 255};
const solidColorParams_t solidAqua           PROGMEM = {HUE_AQUA      , HUE_AQUA      , 255, 255};
const solidColorParams_t solidGreen          PROGMEM = {HUE_GREEN     , HUE_GREEN     , 255, 255};
const solidColorParams_t solidYellow         PROGMEM = {HUE_YELLOW    , HUE_YELLOW    , 255, 255};
const solidColorParams_t solidOrange         PROGMEM = {HUE_ORANGE    , HUE_ORANGE    , 255, 255};
const solidColorParams_t solidBSUOrange      PROGMEM = {HUE_BSU_ORANGE, HUE_BSU_ORANGE, 255, 255};
const solidColorParams_t solidRed            PROGMEM = {HUE_RED       , HUE_RED       , 255, 255};
const solidColorParams_t solidWhite          PROGMEM = {HUE_YELLOW    , HUE_YELLOW    ,   0, 255};
const solidColorParams_t solidLightPink      PROGMEM = {HUE_PINK      , HUE_PINK      , 112, 255};
const solidColorParams_t solidLightPurple    PROGMEM = {HUE_PURPLE    , HUE_PURPLE    , 144, 255};
const solidColorParams_t solidLightBlue      PROGMEM = {HUE_BLUE      , HUE_BLUE      , 112, 255};
const solidColorParams_t solidLightAqua      PROGMEM = {HUE_AQUA      , HUE_AQUA      , 112, 255};
const solidColorParams_t solidLightGreen     PROGMEM = {HUE_GREEN     , HUE_GREEN     , 112, 255};
const solidColorParams_t solidLightYellow    PROGMEM = {HUE_YELLOW    , HUE_YELLOW    , 144, 255};
const solidColorParams_t solidLightOrange    PROGMEM = {HUE_ORANGE    , HUE_ORANGE    , 160, 255};
const solidColorParams_t solidLightBSUOrange PROGMEM = {HUE_BSU_ORANGE, HUE_BSU_ORANGE, 160, 255};
const solidColorParams_t solidLightRed       PROGMEM = {HUE_RED       , HUE_RED       , 144, 255};
const solidColorParams_t solidWarmWhite      PROGMEM = {HUE_YELLOW    , HUE_YELLOW    , 128, 255};


// ---------- shine patterns ----------

// Elements:
//     bgHue          starting hue
//     shineInterval  interval between shines (ms, 0-65535)
//     delay          approx. ms delay between shine steps (0-255)

const shineParams_t blueShine10Sec  PROGMEM = {HUE_BLUE, 7000, 1};


// ---------- Red White and Blue ----------

// Elements:
//     delay                  approx. ms delay between rotation steps (0-255)
//     numPixelsPerSegment;   number of pixels per color segment (1-255)

const rwabParams_t americaYeahJoshua PROGMEM = {100, 50};
const rwabParams_t americaFuckYeah   PROGMEM = {150,  9};


#endif  // #ifndef __PATTERN_DEFINITIONS_H

