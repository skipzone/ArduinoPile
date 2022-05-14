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
#include "patternFrameworkTypes.h"
#include "sequenceDefinition.h"
#include "AutoShowSelector.h"

using namespace pixelPattern;


CRGB pixelArray[NUM_PHYSICAL_PIXELS];
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
//PatternSequence primaryPatternSequence(patternDefs, sizeof(patternDefs) / sizeof(PatternDef), &primaryPatternSelector);
PatternSequence* primaryPatternSequence;

PixelPatternController patternController;


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

  //Serial.begin(57600);
  //Serial.println(F("Starting"));

  //FastLED.addLeds<WS2812B, PIXEL_DATA_PIN, GRB>(pixelArray, NUM_PHYSICAL_PIXELS); // 60/144 pixels/m 5 V white strips (devel. strip, lampshade hat) 
  FastLED.addLeds<WS2812B, PIXEL_DATA_PIN, GRB>(pixelArray, NUM_PHYSICAL_PIXELS); // 60/144 pixels/m 5 V white strips (devel. strip, lampshade hat) 

//  AutoShowSelector primaryPatternSelector;
  primaryPatternSequence = new PatternSequence(patternDefs, sizeof(patternDefs) / sizeof(PatternDef), 0);

  patternController.addSequence(primaryPatternSequence, &primaryPixelSet);

  patternController.enableEeprom();
  patternController.enableStatusLed(ONBOARD_LED_PIN);
  patternController.enableModeButton(MODE_PUSHBUTTON_PIN, GND_PIN, PUSHBUTTON_DEBOUNCE_INTERVAL_MS);
  patternController.init();

  patternController.displayFreeMemory(&primaryPixelSet);
  delay(2000);
}


void loop()
{
    patternController.update();
}

