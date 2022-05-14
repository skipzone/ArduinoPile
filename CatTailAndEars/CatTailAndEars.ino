//#define FASTSPI_USE_DMX_SIMPLE
#define DMX_SIZE 27

//#include <DmxSimple.h>
#include "FastLED.h"

#define NUM_LEDS 42
#define HALF_NUM_LEDS 21
#define DATA_PIN 3
#define EARS_PIN 5

#define BSU_BLUE   0x0000FF
#define BSU_ORANGE 0xC02000  // for WS2812B 5V pixel strips purchased in 2013

#define EARS_INTENSITY_SCALE_FACTOR 64
#define PIXEL_INTENSITY_SCALE_FACTOR 32
#define PATTERN_CHANGE_INTERVAL_MS 60000

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

uint8_t stepNum = 0;
uint8_t stepDir = 0;
uint8_t stepDelay = 0;
uint8_t patternNum = 0;
bool changeDir = false;
unsigned long nextPatternChangeMs;



void setup() {

  delay(200);   // let shit settle down

  pinMode(EARS_PIN, OUTPUT);     
//  digitalWrite(EARS_PIN, LOW);  
  analogWrite(EARS_PIN, EARS_INTENSITY_SCALE_FACTOR);

  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);  // white pixel strips
  
  nextPatternChangeMs = millis() + PATTERN_CHANGE_INTERVAL_MS;
}


