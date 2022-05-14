//#define FASTSPI_USE_DMX_SIMPLE
//#define DMX_SIZE 27

//#include <DmxSimple.h>
#include "FastLED.h"

//#define NUM_LEDS 9  // 5 * 3 = 15 A, which is the maximum for a DMX-27 board
#define NUM_LEDS 50

#define MAX_INTENSITY 128

#define PIXEL_DATA_PIN 2
#define DMX_DATA_PIN 3
#define CLOCK_PIN 4   // Clock pin only needed for SPI based chipsets when not using hardware SPI
#define COLOR_MODE_PIN 11
#define TEST_MODE_PIN 12
#define ONBOARD_LED_PIN 13
#define H_OR_R_APIN 0
#define S_OR_G_APIN 1
#define V_OR_B_APIN 2


CRGB leds[NUM_LEDS];


void setup() {

  // Uncomment one of the following lines for your leds arrangement.
  // FastLED.addLeds<TM1803, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<TM1804, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<TM1809, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<WS2811, PIXEL_DATA_PIN, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, PIXEL_DATA_PIN, BRG>(leds, NUM_LEDS);
  // FastLED.addLeds<WS2812, PIXEL_DATA_PIN, RGB>(leds, NUM_LEDS);
  //FastLED.addLeds<WS2812B, PIXEL_DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<WS2812B, PIXEL_DATA_PIN, GRB>(leds, NUM_LEDS);
  // FastLED.addLeds<DMXSIMPLE, DMX_DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<UCS1903, DATA_PIN, RGB>(leds, NUM_LEDS);

  // FastLED.addLeds<WS2801, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<SM16716, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<LPD8806, RGB>(leds, NUM_LEDS);

  // FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<SM16716, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<LPD8806, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
  
  pinMode(COLOR_MODE_PIN, INPUT_PULLUP);
  pinMode(TEST_MODE_PIN, INPUT_PULLUP);
  pinMode(ONBOARD_LED_PIN, OUTPUT);
}


void loop() {

  static uint8_t colorStepNum = 0;
  static uint8_t intensityStepNum = 0;
  static bool intensityUp;
  static CRGB rgbColor;
  CRGB rgbColorFaded;
  CHSV hsvColor;

  bool hsvMode = !digitalRead(COLOR_MODE_PIN);
  digitalWrite(ONBOARD_LED_PIN, hsvMode);

  if (!digitalRead(TEST_MODE_PIN)) {
    
    // Facilitate manual adjustment of color of entire strip.
    
    uint8_t hOrR = analogRead(H_OR_R_APIN) >> 2;
    uint8_t sOrG = analogRead(S_OR_G_APIN) >> 2;
    uint8_t vOrB = analogRead(V_OR_B_APIN) >> 2;
  
    if (hsvMode) {
      hsvColor.h = hOrR;
      hsvColor.s = sOrG;
      hsvColor.v = vOrB;
      hsv2rgb_rainbow(hsvColor, rgbColor);
    }
    else {
      rgbColor.red = hOrR;
      rgbColor.green = sOrG;
      rgbColor.blue = vOrB;
    }
  
    fill_solid(leds, NUM_LEDS, rgbColor);
    FastLED.show();
    delay(10);
  }
  
  else {
    
    // Do test pattern.
    
    if (intensityStepNum > 0) {
      if (intensityUp) {
        if (++intensityStepNum == MAX_INTENSITY) {
          intensityUp = false;
          // Blink the last LED a few times.
          for (uint8_t i = 0; i < 5; ++i) {
            leds[NUM_LEDS - 1] = CRGB::Black;
            FastLED.show();
            delay(100);
            leds[NUM_LEDS - 1] = leds[NUM_LEDS - 2];
            FastLED.show();
            delay(100);
            if (7 == colorStepNum) {
              delay(3000);
            }
          }
        }
      }
      else {
        --intensityStepNum;
      }
      rgbColorFaded = rgbColor;
      rgbColorFaded.nscale8_video(intensityStepNum);
      fill_solid(leds, NUM_LEDS, rgbColorFaded);
      FastLED.show();
      delay(5);
    }
    else {
      intensityStepNum = 1;
      intensityUp = true;
      if (++colorStepNum > 7)
        colorStepNum = 1;
      rgbColor.red = colorStepNum & 0x01 ? 255 : 0;
      rgbColor.green = colorStepNum & 0x02 ? 255 : 0;
      rgbColor.blue = colorStepNum & 0x04 ? 255 : 0;
    }

  }

}


