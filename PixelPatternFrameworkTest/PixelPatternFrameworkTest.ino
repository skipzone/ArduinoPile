/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Test Sketch                                                     *
 *                                                                 *
 * by Ross Butler  Dec. 2015                                       *
 *                                                                 *
 *****************************************************************/

#include "implementationConfig.h"
#ifdef ENABLE_DMX
  #include <DmxSimple.h>
#endif
#include "FastLED.h"
#include "PixelPatternController.h"
#include "PatternSequence.h"
#include "pixelPatternFrameworkTypes.h"
#include "sequenceDefinition.h"
#include "AutoShowSelector.h"
#include "PushbuttonSelector.h"

using namespace pixelPattern;

PixelPatternController patternController;

//CRGB pixelArray[NUM_PHYSICAL_PIXELS];
CRGB pixelArray[TEST_STRIP_NUM_PHYSICAL_PIXELS];
PixelSet primaryPixelSet(
    pixelArray,
    NUM_PHYSICAL_PIXELS,
    NUM_SKIP_PIXELS,
    NUM_NONSYMMETRICAL_PIXELS,
    NUM_PANELS,
    NUM_PANEL_PIXELS,
    FG_INTENSITY_SCALE_FACTOR,
    BG_INTENSITY_SCALE_FACTOR); 

//AutoShowSelector primaryPatternSelector;
PushbuttonSelector primaryPatternSelector(MODE_PUSHBUTTON_PIN);
PatternSequence primaryPatternSequence(patternDefs, sizeof(patternDefs) / sizeof(PatternDef), &primaryPatternSelector);

#ifdef ENABLE_AMBIENCE_LIGHTS
  //CRGB ambiencePixelArray[AMBIENCE_NUM_PHYSICAL_PIXELS];
  PixelSet ambiencePixelSet(
      //ambiencePixelArray,
      pixelArray + (TEST_STRIP_NUM_PHYSICAL_PIXELS - AMBIENCE_NUM_PHYSICAL_PIXELS),
      AMBIENCE_NUM_PHYSICAL_PIXELS,
      AMBIENCE_NUM_SKIP_PIXELS,
      AMBIENCE_NUM_NONSYMMETRICAL_PIXELS,
      AMBIENCE_NUM_PANELS,
      AMBIENCE_NUM_PANEL_PIXELS,
      AMBIENCE_FG_INTENSITY_SCALE_FACTOR,
      AMBIENCE_BG_INTENSITY_SCALE_FACTOR); 

  AutoShowSelector ambiencePatternSelector;
  PatternSequence ambiencePatternSequence(ambiencePatternDefs, sizeof(ambiencePatternDefs) / sizeof(PatternDef), &ambiencePatternSelector);
#endif


/***********************
 * Setup and Main Loop *
 ***********************/

void setup()
{
  // Let everything settle down after a rough power-up.
  delay(100);

  // Analog input 0 should be disconnected, making it a good source of
  // random noise with which we can seed the random number generator.
  randomSeed(analogRead(0));  

  pinMode(MODE_PUSHBUTTON_PIN, INPUT_PULLUP);

  pinMode(GND_PIN, OUTPUT);
  digitalWrite(GND_PIN, LOW);

  //Serial.begin(57600);
  //Serial.println(F("Starting"));

  //FastLED.addLeds<WS2812B, PIXEL_DATA_PIN, GRB>(pixelArray, NUM_PHYSICAL_PIXELS); // 60/144 pixels/m 5 V white strips (devel. strip, lampshade hat) 
  FastLED.addLeds<WS2812B, PIXEL_DATA_PIN, GRB>(pixelArray, TEST_STRIP_NUM_PHYSICAL_PIXELS); // 60/144 pixels/m 5 V white strips (devel. strip, lampshade hat) 

  patternController.addSequence(&primaryPatternSequence, &primaryPixelSet);

#ifdef ENABLE_AMBIENCE_LIGHTS
#ifdef ENABLE_DMX
  FastLED.addLeds<DMXSIMPLE, DMX_DATA_PIN, RGB>(dmxRgbArray, NUM_DMX_RGB_CHANNELS);
#endif
  patternController.addSequence(&ambiencePatternSequence, &ambiencePixelSet);
#endif

  patternController.enableStatusLed(ONBOARD_LED_PIN);
  patternController.init();
}


void loop()
{
  primaryPatternSelector.poll();
  patternController.update();
}

