/*******************************************************************
 *                                                                 *
 * Gay Lynn's Patio and Soffit Holiday Lights                      *
 *                                                                 *
 * by Ross Butler  Oct. 2016                                       * 
 *                                                                 *
 *******************************************************************/

/*
 * To upload to Adafruit HUZZAH:
 * 1.  Hold down the GPIO0 button.  The red LED will be lit.
 * 2.  While holding down GPIO0, click the RESET button.
 * 3.  Release RESET, then release GPIO0.  The red LED will be lit dimly,
 *     meaning the board is in bootload mode.
 * 4.  Upload the sketch.
 * 5.  After the upload is complete, click the RESET button.
 */

#include "Esp8266WebPage.h"
#include <ESPDMX.h>
#include <ESP8266WiFi.h>
#include "FastLED.h"
#include "PixelPatternController.h"
#include "PatternSequence.h"
#include "pixelPatternFrameworkTypes.h"
#include "sequenceDefinition.h"
#include "ExternalControlSelector.h"
#include <vector>

using namespace pixelPattern;

// const String apSsid = "treelights";
// const String apPassword = "2083433061";
// const String staSsid = "";
// const String staPasskey = "";
// const String staticIpAddress = "";
// const String subnetMask = "";
// const String gatewayAddress = "";
// const String dns1Address = "";
// const String dns2Address = "";

const String staSsid = "trees";
const String staPasskey = "2083433061";
const String staticIpAddress = "192.168.76.8";
const String subnetMask = "255.255.255.0";
const String gatewayAddress = "192.168.76.3";
const String dns1Address = "192.168.76.3";
const String dns2Address = "0.0.0.0";
const String apSsid = "";
const String apPassword = "";

// const String staSsid = "meow";
// const String staPasskey = "11111111";
// const String staticIpAddress = "10.10.10.8";
// const String subnetMask = "255.255.255.0";
// const String gatewayAddress = "10.10.10.254";
// const String dns1Address = "10.10.10.254";
// const String dns2Address = "0.0.0.0";
// const String apSsid = "";
// const String apPassword = "";


#define NUM_SECTIONS 2
#define MAX_SECTION_PATTERNS 39

#define LED_PIN 5
#define ANALOG_PIN A0
#define HOUSE_SOFFIT_DATA_PIN 12
// The patio LED strips use DMX.

#define HOUSE_SOFFIT_NUM_PIXELS 123
#define PATIO_STRIPS_NUM_PIXELS 9

CRGB houseSoffitPixelArray[HOUSE_SOFFIT_NUM_PIXELS];
CRGB patioStripsPixelArray[PATIO_STRIPS_NUM_PIXELS];

PixelSet sectionPixelSets[NUM_SECTIONS] = {
  {
    houseSoffitPixelArray,
    HOUSE_SOFFIT_NUM_PIXELS,
    0,    // skip pixels
    0,    // non-symmetrical pixels
    1,    // number of panels
    HOUSE_SOFFIT_NUM_PIXELS,  // number of pixels per panel (should be symmetrical / panels)
    255,  // foreground intensity scale factor
    255   // background intensity scale factor 
  },
  {
    patioStripsPixelArray,
    PATIO_STRIPS_NUM_PIXELS,
    0,    // skip pixels
    0,    // non-symmetrical pixels
    1,    // number of panels
    PATIO_STRIPS_NUM_PIXELS,  // number of pixels per panel (should be symmetrical / panels)
    255,  // foreground intensity scale factor
    255   // background intensity scale factor 
  },
};

std::vector<Esp8266WebPage::Preset> presets = { 
  // name       pattern number for each section
  {"BSU Solid"  ,  {13,  6}},
  {"BSU Wave"   ,  {23,  5}},
  {"BSU Sparkle",  {25,  5}},
  {"Xmas Simple",  { 4,  1}},
  {"Xmas Wave"  ,  { 1,  1}},
  {"4th Of July",  {14,  7}},
  {"All Off"    ,  { 0,  0}}
};

ExternalControlSelector sectionPatternSelector[NUM_SECTIONS];
PatternSequence* sectionPatternSequences[NUM_SECTIONS];
uint8_t sectionPatternSequenceIdxs[NUM_SECTIONS];

DMXESPSerial dmx;
Esp8266WebPage esp8266WebPage;
PixelPatternController patternController;


void initHardware()
{
  //Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
}


void initPatterns()
{
  // Brilliant Bulb (tm) pixel strawberry strings:  WS2812B, RGB 
  FastLED.addLeds<WS2812B, HOUSE_SOFFIT_DATA_PIN, RGB>(houseSoffitPixelArray, HOUSE_SOFFIT_NUM_PIXELS);

  dmx.init();

  sectionPatternSequences[0] = new PatternSequence(houseSoffitPatternDefs, sizeof(houseSoffitPatternDefs) / sizeof(PatternDef), &sectionPatternSelector[0]);
  sectionPatternSequences[1] = new PatternSequence(patioStripsPatternDefs, sizeof(patioStripsPatternDefs) / sizeof(PatternDef), &sectionPatternSelector[1]);

  for (int sectionIdx = 0; sectionIdx < NUM_SECTIONS; ++sectionIdx) {
    sectionPatternSequenceIdxs[sectionIdx] =
      patternController.addPatternSequence(sectionPatternSequences[sectionIdx], &sectionPixelSets[sectionIdx]);
    // Initially, turn everything off.
    sectionPatternSelector[sectionIdx].setPatternNum(0);
  }

  patternController.enableStatusLed(LED_PIN);
  patternController.init();
}


void initWebPage()
{
  esp8266WebPage.setTitleText("Gay Lynn's Holiday Lights");
  esp8266WebPage.setHostname("HolidayLights");

  if (staticIpAddress != "") {
    esp8266WebPage.setStaticIpAddress(staticIpAddress, gatewayAddress, subnetMask, dns1Address, dns2Address);
  }

  if (staSsid != "") {
    esp8266WebPage.setSta(staSsid, staPasskey);
  }

  if (apSsid != "") {
    esp8266WebPage.setAp(apSsid, apPassword);
  }

  for (int sectionIdx = 0; sectionIdx < NUM_SECTIONS; ++sectionIdx) {
    esp8266WebPage.addPatternSequence(sectionPatternSequences[sectionIdx]);
  }

  esp8266WebPage.addPreset(presets);

  esp8266WebPage.init();
}


void writeToDmxLeds()
{
  for (int i = 0, dmxCh = 1; i < PATIO_STRIPS_NUM_PIXELS; ++i) {
    dmx.write(dmxCh++, patioStripsPixelArray[i].red);
    dmx.write(dmxCh++, patioStripsPixelArray[i].green);
    dmx.write(dmxCh++, patioStripsPixelArray[i].blue);
    dmx.update();
  }
}


void setup() 
{
  // Let everything settle down after a rough power-up.
  delay(1000);

  initHardware();

  // The floating analog pin should be a good enough seed for the random number generator.
  randomSeed(analogRead(ANALOG_PIN));

  initPatterns();

  // initWebPage();

  // TODO:  random pattern selection until Wi-Fi conflict is fixed
  sectionPatternSelector[0].setPatternNum(255);
  // sectionPatternSelector[1].setPatternNum(255);
}


void loop() 
{
  // esp8266WebPage.doWiFi();

  patternController.update();
  
  // if (patternController.getUpdateLeds(sectionPatternSequenceIdxs[1])) {
  //   writeToDmxLeds();
  // }
}