void loop() {

  CRGB rgbColor;
  CHSV hsvColor;

  uint8_t i;
  uint8_t scaleFactor;
  float t;
  float y;
  float ledAngleInterval = TWO_PI / (float) NUM_LEDS;
  bool initPattern = false;

  if (millis() >= nextPatternChangeMs) {
    nextPatternChangeMs = millis() + PATTERN_CHANGE_INTERVAL_MS;
    ++patternNum;
    initPattern = true;
    stepNum = 0;
    stepDir = 0;
    rgbColor = CRGB::Black;
    fill_solid(leds, NUM_LEDS, rgbColor);
    FastLED.show();
    delay(1000);
  }  

  switch (patternNum) {
    
    case 1:
      // Le Tricolore
      rgbColor = CRGB::Blue;
      rgbColor.nscale8_video(PIXEL_INTENSITY_SCALE_FACTOR);
      for (i = 0; i < HALF_NUM_LEDS / 3; ++i) {
        leds[i] = rgbColor;        
        leds[HALF_NUM_LEDS + i] = rgbColor;        
      }
      rgbColor = CRGB::White;
      rgbColor.nscale8_video(PIXEL_INTENSITY_SCALE_FACTOR);
      for (i = 0; i < HALF_NUM_LEDS / 3; ++i) {
        leds[HALF_NUM_LEDS / 3 + i] = rgbColor;        
        leds[HALF_NUM_LEDS + (HALF_NUM_LEDS / 3 + i)] = rgbColor;        
      }
      rgbColor = CRGB::Red;
      rgbColor.nscale8_video(PIXEL_INTENSITY_SCALE_FACTOR);
      for (i = 0; i < HALF_NUM_LEDS / 3; ++i) {
        leds[2 * HALF_NUM_LEDS / 3 + i] = rgbColor;        
        leds[HALF_NUM_LEDS + (2 * HALF_NUM_LEDS / 3 + i)] = rgbColor;        
      }
      FastLED.show();
      delay(100);
      break;

    case 2:
      // laser dot
      if (initPattern) {
        stepDelay = random(25) + 5;
      }
      changeDir = !random(20);
      leds[stepNum] = CRGB::Black;
      leds[HALF_NUM_LEDS + stepNum] = CRGB::Black;
      if (!stepDir) {
        if (++stepNum >= HALF_NUM_LEDS || (changeDir && stepNum > 3)) {
          stepNum -= 2;
          stepDir = 1;
          stepDelay = random(20) + 10;
        }
      }
      else {
        if (--stepNum == 0 || changeDir) {
          stepDir = 0;
          stepDelay = random(20) + 10;
        }
      }
      leds[stepNum] = CRGB::Red;
      leds[HALF_NUM_LEDS + stepNum] = CRGB::Red;
      FastLED.show();
      delay(stepDelay);
      break;

    case 3:
      // white sparkle on blue
      rgbColor = CRGB::Blue;
      rgbColor.nscale8_video(PIXEL_INTENSITY_SCALE_FACTOR);
      if (!stepDir) {
        stepDir = 1;
        fill_solid(leds, NUM_LEDS, rgbColor);
      }
      leds[stepNum] = rgbColor;
      leds[HALF_NUM_LEDS + stepNum] = rgbColor;
      stepNum = random(HALF_NUM_LEDS);
      leds[stepNum] = CRGB::White;
      leds[HALF_NUM_LEDS + stepNum] = CRGB::White;
      FastLED.show();
      delay(15);
      break;

    case 4:
      // solid blues
      if (initPattern) {
        stepNum = (int) HUE_AQUA;
        stepDir = 1;
      }
      if (stepDir) {
        if (++stepNum == (HSVHue) 175) {
          stepDir = 0;
        }
      }
      else {
        if (--stepNum == HUE_AQUA) {
          stepDir = 1;
        }
      }
      hsvColor.h = stepNum;
      hsvColor.s = 255;
      hsvColor.v = 255;
      hsv2rgb_rainbow(hsvColor, rgbColor);
      fill_solid(leds, HALF_NUM_LEDS, rgbColor);
      for (i = 0; i < HALF_NUM_LEDS; ++i) {
        leds[HALF_NUM_LEDS + i] = leds[i];
      }
      for (i = 0; i < NUM_LEDS; leds[i++].nscale8_video(PIXEL_INTENSITY_SCALE_FACTOR));
      FastLED.show();
      delay(300);
      break;

    case 5:
      // Rotating rainbow.
      fill_rainbow(leds, HALF_NUM_LEDS, stepNum, 255 / HALF_NUM_LEDS);
      for (i = 0; i < HALF_NUM_LEDS; ++i) {
        leds[HALF_NUM_LEDS + i] = leds[i];
      }
      for (i = 0; i < NUM_LEDS; leds[i++].nscale8_video(PIXEL_INTENSITY_SCALE_FACTOR));
      FastLED.show();
      delay(10);
      --stepNum;
      break;

    case 6:
      // blue sparkle
      leds[stepNum] = CRGB::Black;
      leds[HALF_NUM_LEDS + stepNum] = CRGB::Black;
      stepNum = random(HALF_NUM_LEDS);
      leds[stepNum] = CRGB::Blue;
      leds[HALF_NUM_LEDS + stepNum] = CRGB::Blue;
      FastLED.show();
      delay(15);
      break;

//    case 1:
//      // 1/2 blue, 1/2 orange, alternating
//      rgbColor = stepNum ? BSU_BLUE : BSU_ORANGE;
//      rgbColor.nscale8_video(PIXEL_INTENSITY_SCALE_FACTOR);
//      fill_solid(leds, HALF_NUM_LEDS, rgbColor);
//      rgbColor = stepNum ? BSU_ORANGE : BSU_BLUE;
//      rgbColor.nscale8_video(PIXEL_INTENSITY_SCALE_FACTOR);
//      fill_solid(leds + (NUM_LEDS - HALF_NUM_LEDS), HALF_NUM_LEDS, rgbColor);
//      FastLED.show();
//      delay(2000);
//      stepNum = !stepNum;
//      break;

//    case 1:
//      // rotating 1/2 blue, 1/2 orange
//      t = (float) stepNum / 255.0 * TWO_PI;
//      for (i = 0; i < NUM_LEDS; ++i) {
//        y = sin(t + ledAngleInterval * i);
//        if (y >= 0) {
//          rgbColor = BSU_ORANGE;
//          scaleFactor = 128.0 * y;
//        }
//        else {
//          rgbColor = BSU_BLUE;
//          scaleFactor = -128.0  * y;
//        }
//        leds[i] = rgbColor.nscale8_video(g_gamma[scaleFactor]);
//      }
//      FastLED.show();
//      delay(20);      // medium-speed rotation
//      ++stepNum;
//      break;

//    case 3:
//      // recoil laser dot
//      leds[stepNum] = CRGB::Black;
//      leds[stepNum + HALF_NUM_LEDS] = CRGB::Black;
//      if (!stepDir) {
//        if (++stepNum >= HALF_NUM_LEDS) {
//          stepNum -= 2;
//          stepDir = 1;
//        }
//      }
//      else {
//        if (--stepNum == 0) {
//          stepDir = 0;
//        }
//      }
//      leds[stepNum] = CRGB::Red;
//      leds[NUM_LEDS - 1 - stepNum] = CRGB::Red;
//      //leds[stepNum] = rgbColor.nscale8_video(PIXEL_INTENSITY_SCALE_FACTOR);
//      FastLED.show();
//      delay(stepDir ? 30 : 10);
//      break;

    default:
      patternNum = 1;
      initPattern = true;
  }
  
}


