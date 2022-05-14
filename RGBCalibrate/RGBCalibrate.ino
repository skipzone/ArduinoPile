//#define FASTSPI_USE_DMX_SIMPLE
//#define DMX_SIZE 27

//#include <DmxSimple.h>

#define ARDUINO_SAMD_ZERO // need to define this to make Sparkfun ATSAMD21G R3 board work
#include "FastLED.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
//
// RGB Calibration code
//
// Use this sketch to determine what the RGB ordering for your chipset should be.  Steps for setting up to use:

// * Uncomment the line in setup that corresponds to the LED chipset that you are using.  (Note that they
//   all explicitly specify the RGB order as RGB)
// * Define DATA_PIN to the pin that data is connected to.
// * (Optional) if using software SPI for chipsets that are SPI based, define CLOCK_PIN to the clock pin
// * Compile/upload/run the sketch 

// You should see six leds on.  If the RGB ordering is correct, you should see 1 red led, 2 green 
// leds, and 3 blue leds.  If you see different colors, the count of each color tells you what the 
// position for that color in the rgb orering should be.  So, for example, if you see 1 Blue, and 2
// Red, and 3 Green leds then the rgb ordering should be BRG (Blue, Red, Green).  

// You can then test this ordering by setting the RGB ordering in the addLeds line below to the new ordering
// and it should come out correctly, 1 red, 2 green, and 3 blue.

// 16 June 2015 Ross:  If NUM_LEDS > 6, additional LEDs are set to white.

//////////////////////////////////////////////////

#define NUM_LEDS 32

// Data pin that led data will be written out over
#define DATA_PIN 3
// Clock pin only needed for SPI based chipsets when not using hardware SPI
//#define CLOCK_PIN 8

CRGB leds[NUM_LEDS];

void setup() {
	// sanity check delay - allows reprogramming if accidently blowing power w/leds
  // 	delay(2000);

      // Uncomment one of the following lines for your leds arrangement.
      // FastLED.addLeds<TM1803, DATA_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<TM1804, DATA_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<TM1809, DATA_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
      //FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
      //FastLED.addLeds<WS2812B, DATA_PIN, BRG>(leds, NUM_LEDS);  // 10 pixels/m 12 V white strips from HolidayCoro 11/2015
      FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS); // 60 pixels/m 4m 5 V white strips (devel. strip, lampshade hat); Joule strips
      //FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS); // 60 pixels/m 4m 5 V white strips (devel. strip, lampshade hat) 
      // FastLED.addLeds<DMXSIMPLE, DATA_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<UCS1903, DATA_PIN, RGB>(leds, NUM_LEDS);

      // FastLED.addLeds<WS2801, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<SM16716, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<LPD8806, RGB>(leds, NUM_LEDS);

      // FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<SM16716, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<LPD8806, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
}

void loop() {
   leds[0] = CRGB::Red; 
   leds[1] = CRGB::Green;
   leds[2] = CRGB::Green;
   leds[3] = CRGB::Blue;
   leds[4] = CRGB::Blue;
   leds[5] = CRGB::Blue;
   for (uint8_t i = 6; i < NUM_LEDS; leds[i++] = CRGB::White);
   for (uint8_t i = 0; i < NUM_LEDS; leds[i++] %= 32);   // scale to 1/8 brightness because full brightness hurts
   FastLED.show();
   delay(1000);
}
