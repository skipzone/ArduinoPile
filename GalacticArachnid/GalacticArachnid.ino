/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Test Sketch                                                     *
 *                                                                 *
 * by Ross Butler  Dec. 2015                                       *
 *                                                                 *
 *****************************************************************/

#include "FastLED.h"
#include "PixelPatternController.h"
#include "PatternSequence.h"
#include "pixelPatternFrameworkTypes.h"
#include "sequenceDefinition.h"
#include "ExternalControlSelector.h"
#include <Wire.h>


using namespace pixelPattern;

#define LEGS_PIXEL_DATA_PIN 2
#define BODY_PIXEL_DATA_PIN 3
#define HEAD_PIXEL_DATA_PIN 4
#define LARGE_EYES_PIXEL_DATA_PIN 5
#define SMALL_EYES_PIXEL_DATA_PIN 6
#define EXTERNAL_CONTROL_TX_EN_PIN 10
#define ONBOARD_LED_PIN 13

#define LEGS_NUM_PIXELS 126
#define BODY_NUM_PIXELS 50
#define HEAD_NUM_PIXELS 25
#define LARGE_EYES_NUM_PIXELS 92  // 23 per eye
#define LARGE_EYES_NUM_PANELS 4
#define SMALL_EYES_NUM_PIXELS 72  // 18 per eye
#define SMALL_EYES_NUM_PANELS 4

CRGB legsPixelArray[LEGS_NUM_PIXELS];
CRGB bodyPixelArray[BODY_NUM_PIXELS];
CRGB headPixelArray[HEAD_NUM_PIXELS];
CRGB largeEyesPixelArray[LARGE_EYES_NUM_PIXELS];
CRGB smallEyesPixelArray[SMALL_EYES_NUM_PIXELS];

PixelSet legsPixelSet(
    legsPixelArray,
    LEGS_NUM_PIXELS,
    0,    // skip pixels
    0,    // non-symmetrical pixels
    1,    // number of panels
    LEGS_NUM_PIXELS,  // number of pixels per panel (should be symmetrical / panels)
    255,  // foreground intensity scale factor
    255   // background intensity scale factor 
);

PixelSet bodyPixelSet(
    bodyPixelArray,
    BODY_NUM_PIXELS,
    0,    // skip pixels
    0,    // non-symmetrical pixels
    1,    // number of panels
    BODY_NUM_PIXELS,   // number of pixels per panel (should be symmetrical / panels)
    255,  // foreground intensity scale factor
    255   // background intensity scale factor 
);

PixelSet headPixelSet(
    headPixelArray,
    HEAD_NUM_PIXELS,
    0,    // skip pixels
    0,    // non-symmetrical pixels
    1,    // number of panels
    HEAD_NUM_PIXELS,   // number of pixels per panel (should be symmetrical / panels)
    255,  // foreground intensity scale factor
    255   // background intensity scale factor 
);

PixelSet largeEyesPixelSet(
    largeEyesPixelArray,
    LARGE_EYES_NUM_PIXELS,   // number of leg pixels
    0,    // skip pixels
    0,    // non-symmetrical pixels
    LARGE_EYES_NUM_PANELS,    // number of panels
    LARGE_EYES_NUM_PIXELS / LARGE_EYES_NUM_PANELS, // number of pixels per panel (should be symmetrical / panels)
    255,  // foreground intensity scale factor
    255   // background intensity scale factor 
);

PixelSet smallEyesPixelSet(
    smallEyesPixelArray,
    SMALL_EYES_NUM_PIXELS,   // number of leg pixels
    0,    // skip pixels
    0,    // non-symmetrical pixels
    SMALL_EYES_NUM_PANELS,    // number of panels
    SMALL_EYES_NUM_PIXELS / SMALL_EYES_NUM_PANELS,    // number of pixels per panel (should be symmetrical / panels)
    255,  // foreground intensity scale factor
    255   // background intensity scale factor 
);

PixelPatternController patternController;

PatternSequence* legsPatternSequence;
PatternSequence* bodyPatternSequence;
PatternSequence* headPatternSequence;
PatternSequence* largeEyesPatternSequence;
PatternSequence* smallEyesPatternSequence;

ExternalControlSelector legsPatternSelector;
ExternalControlSelector bodyPatternSelector;
ExternalControlSelector headPatternSelector;
ExternalControlSelector largeEyesPatternSelector;
ExternalControlSelector smallEyesPatternSelector;


