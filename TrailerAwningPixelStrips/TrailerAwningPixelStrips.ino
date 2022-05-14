#define FASTSPI_USE_DMX_SIMPLE
#define DMX_SIZE 27

#include <DmxSimple.h>
#include "FastLED.h"

#define NUM_LEDS 60
#define HALF_NUM_LEDS 30
#define PIXEL1_DATA_PIN 2
#define PIXEL2_DATA_PIN 4
#define DMX_DATA_PIN 3

#define FG_INTENSITY 128

#define H_PIN 2
#define S_PIN 1
#define V_PIN 0

#define BSU_BLUE   0x0000FF
#define BSU_ORANGE 0xF01600


// gamma correction for human-eye perception of WS2812 RGB LED brightness
// (from http://rgb-123.com/ws2812-color-output/ on 3 April 2014)
uint8_t g_gamma[] = {
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,
      2,  2,  2,  3,  3,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,  5,
      6,  6,  6,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 11, 11,
     11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18,
     19, 19, 20, 21, 21, 22, 22, 23, 23, 24, 25, 25, 26, 27, 27, 28,
     29, 29, 30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 37, 38, 39, 40,
     40, 41, 42, 43, 44, 45, 46, 46, 47, 48, 49, 50, 51, 52, 53, 54,
     55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
     71, 72, 73, 74, 76, 77, 78, 79, 80, 81, 83, 84, 85, 86, 88, 89,
     90, 91, 93, 94, 95, 96, 98, 99,100,102,103,104,106,107,109,110,
    111,113,114,116,117,119,120,121,123,124,126,128,129,131,132,134,
    135,137,138,140,142,143,145,146,148,150,151,153,155,157,158,160,
    162,163,165,167,169,170,172,174,176,178,179,181,183,185,187,189,
    191,193,194,196,198,200,202,204,206,208,210,212,214,216,218,220,
    222,224,227,229,231,233,235,237,239,241,244,246,248,250,252,255
};


CRGB leds[NUM_LEDS];


void rotateCrgbRight(CRGB* a, uint8_t length)
{
  uint8_t i;
  CRGB temp;

  temp = a[length - 1];

  for (i = length - 1; i > 0; --i) {
    a[i] = a[i - 1];
  }

  a[0] = temp;
}


void setup() {
  
  delay(500);
  
  //FastLED.addLeds<DMXSIMPLE, DMX_DATA_PIN, RGB>(leds, NUM_LEDS);
  FastLED.addLeds<WS2812B, PIXEL1_DATA_PIN, GRB>(leds, NUM_LEDS);  // white pixel strips and 2015 pixel strings
  FastLED.addLeds<WS2812B, PIXEL2_DATA_PIN, GRB>(leds, NUM_LEDS);  // white pixel strips and 2015 pixel strings

/*
  // setup for Red, White, and Blue
  CRGB rgbColor;
  uint8_t i;
  uint8_t j;
  uint8_t k;
  uint8_t pixelsPerSegment = 9;
  uint8_t numSegments = NUM_LEDS / pixelsPerSegment;
  rgbColor = CRGB::Red;
  k = 0;
  rgbColor.nscale8_video(FG_INTENSITY);
  for (i = 0; i < numSegments; ++i) {
    for (j = 0; j < pixelsPerSegment; ++j) {
      leds[i * pixelsPerSegment + j] = rgbColor;
    }
    switch (k) {
      case 0:
        rgbColor = CRGB::White;
        k = 1;
        break;
      case 1:
        rgbColor = CRGB::Blue;
        k = 2;
        break;
      case 2:
        rgbColor = CRGB::Red;
        k = 0;
    }
    rgbColor.nscale8_video(FG_INTENSITY);
  }
*/

}


uint8_t h = 0;
uint8_t s = 0;
uint8_t v = 0;

uint8_t stepNum = 0;


