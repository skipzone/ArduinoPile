/*******************************************************************
 *                                                                 *
 * Test program for Gay Lynn's Soffit Holiday Lights               *
 *                                                                 *
 * by Ross Butler  Dec. 2020                       Fuck the 'rona. * 
 *                                                                 *
 *******************************************************************/

//#include "Esp8266WebPage.h"
//#include <ESPDMX.h>
//#include <ESP8266WiFi.h>
#include "FastLED.h"

//const String apPassword = "";
//const String apSsid = "";

//const String staSsid = "trees";
//const String staPasskey = "2083433061";

//const String staticIpAddress = "192.168.76.8";
//const String gatewayAddress = "192.168.76.3";
//const String subnetMask = "255.255.255.0";
//const String dns1Address = "192.168.76.3";
//const String dns2Address = "0.0.0.0";

#define MAX_SECTION_PATTERNS 39

#define LED_PIN 5
#define ANALOG_PIN A0
#define HOUSE_SOFFIT_DATA_PIN 13  // 12

#define HOUSE_SOFFIT_NUM_PIXELS 123

CRGB houseSoffitPixelArray[HOUSE_SOFFIT_NUM_PIXELS];


void initHardware()
{
  //Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
}


void setup() 
{
  // Let everything settle down after a rough power-up.
  delay(1000);

  initHardware();

  // The floating analog pin should be a good enough seed for the random number generator.
  randomSeed(analogRead(ANALOG_PIN));

  // Brilliant Bulb (tm) pixel strawberry strings:  WS2812B, RGB 
  FastLED.addLeds<WS2812B, HOUSE_SOFFIT_DATA_PIN, RGB>(houseSoffitPixelArray, HOUSE_SOFFIT_NUM_PIXELS);

  for (uint8_t i = 0; i < HOUSE_SOFFIT_NUM_PIXELS; ++i) {
//      houseSoffitPixelArray[i] = CRGB::Blue;
//      houseSoffitPixelArray[i] = CRGB::White;
      houseSoffitPixelArray[i] = i & 0x01 ? CRGB::Blue : CRGB::White;
  }
}


void loop() 
{
   FastLED.show();
   delay(25);
}