void i2cReceiveEvent(int numRxBytes) {

  static uint32_t rxTimeoutMs;

  if (numRxBytes == 0) {
    return;
  }

  //Serial.println(patternController.freeRam());

//  if (numRxBytes != 3) {
//    //Serial.print(numRxBytes);
//    //Serial.println(" rx");
//    return;
//  }

  if (numRxBytes < 3) {
    //Serial.println("partial rx");
    //Serial.println(numRxBytes);
    uint32_t now = millis();
    if (rxTimeoutMs != 0  && now >= rxTimeoutMs) {
      //Serial.println("timeout");
      for (uint8_t i = 0; i < numRxBytes; ++i) {
        uint8_t rxByte = Wire.read();
        //Serial.println(rxByte);
      }
      rxTimeoutMs = 0;
      return;
    }
    rxTimeoutMs = now + 100L;
  }
  
  uint8_t selectedLegPatternNum = Wire.read();
  uint8_t selectedBodyPatternNum = Wire.read();
  uint8_t selectedHeadPatternNum = selectedBodyPatternNum;  //Wire.read();
  uint8_t selectedEyePatternNum = Wire.read();

  //Serial.print(selectedLegPatternNum);
  //Serial.print(",");
  //Serial.print(selectedBodyPatternNum);
  //Serial.print(",");
  //Serial.print(selectedHeadPatternNum);
  //Serial.print(",");
  //Serial.println(selectedEyePatternNum);

  if (selectedLegPatternNum != 254) {
    legsPatternSelector.setPatternNum(selectedLegPatternNum);
  }

  if (selectedBodyPatternNum != 254) {
    bodyPatternSelector.setPatternNum(selectedBodyPatternNum);
  }

  if (selectedHeadPatternNum != 254) {
    headPatternSelector.setPatternNum(selectedHeadPatternNum);
  }

  if (selectedEyePatternNum != 254) {
    largeEyesPatternSelector.setPatternNum(selectedEyePatternNum);
    smallEyesPatternSelector.setPatternNum(selectedEyePatternNum);
  }

//  return;

//  // Didn't get the expected number of bytes so discard everything.
//  //Serial.print("Expected 3 bytes but received ");
//  //Serial.println(numRxBytes);
//  for (uint8_t i = 0; i < numRxBytes; ++i) {
//    uint8_t rxByte = Wire.read();
//    Serial.println(rxByte);
//  }
}


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

  pinMode(EXTERNAL_CONTROL_TX_EN_PIN, OUTPUT);
  digitalWrite(EXTERNAL_CONTROL_TX_EN_PIN, 0);

  FastLED.addLeds<WS2812B, LEGS_PIXEL_DATA_PIN, GRB>(legsPixelArray, LEGS_NUM_PIXELS); // 60/144 pixels/m 5 V white strips (devel. strip, lampshade hat); Joule; spider 12V/5V strips
  FastLED.addLeds<WS2812B, BODY_PIXEL_DATA_PIN, BRG>(bodyPixelArray, BODY_NUM_PIXELS); // 10 pixels/m 12 V white strips from HolidayCoro 11/2015
  FastLED.addLeds<WS2812B, HEAD_PIXEL_DATA_PIN, BRG>(headPixelArray, HEAD_NUM_PIXELS); // 10 pixels/m 12 V white strips from HolidayCoro 11/2015
  FastLED.addLeds<WS2812B, LARGE_EYES_PIXEL_DATA_PIN, GRB>(largeEyesPixelArray, LARGE_EYES_NUM_PIXELS); // 60/144 pixels/m 5 V white strips (devel. strip, lampshade hat); Joule; spider 12V/5V strips
  FastLED.addLeds<WS2812B, SMALL_EYES_PIXEL_DATA_PIN, GRB>(smallEyesPixelArray, SMALL_EYES_NUM_PIXELS); // 60/144 pixels/m 5 V white strips (devel. strip, lampshade hat); Joule; spider 12V/5V strips

  legsPatternSequence = new PatternSequence(legsPatternDefs, sizeof(legsPatternDefs) / sizeof(PatternDef), &legsPatternSelector);
  bodyPatternSequence = new PatternSequence(bodyPatternDefs, sizeof(bodyPatternDefs) / sizeof(PatternDef), &bodyPatternSelector);
  headPatternSequence = new PatternSequence(headPatternDefs, sizeof(headPatternDefs) / sizeof(PatternDef), &headPatternSelector);
  largeEyesPatternSequence = new PatternSequence(largeEyesPatternDefs, sizeof(largeEyesPatternDefs) / sizeof(PatternDef), &largeEyesPatternSelector);
  smallEyesPatternSequence = new PatternSequence(smallEyesPatternDefs, sizeof(smallEyesPatternDefs) / sizeof(PatternDef), &smallEyesPatternSelector);

  patternController.addPatternSequence(legsPatternSequence, &legsPixelSet);
  patternController.addPatternSequence(bodyPatternSequence, &bodyPixelSet);
  patternController.addPatternSequence(headPatternSequence, &headPixelSet);
  patternController.addPatternSequence(largeEyesPatternSequence, &largeEyesPixelSet);
  patternController.addPatternSequence(smallEyesPatternSequence, &smallEyesPixelSet);

  patternController.enableStatusLed(ONBOARD_LED_PIN);
  patternController.init();

  legsPatternSelector.setPatternNum(255);
  bodyPatternSelector.setPatternNum(255);
  headPatternSelector.setPatternNum(255);
  largeEyesPatternSelector.setPatternNum(255);
  smallEyesPatternSelector.setPatternNum(255);

  //Serial.begin(9600);
  //Serial.println("Starting");

  Wire.begin(42);                     // join i2c bus with address 42
  Wire.onReceive(i2cReceiveEvent);    // register event  
    
//  patternController.displayFreeMemory(&legsPixelSet);
  delay(2000);
}


void loop()
{
//  digitalWrite(EXTERNAL_CONTROL_TX_EN_PIN, 1);
//  delay(1);
//  digitalWrite(EXTERNAL_CONTROL_TX_EN_PIN, 0);

  patternController.update();
}

