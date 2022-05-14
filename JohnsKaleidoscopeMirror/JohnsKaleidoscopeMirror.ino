/*******************************************************************
 *                                                                 *
 * Pixel Pattern Library ESP8266 Web Page Test                     *
 * customized for Mzfit's Kaleidoscope Mirror                      *
 *                                                                 *
 * by Ross Butler  Nov. 2016                                       * 
 *                                                                 *
 *******************************************************************/

// /Users/ross/Library/Arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/1.20.0-26-gb404fb9-2/bin/xtensa-lx106-elf-objdump  -d -S /var/folders/pr/80vsgp_575lbbzk8x5wbbd7m0000gn/T/arduino_build_722264/ESP8266PixelPatternFrameworkTest.ino.elf > xxx

#include "Esp8266WebPage.h"
//#include <ESPDMX.h>
#include <ESP8266WiFi.h>
#include "FastLED.h"
#include "PixelPatternController.h"
#include "PatternSequence.h"
#include "pixelPatternFrameworkTypes.h"
#include "sequenceDefinition.h"
#include "ExternalControlSelector.h"
#include <vector>
#include <Wire.h>

extern "C" {
#include "ets_sys.h"  // ESP8266 timer and event stuff
#include "osapi.h"    // os_timer... definitions
}

using namespace pixelPattern;

enum class OperatingState {
    HARD_INIT,
    INIT,
    INIT_CONFIG,
    READ_CONFIG,
    INIT_PATTERNS,
    INIT_WEB_PAGE,
    RUN,
    STOP
}; 

enum class StatusBlinkState {
    INIT,
    DIGIT_START,
    LED_ON_START,
    LED_ON_DELAY,
    INTER_ELEMENT_DELAY,
    INTER_DIGIT_DELAY,
    INTER_MESSAGE_DELAY,
};

OperatingState opState;

unsigned int opStatus;

// To facilitate accurate visual blink counting, digits should be
// 5 or less, and zeros should only appear in leading positions.
constexpr unsigned int maxStatusDigits = 2;
constexpr unsigned int status_normal = 0x1;
constexpr unsigned int status_batteryLow = 0x2;
constexpr unsigned int status_hardInit = 0x3;
constexpr unsigned int status_initEarly = 0x11;
constexpr unsigned int status_init_config = 0x12;
constexpr unsigned int status_read_config = 0x13;
constexpr unsigned int status_initPatterns = 0x14;
constexpr unsigned int status_initWebPage = 0x15;
constexpr unsigned int status_configInitFail = 0x51;
constexpr unsigned int status_lowBatteryShutdown = 0x52;
constexpr unsigned int status_HardInitSuccess = 0x53;
constexpr unsigned int status_HardInitFail = 0x54;
constexpr unsigned int status_unknownFatalStop = 0x55;

StatusBlinkState statusBlinkState;
ETSTimer statusBlinkTimer;
constexpr unsigned int statusBlinkTimerIntervalMs = 50;
constexpr unsigned int statusBlinkLedOnDelay = 1;
constexpr unsigned int statusBlinkInterElementDelay = 5;
constexpr unsigned int statusBlinkInterDigitDelay = 18;
constexpr unsigned int statusBlinkInterMessageDelay = 24;

const String webPageTitle = "Mzfit's Kaleidoscope Mirror";
const String hostname = "KaleidoMirror";

const String apPassword = "";
//const String apSsid = "";
const String apSsid = "KaleidoMirror";

//const String staSsid = "";    // set to empty string to disable station mode
//const String staPasskey = "";
const String staSsid = "MzFits";
const String staPasskey = "Ch@nged!t5/6";
//const String staSsid = "trees";
//const String staPasskey = "2083433061";
//const String staSsid = "XanaduShop";
//const String staPasskey = "xanadushop";

const String staticIpAddress = "";
const String gatewayAddress = "";
const String subnetMask = "";
const String dns1Address = "0.0.0.0";
const String dns2Address = "0.0.0.0";
//const String staticIpAddress = "192.168.76.98";
//const String gatewayAddress = "192.168.76.3";
//const String subnetMask = "255.255.255.0";
//const String dns1Address = "192.168.76.3";
//const String dns2Address = "0.0.0.0";