void loop() {

  CRGB rgbColor;
  CHSV hsvColor;

  uint8_t i;
  uint8_t j;
  uint8_t k;
  uint8_t scaleFactor;
  float t;
  float y;
  float ledAngleInterval = TWO_PI / (float) NUM_LEDS;

//  rotateCrgbRight(leds, 100);

  hsvColor.h = stepNum++;
  hsvColor.s = 255;
  hsvColor.v = FG_INTENSITY;
  hsv2rgb_rainbow(hsvColor, rgbColor);
  fill_solid(leds, NUM_LEDS, rgbColor);

  FastLED.show();
  delay(1000);

/*
  // Rotating rainbow.
  fill_rainbow(leds, NUM_LEDS, h, 255 / NUM_LEDS);
  for (uint8_t i = 0; i < NUM_LEDS; leds[i++].nscale8_video(FG_INTENSITY));
  FastLED.show();
//  delay(10);      // fast
  delay(100);     // slow
  ++h;
*/

/*
  // Static rainbow.
#define NUM_RAINBOW_BANDS 6
  uint8_t numLedsInBand[NUM_RAINBOW_BANDS];
  uint8_t numLedsPerBand = NUM_LEDS / NUM_RAINBOW_BANDS;
  uint8_t leftover = NUM_LEDS % NUM_RAINBOW_BANDS;
  for (i = 0; i < NUM_RAINBOW_BANDS; ++i) {
    numLedsInBand[i] = numLedsPerBand;
  }
  i = 0;
  while (leftover > 0) {
    --leftover;
    ++numLedsInBand[i];
    if (++i >= NUM_LEDS) {
      i = 0;
    }
  }
  k = 0;
  for (i = 0; i < NUM_RAINBOW_BANDS; ++i) {
    for (j = 0; j < numLedsInBand[i]; ++j) {
      switch (i) {
        case 0:
          leds[k++] = CRGB::Red;
          break;
        case 1:
          leds[k++] = 0xF04000;
          break;
        case 2:
          leds[k++] = CRGB::Yellow;
          break;
        case 3:
          leds[k++] = CRGB::Green;
          break;
        case 4:
          leds[k++] = CRGB::Blue;
          break;
        case 5:
          leds[k++] = CRGB::Violet;
          break;
        default:
          leds[k++] = CRGB::Violet;
          break;
      }
    }
    leds[i] = CRGB::Red;
  }
  for (uint8_t i = 0; i < NUM_LEDS; leds[i++].nscale8_video(FG_INTENSITY));
  FastLED.show();
  delay(100);
*/

/*
  // 1/2 blue, 1/2 orange, alternating
  rgbColor = stepNum ? BSU_BLUE : BSU_ORANGE;
  fill_solid(leds, HALF_NUM_LEDS, rgbColor);
  rgbColor = stepNum ? BSU_ORANGE : BSU_BLUE;
  fill_solid(leds + (NUM_LEDS - HALF_NUM_LEDS), HALF_NUM_LEDS, rgbColor);
  for (uint8_t i = 0; i < NUM_LEDS; leds[i++].nscale8_video(FG_INTENSITY));
  FastLED.show();
  delay(2000);
  stepNum = !stepNum;
*/

/*
  // rotating 1/2 blue, 1/2 orange
  t = (float) stepNum / 255.0 * TWO_PI;
  for (i = 0; i < NUM_LEDS; ++i) {
    y = sin(t + ledAngleInterval * i);
    if (y >= 0) {
      rgbColor = BSU_ORANGE;
      scaleFactor = (float) FG_INTENSITY * y;
    }
    else {
      rgbColor = BSU_BLUE;
      scaleFactor = - (float) FG_INTENSITY * y;
    }
    leds[i] = rgbColor.nscale8_video(g_gamma[scaleFactor]);
  }
  FastLED.show();
  delay(20);      // medium-speed rotation
  ++stepNum;
*/  

/* H, S, and V controlled by potentiometers
 *
  hsvColor.h = analogRead(H_PIN) >> 2;
  hsvColor.s = analogRead(S_PIN) >> 2;
  hsvColor.v = analogRead(V_PIN) >> 2;
  hsv2rgb_rainbow(hsvColor, rgbColor);
  fill_solid(leds, NUM_LEDS, rgbColor);
  FastLED.show();
  delay(10);
  *
  */
  
}