constexpr uint8_t systemModeButtonPin = 4;
constexpr int systemModeButtonActive = 0;

constexpr uint8_t systemStatusLedPin = 0;
constexpr uint8_t systemStatusLedOff = HIGH;
constexpr uint8_t systemStatusLedOn = LOW;

constexpr uint32_t hardInitButtonDownCheckPeriodMs = 3000;

#define ANALOG_PIN A0
constexpr float vBatLsb = 15.83 / 972.0;        // at 1 V to ADC:  Vin = 15.83, Vdivout=1.000, A0 = 972 (avg)

#define STATUS_TEXT_UPDATE_INTERVAL_MS 1000

#define NUM_SECTIONS 1
#define MAX_SECTION_PATTERNS 10

#define SECTION_0_DATA_PIN 13

#define SECTION_0_NUM_PIXELS 99

#define SECTION_0_NUM_PANELS 3

CRGB section0PixelArray[SECTION_0_NUM_PIXELS];

PixelSet sectionPixelSets[NUM_SECTIONS] = {
    {
        section0PixelArray,
        SECTION_0_NUM_PIXELS,
        0,    // skip pixels
        0,    // non-symmetrical pixels
        SECTION_0_NUM_PANELS,   // number of panels
        SECTION_0_NUM_PIXELS / SECTION_0_NUM_PANELS,   // number of pixels per panel (should be symmetrical / panels)
        128,  // foreground intensity scale factor
        48    // background intensity scale factor 
    },
};

std::vector<Esp8266WebPage::Preset> presets = { 
  // name       pattern number for each section
  {"All Off"   ,  {0}}
};

ExternalControlSelector sectionPatternSelector[NUM_SECTIONS];
PatternSequence* sectionPatternSequences[NUM_SECTIONS];
uint8_t sectionPatternSequenceIdxs[NUM_SECTIONS];

//DMXESPSerial dmx;
Esp8266WebPage esp8266WebPage;
PixelPatternController patternController;



void statusBlinkTimerCallback(void *arg)
{
  static unsigned int delayIntervalCount;
  static unsigned int opStatusDigits;
  static unsigned int digitCount;
  static unsigned int blinkCount;
  static unsigned int numBlinks;

  switch(statusBlinkState)
  {
    case StatusBlinkState::INIT:
      digitalWrite(systemStatusLedPin, systemStatusLedOff);
      // If the delay config isn't fucky, just do an inter-digit
      // delay before starting with the first status display.
      delayIntervalCount = statusBlinkInterMessageDelay > statusBlinkInterElementDelay
        ? statusBlinkInterMessageDelay - statusBlinkInterElementDelay
        : 0;
      statusBlinkState = StatusBlinkState::INTER_MESSAGE_DELAY;
      break;

    case StatusBlinkState::DIGIT_START:
      ++digitCount;
      numBlinks = opStatusDigits & 0x0f;
      opStatusDigits >>= 4;
      blinkCount = 0;
      delayIntervalCount = 0;
      statusBlinkState = numBlinks > 0
        ? StatusBlinkState::LED_ON_START
        : StatusBlinkState::INTER_DIGIT_DELAY;
      break;

    case StatusBlinkState::LED_ON_START:
      ++blinkCount;
      digitalWrite(systemStatusLedPin, systemStatusLedOn);
      delayIntervalCount = 0;
      statusBlinkState = StatusBlinkState::LED_ON_DELAY;
      break;
        
    case StatusBlinkState::LED_ON_DELAY:
      if (++delayIntervalCount >= statusBlinkLedOnDelay) {
        digitalWrite(systemStatusLedPin, systemStatusLedOff);
        delayIntervalCount = 0;
        statusBlinkState = blinkCount < numBlinks
          ? StatusBlinkState::INTER_ELEMENT_DELAY
          : StatusBlinkState::INTER_DIGIT_DELAY;
      }
      break;

    case StatusBlinkState::INTER_ELEMENT_DELAY:
      if (++delayIntervalCount >= statusBlinkInterElementDelay) {
        statusBlinkState = StatusBlinkState::LED_ON_START;
      }
      break;

    case StatusBlinkState::INTER_DIGIT_DELAY:
      if (++delayIntervalCount >= statusBlinkInterDigitDelay) {
        delayIntervalCount = 0;
        statusBlinkState = digitCount < maxStatusDigits
          ? StatusBlinkState::DIGIT_START
          : StatusBlinkState::INTER_MESSAGE_DELAY;
      }
      break;

    case StatusBlinkState::INTER_MESSAGE_DELAY:
      if (++delayIntervalCount >= statusBlinkInterMessageDelay) {
        digitCount = 0;
        // Reverse the order of the nibbles in opStatus so that they
        // will be displayed in most-to-least-significant order.
        opStatusDigits = 0;
        int t = opStatus;
        for (int i = 0; i < maxStatusDigits; ++i) {
          opStatusDigits <<= 4;
          opStatusDigits |= t & 0x0f;
          t >>= 4;
        }
        delayIntervalCount = 0;
        statusBlinkState = StatusBlinkState::DIGIT_START;
      }
      break;
  }

}


void initHardware()
{
  Serial.begin(115200);
  pinMode(systemModeButtonPin, INPUT);
  pinMode(systemStatusLedPin, OUTPUT);
  digitalWrite(systemStatusLedPin, systemStatusLedOn);
  //Serial.println("hardware init done");
}


void initTimers()
{
//  ets_timer_setfn(&statusBlinkTimer, &statusBlinkTimerCallback, &statusBlinkTimerArg);
//  ets_timer_arm_new(&statusBlinkTimer, statusBlinkTimerIntervalMs, 1, 1);   // repeat = 1, isMsTimer = 1
  os_timer_setfn(&statusBlinkTimer, &statusBlinkTimerCallback, NULL);
  os_timer_arm(&statusBlinkTimer, statusBlinkTimerIntervalMs, 1);   // repeat = 1
}


void initPatterns()
{
  // 10 pixels/m 12 V white strips from HolidayCoro 11/2015:  WS2812B, BRG
  // 60? pixels/m 5 V white 8-pixel strip gift from E11 2015:  WS2812B, GRB
  // 60? pixels/m 5 V white strips (John's mirror, infinity bar top?):  WS2812B, GRB
  // Brilliant Bulb (tm) pixel strawberry strings:  WS2812B, RGB
  // 144 pixels/m strip:  WS2812B, GRB
  FastLED.addLeds<WS2812B, SECTION_0_DATA_PIN, GRB>(section0PixelArray, SECTION_0_NUM_PIXELS);

//  dmx.init();

  sectionPatternSequences[0] = new PatternSequence(section0PatternDefs, sizeof(section0PatternDefs) / sizeof(PatternDef), &sectionPatternSelector[0]);

  for (int sectionIdx = 0; sectionIdx < NUM_SECTIONS; ++sectionIdx) {
    sectionPatternSequenceIdxs[sectionIdx] =
      patternController.addPatternSequence(sectionPatternSequences[sectionIdx], &sectionPixelSets[sectionIdx]);
    sectionPatternSelector[sectionIdx].setPatternNum(255);    // start off in automatic mode
  }

  //patternController.enableStatusLed(LED_PIN);
  patternController.init();

  //Serial.println("pattern init done");
}


void initWebPage()
{
  esp8266WebPage.setTitleText(webPageTitle);
  esp8266WebPage.setHostname(hostname);
  
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

  //Serial.println("wifi init done");
}


void updateStatusText()
{
  unsigned int ia0 = 0;
  for (int i = 0; i < 16; ++i) {
    ia0 += analogRead(ANALOG_PIN);
  }
  float a0 = ia0 >> 4;
  float vBat = vBatLsb * a0;
//  Serial.print("a0=");
//  Serial.print(a0);
//  Serial.print(" vBat=");
//  Serial.println(vBat);
  String statusText = "<h3>Battery: " + String(vBat, 1) + " V</h3>";
  esp8266WebPage.setStatusText(statusText);
}


//void writeToDmxLeds()
//{
//  for (int i = 0, dmxCh = 1; i < SECTION_4_NUM_PIXELS; ++i) {
//    dmx.write(dmxCh++, section4PixelArray[i].red);
//    dmx.write(dmxCh++, section4PixelArray[i].green);
//    dmx.write(dmxCh++, section4PixelArray[i].blue);
//    dmx.update();
//  }
//}


void setup() 
{
  // Let everything settle down after a rough power-up,
  // and wait a while to make sure power remains applied.
  delay(1000);

  initHardware();

  // Check if a hard initialization is being requested
  // by way of holding down the mode button.
  uint32_t endHardInitCheckMs = millis() + hardInitButtonDownCheckPeriodMs;
  while (digitalRead(systemModeButtonPin) == systemModeButtonActive
         && ((int32_t) (millis() - endHardInitCheckMs) < 0))
  {
    delay(1);
  }
  if (digitalRead(systemModeButtonPin) == systemModeButtonActive) {
    opStatus = status_hardInit;
    statusBlinkState = StatusBlinkState::INIT;
    opState = OperatingState::HARD_INIT;
  }
  else {
    opStatus = status_initEarly;
    statusBlinkState = StatusBlinkState::INIT;
    opState = OperatingState::INIT;
  }

  initTimers();

  // The battery voltage should be a good enough seed for the random number generator.
  randomSeed(analogRead(ANALOG_PIN));
}


void loop() 
{
  static uint32_t nextStatusTextUpdateMs;
  uint32_t now;

  switch(opState) {

    case OperatingState::HARD_INIT:
      // Do the hard init only after the mode button has been released.  That
      // way, we'll continue to display the hard init status while the button
      // is pressed to give confirmation that hard init was triggered.
      if (digitalRead(systemModeButtonPin) != systemModeButtonActive) {
        // TODO:  call whatever is going to do the hard init, set the status to status_HardInitFail if it fails
        opStatus = status_HardInitSuccess;
        opState = OperatingState::STOP;
      }
      break;

    case OperatingState::INIT:
      nextStatusTextUpdateMs = millis();
      opState = OperatingState::INIT_CONFIG;
      break;

    case OperatingState::INIT_CONFIG:
      opStatus = status_init_config;
      Serial.println("state:  INIT_CONFIG");
      opState = OperatingState::READ_CONFIG;
      break;

    case OperatingState::READ_CONFIG:
      opStatus = status_read_config;
      Serial.println("state:  READ_CONFIG");
      opState = OperatingState::INIT_PATTERNS;
      break;

    case OperatingState::INIT_PATTERNS:
      opStatus = status_initPatterns;
      Serial.println("state:  INIT_PATTERNS");
      initPatterns();
      opState = OperatingState::INIT_WEB_PAGE;
      break;

    case OperatingState::INIT_WEB_PAGE:
      opStatus = status_initWebPage;
      Serial.println("state:  INIT_WEB_PAGE");
      initWebPage();
      opState = OperatingState::RUN;
      opStatus = status_normal;
      break;

    case OperatingState::RUN:
      now = millis();
//      Serial.print(now);
//      Serial.print(" - ");
//      Serial.println(nextStatusTextUpdateMs);
      if ((int32_t) (now - nextStatusTextUpdateMs) >= 0) {
        nextStatusTextUpdateMs += STATUS_TEXT_UPDATE_INTERVAL_MS;
        updateStatusText();
      }
      esp8266WebPage.doWiFi();
      patternController.update();
//      if (patternController.getUpdateLeds(sectionPatternSequenceIdxs[4])) {
//        writeToDmxLeds();
//      }
      break;

    case OperatingState::STOP:
      // Do nothing; just let the status light blink--if it can.  If something hasn't
      // changed the operating status from normal to some other code, force it to
      // indicate that there has been a fatal stop for an unknown reason.
      if (status_normal == opStatus) {
        opStatus = status_unknownFatalStop;
      }
      break;
  }

}


