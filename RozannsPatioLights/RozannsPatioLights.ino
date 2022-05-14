/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Driver for Rozann's Patio Lights          *
 *                                                                 *
 * Platform:  Arduino Uno, Pro, Pro Mini                           *
 *                                                                 *
 * Architecture and pattern functions by Ross Butler               *
 * Nov. 2014 - Nov. 2015                                           *
 *                                                                 *
 *****************************************************************/

#include <avr/pgmspace.h>
#include "implementationConfig.h"
#ifdef ENABLE_DMX_AMBIENCE_LIGHTS
  #include <DmxSimple.h>
#endif
#include <EEPROM.h>
#include "FastLED.h"
#include "patternFrameworkTypes.h"
#include "patternDefinitions.h"
#include "sequenceDefinition.h"


// EEPROM version and locations
#define EEPROM_VERSION 0x01
#define EEPROM_ADDR_VERSION           0x00
#define EEPROM_ADDR_PATTERN_SELECTION 0x01

// Calculate the number of patterns that make up the entire show.
#define NUM_PATTERNS (sizeof(patternsDefs) / sizeof(patternDef_t))


/********************************
 * Helper Function Declarations *
 ********************************/

void selectRandomRgb(CRGB* rgbColor, CRGB* rgbColorInverse);
void selectRandomHue(HSVHue* hue, HSVHue* hueInverse);
void rotateCrgbRight(CRGB* a, uint16_t length);
void rotateCrgbLeft(CRGB* a, uint16_t length);
void replicatePixelPanels();



/***********
 * Globals *
 ***********/

CRGB pixelArray[NUM_PHYSICAL_PIXELS];
CRGB* pixels = pixelArray + NUM_SKIP_PIXELS;

static unsigned long nextPatternUpdateMs = 0;

#ifdef ENABLE_DMX_AMBIENCE_LIGHTS
CRGB dmxRgbArray[NUM_DMX_RGB_CHANNELS];
#endif



/*****************************
 * Pattern Drawing Functions *
 *****************************/


void shine(void* patParams)
{
  static shineParams_t* pp;
  static unsigned long nextShineMs;
  static bool doingShine = false;
//  static uint8_t shineStepNum;

  static float n0;
  static float s;
  static float w;
  static float ns;

  unsigned long now = millis();

  if (patParams) {
    pp = (shineParams_t*) patParams;
    nextShineMs = now + pp->shineInterval;
    s = 0.025;
    w = (NUM_PIXELS <= 30) ? 6 : NUM_PIXELS / 5;
    ns = 255.0 / (float) w;
    return;
  }


  nextPatternUpdateMs = millis() + pp->delay;

  if (!doingShine && now >= nextShineMs) {
    n0 = - w;
    doingShine = true;
//    Serial.println("starting shine");
//    Serial.print("s=");
//    Serial.print(s);
//    Serial.print("  w=");
//    Serial.print(w);
//    Serial.print("  ns=");
//    Serial.print(ns);
//    Serial.print("  n0=");
//    Serial.println(n0);
  }

  CHSV hsvColor;
  CRGB rgbColor;
  hsvColor.h = pp->bgHue;
  hsvColor.v = BG_INTENSITY_SCALE_FACTOR;
  for (uint16_t i = 0; i < NUM_PIXELS; ++i) {

    if (doingShine) {
      if (i < n0 || i > n0 + w) {
        hsvColor.s = 255;
      }
      else {
        float n = (((float) i) - n0) * ns;
        float y = quadwave8(n);
        hsvColor.s = 255 - y;
//        Serial.print("i=");
//        Serial.print(i);
//        Serial.print("  n0=");
//        Serial.print(n0);
//        Serial.print("  n=");
//        Serial.print(n);
//        Serial.print("  y=");
//        Serial.print(y);
//        Serial.print("  hsvColor.s=");
//        Serial.print(hsvColor.s);
//        Serial.println("----------");
      }
    }
    else {
      hsvColor.s = 255;
    }

    hsv2rgb_rainbow(hsvColor, rgbColor);
    pixels[i] = rgbColor;  
  }

  // Make the non-symmetrical pixels the background color.
  hsvColor.s = 255;
  hsv2rgb_rainbow(hsvColor, rgbColor);
  for (uint16_t i = NUM_PIXELS; i < NUM_PIXELS + NUM_NONSYMMETRICAL_PIXELS; pixels[i++] = rgbColor);

  FastLED.show();

  if (doingShine) {
    n0 += s;
    if (n0 >= (float) NUM_PIXELS) {
      doingShine = false;
      nextShineMs = now + pp->shineInterval;
//      Serial.println("shine done");
    }
  }
}



void xmasLights(void* patParams)
{
  static xmasLightsParams_t* pp;

  if (patParams) {
    pp = (xmasLightsParams_t*) patParams;
    return;
  }


  nextPatternUpdateMs = millis() + 100;

  uint8_t hueNum = 0;
  for (uint16_t i = 0; i < NUM_PIXELS; ++i) {

    CHSV hsvColor;
    hsvColor.h = pp->hues[hueNum];
    hsvColor.s = 255;
    hsvColor.v = 255;
    CRGB rgbColor;
    hsv2rgb_rainbow(hsvColor, rgbColor);
    pixels[i] = rgbColor;
    
    if (++hueNum >= pp->numHues) {
      hueNum = 0;
    }
  }
  
  for (uint16_t i = 0; i < NUM_PIXELS; pixels[i++].nscale8_video(BG_INTENSITY_SCALE_FACTOR));
  FastLED.show();
}



void solidColor(void* patParams)
{
  static solidColorParams_t* pp;
  static uint8_t stepNum;
  static uint8_t stepDir;  // non-zero for start->end, zero for end->start

  if (patParams) {
    pp = (solidColorParams_t*) patParams;
    stepNum = pp->startHue;
    stepDir = 1;
    return;
  }


  nextPatternUpdateMs = millis() + pp->delay;

  CHSV hsvColor;
  hsvColor.h = stepNum;
  hsvColor.s = pp->saturation;
  hsvColor.v = 255;
  CRGB rgbColor;
  hsv2rgb_rainbow(hsvColor, rgbColor);
  fill_solid(pixels, NUM_PIXELS + NUM_NONSYMMETRICAL_PIXELS, rgbColor);
  for (uint16_t i = 0; i < NUM_PIXELS; pixels[i++].nscale8_video(BG_INTENSITY_SCALE_FACTOR));
  FastLED.show();

  if (pp->startHue != pp->endHue) {
    if (stepDir) {
      if (++stepNum == pp->endHue) {
        stepDir = 0;
      }
    }
    else {
      if (--stepNum == pp->startHue) {
        stepDir = 1;
      }
    }
  }
}



void redWhiteAndBlue(void* patParams)
{
  static rwabParams_t* pp;

  if (patParams) {
    pp = (rwabParams_t*) patParams;

    CRGB rgbColor;
    uint16_t i;
    uint16_t j;
    uint16_t k;
    uint16_t pixelsPerSegment = pp->numPixelsPerSegment;
    uint16_t numSegments = NUM_PIXELS / pixelsPerSegment;
    rgbColor = CRGB::Red;
    k = 0;
    rgbColor.nscale8_video(BG_INTENSITY_SCALE_FACTOR);
    for (i = 0; i < numSegments; ++i) {
      for (j = 0; j < pixelsPerSegment; ++j) {
        pixels[i * pixelsPerSegment + j] = rgbColor;
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
      rgbColor.nscale8_video(BG_INTENSITY_SCALE_FACTOR);
    }
    return;
  }

  nextPatternUpdateMs = millis() + pp->delay;

  rotateCrgbRight(pixels, (uint16_t) NUM_PIXELS);
  FastLED.show();
}



void rainbow(void* patParams)
{
  static rainbowParams_t* pp;
  static uint8_t stepNum;
  static uint8_t delta;

  uint16_t i;

  if (patParams) {
    pp = (rainbowParams_t*) patParams;
    stepNum = 0;
    delta = (uint8_t) (PANEL_NUM_PIXELS > 256 ? 1 : (uint16_t) 256 / (uint16_t) PANEL_NUM_PIXELS);
    return;
  }

  nextPatternUpdateMs = millis() + pp->delay;

  // TODO:  need to handle > 255 pixels
  fill_rainbow(pixels, PANEL_NUM_PIXELS, stepNum, delta);
  replicatePixelPanels();

  for (i = 0; i < NUM_PIXELS; pixels[i++].nscale8_video(BG_INTENSITY_SCALE_FACTOR));

  FastLED.show();

  if (pp->directionDown) {
    ++stepNum;
  }
  else {
    --stepNum;
  }
}



void multiWave(void* patParams)
{
  static multiWaveParams_t* pp;
  static uint8_t numWaveforms;
  static HSVHue posHue[MAX_WAVEFORMS];
  static HSVHue negHue[MAX_WAVEFORMS];
  static uint16_t angleInterval[MAX_WAVEFORMS];
  static uint16_t i0[MAX_WAVEFORMS];
  static uint16_t delayCount[MAX_WAVEFORMS];
  static uint16_t lastDelay;

  uint8_t w;
  colorWaveParams_t* waveParam;
  uint16_t i;
  uint16_t j;
  uint16_t t;
  uint16_t y;
  CHSV hsvColor;
  CHSV hsvBlended;
  uint16_t amountOfOverlay;
  uint16_t valueSum;
  CRGB rgbColor;


  if (patParams) {
    pp = (multiWaveParams_t*) patParams;

    numWaveforms = 0;

    // Calculate and save the LED values for each wave at t = 0.    
    for (w = 0; w < MAX_WAVEFORMS; ++w)
    {
      waveParam = pp->waveParams + w;

      if (0 == waveParam->numWaves) {
        break;
      }
      ++numWaveforms;

      if (waveParam->randomHue) {
        selectRandomHue(&posHue[w], &negHue[w]);
      }
      else {
        posHue[w] = waveParam->posHue;
        negHue[w] = waveParam->negHue;
      }
  
      // We use 16 bits for the angle interval and "time" so that we have sufficient
      // resultion to fit a complete set of the requested number of waves.
      angleInterval[w] = 65535 / (pp->boundedByPanel ? PANEL_NUM_PIXELS : NUM_PIXELS) * abs(waveParam->numWaves);

      i0[w] = 0;
      
      delayCount[w] = waveParam->delay > 0 ? waveParam->delay : 1;
    }
    
    lastDelay = 1;

    return;
  }


  unsigned long now = millis();

  bool needToDisplay = false;
  uint16_t lowestDelayCount = 65535;
  for (w = 0; w < numWaveforms; ++w) {
    delayCount[w] -= lastDelay;
    if (0 == delayCount[w]) {
      delayCount[w] = pp->waveParams[w].delay > 0 ? pp->waveParams[w].delay : 1;
      needToDisplay = true;
      if (pp->waveParams[w].directionDown) {
        if (++i0[w] >= (pp->boundedByPanel ? PANEL_NUM_PIXELS : NUM_PIXELS)) {
          i0[w] = 0;
        }
      }
      else {
        if (0 == i0[w]--) {
          i0[w] = pp->boundedByPanel ? PANEL_NUM_PIXELS - 1 : NUM_PIXELS - 1;
        }
      }
    }
    // The lowest delay count is the number of ms after which we need to display again.
    if (delayCount[w] < lowestDelayCount) {
      lowestDelayCount = delayCount[w];
    }
  }
  nextPatternUpdateMs = now + lowestDelayCount;
  lastDelay = lowestDelayCount;

  if (!needToDisplay) {
    return;
  }

    
  for (i = 0; i < (pp->boundedByPanel ? PANEL_NUM_PIXELS : NUM_PIXELS); ++i) {

    for (w = 0; w < numWaveforms; ++w) {

      t = ((uint16_t) i + i0[w]) * angleInterval[w] / 256;
      switch (pp->waveParams[w].waveformType) {
        case SQUARE_WAVE:
          y = triwave8(t) >= 128 ? 255 : 0;
          break;
        case TRIANGLE_WAVE:
          y = triwave8(t);
          break;
        case QUADRATIC_EASING:
          y = quadwave8(t);
          break;
        case CUBIC_EASING:
          y = cubicwave8(t);
          break;
        case SINE_WAVE:
        default:
          y = sin8(t);
          break;
      }
      if (y >= 128) {
        hsvColor.h = posHue[w];
        hsvColor.v = (y - 128) * 2;
      }
      else {
        hsvColor.h = negHue[w];
        // A negative number of waves means make a half wave.
        hsvColor.v = (pp->waveParams[w].numWaves >= 0) ? (127 - y) * 2 : 0;
      }
      hsvColor.s = 255;
      
      if (0 == w) {
        hsvBlended = hsvColor;
      }
      else {
        amountOfOverlay = 255 * (uint16_t) hsvBlended.v / ((uint16_t) hsvColor.v + (uint16_t) hsvBlended.v);
        valueSum = qadd8(hsvBlended.v, hsvColor.v);
        nblend(hsvBlended, hsvColor, amountOfOverlay);
        hsvBlended.v = valueSum;
      }
    }
    
    hsv2rgb_rainbow(hsvBlended, pixels[i]);
    pixels[i].nscale8_video(BG_INTENSITY_SCALE_FACTOR);
  }

  if (pp->boundedByPanel) {
    replicatePixelPanels();
  }
  
  FastLED.show();
}



void movingDot(void* patParams)
{
  static movingDotParams_t* pp;
  static uint16_t stepNum;
  static uint8_t stepDir;    // 0 = away from pixel 0, 1 = toward pixel 0, 2 = hold at far end, 3 = hold at near end
  static CRGB fgColor;
  static CRGB bgColor;

  CRGB rgbColor;
  uint16_t prevStepNum;
  uint8_t prevStepDir;
  uint16_t j;

  if (patParams) {
    pp = (movingDotParams_t*) patParams;
    
    stepNum = 0;
    stepDir = 0;

    selectRandomRgb(&fgColor, &bgColor);
    if (!pp->randomFgColor) {
        fgColor = pp->fgColorCode;
    }
    if (!pp->randomBgColor) {
        bgColor = pp->bgColorCode;
    }
    fgColor.nscale8_video(BG_INTENSITY_SCALE_FACTOR);
    bgColor.nscale8_video(BG_INTENSITY_SCALE_FACTOR);

    if (!pp->zipperBg) {
      fill_solid(pixelArray, NUM_PIXELS, bgColor);
    }

    return;
  }


  unsigned long now = millis();

  prevStepNum = stepNum;
  prevStepDir = stepDir;
  
  switch (stepDir) {

    // moving away from pixel 0
    case 0:
      if (++stepNum >= PANEL_NUM_PIXELS) {
        stepDir = 2;
        stepNum = PANEL_NUM_PIXELS - 1;
        nextPatternUpdateMs = now + pp->holdTimeFarMs;
      }
      else {
        nextPatternUpdateMs = now + pp->delay0;
      }
      break;

    // moving toward pixel 0
    case 1:
      if (--stepNum == 0) {
        stepDir = 3;
      }
      nextPatternUpdateMs = now + pp->delay1;
      break;

    // held at far end
    case 2:
      if (pp->bidirectional) {
        stepDir = 1;
        stepNum = PANEL_NUM_PIXELS - 2;
      }
      else {
        stepDir = 0;
        stepNum = 0;
      }
      break;

    // held at near end
    case 3:
      stepDir = 0;
      nextPatternUpdateMs = now + pp->holdTimeNearMs;
      break;
  }

  if (0 == stepDir && 0 != prevStepDir && pp->changeFgColor) {
    if (pp->zipperBg) {
      // When doing the zipper effect, change the background color, too.
      selectRandomRgb(&fgColor, &bgColor);
      fgColor.nscale8_video(BG_INTENSITY_SCALE_FACTOR);
      bgColor.nscale8_video(BG_INTENSITY_SCALE_FACTOR);
    }
    else {
      selectRandomRgb(&fgColor, NULL);
      fgColor.nscale8_video(BG_INTENSITY_SCALE_FACTOR);
    }
  }

  rgbColor = ((stepDir == 1 || prevStepDir == 1) && pp->zipperBg) ? CRGB::Black : bgColor;
  pixels[prevStepNum] = rgbColor;
  for (j = 1; j < NUM_PANELS; ++j) {
    pixels[prevStepNum + PANEL_NUM_PIXELS * j] = rgbColor;
  }

  pixels[stepNum] = fgColor;
  for (j = 1; j < NUM_PANELS; ++j) {
    pixels[stepNum + PANEL_NUM_PIXELS * j] = fgColor;
  }
  
  FastLED.show();
}



void sparkle(void* patParams)
{
  static sparkleParams_t* pp;
  static uint8_t density;
  static uint16_t selectedPixels[MAX_SPARKLE_DENSITY];
  static CRGB fgColor;
  static CRGB bgColor;

  uint8_t i;
  
  if (patParams) {
    pp = (sparkleParams_t*) patParams;
    
    density = pp->density <= MAX_SPARKLE_DENSITY ? pp->density : MAX_SPARKLE_DENSITY;
    for (i = 0; i < density; ++i) {
      selectedPixels[i] = 0;
    }

    if (pp->fgColorCode != CRGB::Black || pp->bgColorCode != CRGB::Black) {
      fgColor = pp->fgColorCode;
      bgColor = pp->bgColorCode;
    }
    else {
      selectRandomRgb(&fgColor, &bgColor);
    }
    bgColor.nscale8_video(BG_INTENSITY_SCALE_FACTOR);
    
    // Fill with background color.
    fill_solid(pixels, NUM_PIXELS + NUM_NONSYMMETRICAL_PIXELS, bgColor);
    FastLED.show();
    return;
  }

  nextPatternUpdateMs = millis() + SPARKLE_DELAY;

  for (i = 0; i < density; ++i) {
    // Turn off the last sparkle pixel.
    pixels[selectedPixels[i]] = bgColor;
  
    // Turn on a random sparkle pixel.
    selectedPixels[i] = random16(NUM_PIXELS + NUM_NONSYMMETRICAL_PIXELS);
    pixels[selectedPixels[i]] = fgColor;
  }

  FastLED.show();
}



void splitRotation(void* patParams)
{
  static splitRotationParams_t* pp;
  
  uint16_t i;
  HSVHue fgHue;
  HSVHue bgHue;
  CHSV fgHsv;
  CHSV bgHsv;
  CRGB rgbColor;
  

  if (patParams) {
    pp = (splitRotationParams_t*) patParams;

    if (pp->randomHue) {
      selectRandomHue(&fgHue, &bgHue);
    }
    else {
      fgHue = pp->fgHue;
      bgHue = pp->bgHue;
    }
    fgHsv.h = fgHue;
    fgHsv.s = 255;
    fgHsv.v = 255;
    bgHsv.h = bgHue;
    bgHsv.s = 255;
    bgHsv.v = 255;

    // Fill with background color.
    hsv2rgb_rainbow(bgHsv, rgbColor);
    rgbColor.nscale8_video(BG_INTENSITY_SCALE_FACTOR);
    fill_solid(pixels, PANEL_NUM_PIXELS, rgbColor);

    // Set every nth pixel to the foreground color.
    hsv2rgb_rainbow(fgHsv, rgbColor);
    rgbColor.nscale8_video(BG_INTENSITY_SCALE_FACTOR);
    for (i = 0; i < PANEL_NUM_PIXELS; i += pp->fgInterval) {
        pixels[i] = rgbColor;
    }

    replicatePixelPanels();

    FastLED.show();

    return;
  }


  nextPatternUpdateMs = millis() + pp->delay;

  i = PANEL_NUM_PIXELS / 4;
  if (pp->directionDown) {
    rotateCrgbLeft(pixels, i);
    rotateCrgbRight(pixels + i, i);
    rotateCrgbRight(pixels + i * 3, i);
    rotateCrgbLeft(pixels + i * 2, i);
  }
  else {
    rotateCrgbRight(pixels, i);
    rotateCrgbLeft(pixels + i, i);
    rotateCrgbLeft(pixels + i * 3, i);
    rotateCrgbRight(pixels + i * 2, i);
  }

  replicatePixelPanels();

  FastLED.show();
}



/***********
 * Helpers *
 ***********/


void selectRandomRgb(CRGB* rgbColor, CRGB* rgbColorInverse)
{
  CHSV hsvColor;

  // Choose a fully saturated random color.
  hsvColor.h = random(256);
  hsvColor.s = 255;
  hsvColor.v = 255;
  hsv2rgb_rainbow(hsvColor, *rgbColor);
  
  if (NULL != rgbColorInverse) {
    switch (random(6)) {
      case 0:
        hsvColor.h += 64;    // 1/4 the way around the color wheel
        break;
      case 1:
        hsvColor.h += 85;    // 1/3 the way around the color wheel
        break;
      case 2:
        hsvColor.h -= 64;    // 1/4 the other way around the color wheel
        break;
      case 3:
        hsvColor.h -= 85;    // 1/3 the other way around the color wheel
        break;
      default:
        hsvColor.h += 128;   // complementary color
        break;
    }
    hsv2rgb_rainbow(hsvColor, *rgbColorInverse);
  }
}


void selectRandomHue(HSVHue* hue, HSVHue* hueInverse)
{
  uint8_t hueValue;

  // Choose a fully saturated random color.
  hueValue = random(256);
  *hue = (HSVHue) hueValue;
  
  if (NULL != hueInverse) {
    switch (random(6)) {
      case 0:
        hueValue += 64;    // 1/4 the way around the color wheel
        break;
      case 1:
        hueValue += 85;    // 1/3 the way around the color wheel
        break;
      case 2:
        hueValue -= 64;    // 1/4 the other way around the color wheel
        break;
      case 3:
        hueValue -= 85;    // 1/3 the other way around the color wheel
        break;
      default:
        hueValue += 128;   // complementary color
        break;
    }
    *hueInverse = (HSVHue) hueValue;
  }
}


void rotateCrgbRight(CRGB* a, uint16_t length)
{
  uint16_t i;
  CRGB temp;

  temp = a[length - 1];

  for (i = length - 1; i > 0; --i) {
    a[i] = a[i - 1];
  }

  a[0] = temp;
}


void rotateCrgbLeft(CRGB* a, uint16_t length)
{
  uint16_t i;
  CRGB temp;

  temp = a[0];

  for (i = 0; i < length - 1; ++i) {
    a[i] = a[i + 1];
  }

  a[length - 1] = temp;
}


void replicatePixelPanels()
{
  if (NUM_PANELS <= 1)
    return;
  
  uint16_t i;
  uint16_t j;
  
  for (i = 0; i < PANEL_NUM_PIXELS; ++i) {
    for (j = 1; j < NUM_PANELS; ++j) {
      pixels[i + PANEL_NUM_PIXELS * j] = pixels[i];
    }
  }
}


#ifdef ENABLE_DMX_AMBIENCE_LIGHTS
void replicateDMXPanels()
{
  if (NUM_DMX_PANELS <= 1)
    return;
  
  uint8_t i;
  uint8_t j;
  
  for (i = 0; i < DMX_PANEL_NUM_RGB_CHANNELS; ++i) {
    for (j = 1; j < NUM_DMX_PANELS; ++j) {
      dmxRgbArray[i + DMX_PANEL_NUM_RGB_CHANNELS * j] = dmxRgbArray[i];
    }
  }
}
#endif


uint16_t freeRam() 
{
  // Based on code retrieved on 1 April 2015 from
  // https://learn.adafruit.com/memories-of-an-arduino/measuring-free-memory

  extern int __heap_start, *__brkval; 
  int v; 
  return (uint16_t) &v - (__brkval == 0 ? (uint16_t) &__heap_start : (uint16_t) __brkval); 
}


void readPatternDefinitionFromFlash(
  uint8_t patternNum,
  uint8_t* pPatternType,
  unsigned long* pDurationSec,
  void** pPatParams)
{
  const patternDef_t* patDef = patternsDefs + patternNum;

  if (pPatternType) {
    uint8_t patternType = (uint8_t) pgm_read_byte(&patDef->patternType);
    *pPatternType = patternType;
  }

  if (pDurationSec) {
    unsigned long durationSec = (unsigned long) pgm_read_dword(&patDef->durationSec);
    *pDurationSec = durationSec;
  }

  if (pPatParams) {
    void* patParams = (void*) pgm_read_word(&patDef->patParams);
    *pPatParams = patParams;
  }
}


void initEepromValues()
{
  uint8_t currentEepromVersion = EEPROM.read(EEPROM_ADDR_VERSION);
  if (EEPROM_VERSION != currentEepromVersion) {
    EEPROM.write(EEPROM_ADDR_VERSION, EEPROM_VERSION);
    EEPROM.write(EEPROM_ADDR_PATTERN_SELECTION, 255);
  }
}


void displayFreeMemory(void)
{
  // Using the pixels, display the relative amount of free RAM.
  uint32_t numFreeBytes = freeRam();
  //Serial.println(numFreeBytes);
  uint32_t memDisplayNumPixelsOn = numFreeBytes * NUM_PIXELS / 2048;
  fill_solid(pixels, memDisplayNumPixelsOn, CRGB::Cyan);
  for (uint16_t i = 0; i < memDisplayNumPixelsOn; pixels[i++].nscale8_video(BG_INTENSITY_SCALE_FACTOR));
  FastLED.show();
  delay(2000);
  fill_solid(pixels, NUM_PIXELS, CRGB::Black);
  FastLED.show();
}



/***********************************
 * DMX Ambiance Lighting Functions *
 ***********************************/

#ifdef ENABLE_DMX_AMBIENCE_LIGHTS


void dmxAmbianceRainbowFade(bool doInit)
{
  static uint8_t stepNum;
  static unsigned long nextStepMs;

  CRGB rgbColor;
  CHSV hsvColor;
  uint8_t i;

  if (doInit) {
    stepNum = 0;
    return;
  }

  if (millis() < nextStepMs)
    return;

  // Step all channels in the panel together through the rainbow (Dr. Naked's suggestion).
  hsvColor.h = stepNum;
  hsvColor.s = 255;
  hsvColor.v = 255;
  hsv2rgb_rainbow(hsvColor, rgbColor);
  fill_solid(dmxRgbArray, DMX_PANEL_NUM_RGB_CHANNELS, rgbColor);

  for (i = 0; i < DMX_PANEL_NUM_RGB_CHANNELS; dmxRgbArray[i++].nscale8_video(DMX_AMBIANCE_INTENSITY_SCALE_FACTOR));
  replicateDMXPanels();
  
  // We don't call FastLED.show() here.  The DMX LEDs will get updated
  // when the pixels get updated, which will be soon enough.

  ++stepNum;

  nextStepMs = millis() + 200;
}


void dmxAmbianceRainbowRotate(bool doInit)
{
  static uint8_t stepNum;
  static unsigned long nextStepMs;

  CRGB rgbColor;
  CHSV hsvColor;
  uint8_t i;

  if (doInit) {
    stepNum = 0;
    return;
  }

  if (millis() < nextStepMs)
    return;

  // Rotate rainbow through all the channels in the panel.
  fill_rainbow(dmxRgbArray, DMX_PANEL_NUM_RGB_CHANNELS, stepNum, 255 / DMX_PANEL_NUM_RGB_CHANNELS);

  for (i = 0; i < DMX_PANEL_NUM_RGB_CHANNELS; dmxRgbArray[i++].nscale8_video(DMX_AMBIANCE_INTENSITY_SCALE_FACTOR));
  replicateDMXPanels();
  
  // We don't call FastLED.show() here.  The DMX LEDs will get updated
  // when the pixels get updated, which will be soon enough.

  ++stepNum;

  nextStepMs = millis() + 200;
}


void dmxAmbianceRedGreenWave(bool doInit)
{
  static uint8_t stepNum;
  static unsigned long nextStepMs;
  static float ledAngleInterval;

  if (doInit) {
    stepNum = 0;
    ledAngleInterval = TWO_PI / (float) DMX_PANEL_NUM_RGB_CHANNELS;
    return;
  }

  if (millis() < nextStepMs)
    return;

  CRGB rgbColor;
  CHSV hsvColor;

  hsvColor.s = 255;

  // rotating 1/2 blue, 1/2 orange
  float t = (float) stepNum / 255.0 * TWO_PI;
  for (uint16_t i = 0; i < DMX_PANEL_NUM_RGB_CHANNELS; ++i) {
    float y = sin(t + ledAngleInterval * i);
    if (y >= 0) {
      hsvColor.h = HUE_RED;
      hsvColor.v = (float) 255 * y;
    }
    else {
      hsvColor.h = HUE_GREEN;
      hsvColor.v = - (float) 255 * y;
    }
    hsv2rgb_rainbow(hsvColor, rgbColor);
    dmxRgbArray[i] = rgbColor;
  }

  for (uint16_t i = 0; i < DMX_PANEL_NUM_RGB_CHANNELS; dmxRgbArray[i++].nscale8_video(DMX_AMBIANCE_INTENSITY_SCALE_FACTOR));

  // We don't call FastLED.show() here.  The DMX LEDs will get updated
  // when the pixels get updated, which will be soon enough.

  ++stepNum;

  nextStepMs = millis() + 200;
}

#endif  // #ifdef ENABLE_DMX_AMBIENCE_LIGHTS



/***********************
 * Setup and Main Loop *
 ***********************/


void setup()
{
  // Let everything settle down after a rough power-up.
  delay(100);

  initEepromValues();
  
  pinMode(MODE_PUSHBUTTON_PIN, INPUT_PULLUP);

  pinMode(GND_PIN, OUTPUT);
  digitalWrite(GND_PIN, LOW);
  
#if defined(ROZANNS_PATIO_LIGHTS)
  FastLED.addLeds<WS2812B, PIXEL_DATA_PIN, BRG>(pixelArray, NUM_PHYSICAL_PIXELS);  // 10 pixels/m 12 V white strips from HolidayCoro 11/2015
#elif defined(HOUSE_PATIO_AND_SOFFIT_LIGHTS)
  FastLED.addLeds<WS2812B, PIXEL_DATA_PIN, RGB>(pixelArray, NUM_PHYSICAL_PIXELS);  // 5 V pixel strings, pixel strawberry strings
#elif defined(SANTA_HAT) || defined(DEVELOPMENT_STRIP)
  FastLED.addLeds<WS2812B, PIXEL_DATA_PIN, GRB>(pixelArray, NUM_PHYSICAL_PIXELS); // 60/144 pixels/m 5 V white strips (devel. strip, lampshade hat) 
#endif

#ifdef ENABLE_DMX_AMBIENCE_LIGHTS
  FastLED.addLeds<DMXSIMPLE, DMX_DATA_PIN, RGB>(dmxRgbArray, NUM_DMX_RGB_CHANNELS);

  // Turn off all the DMX LEDs.
  fill_solid(dmxRgbArray, NUM_DMX_RGB_CHANNELS, CRGB::Black);
#endif

  // Initialize the pixel array so that any skipped pixels will remain off.
  fill_solid(pixelArray, NUM_PIXELS, CRGB::Black);

  // Analog input 0 should be disconnected, making it a good source of
  // random noise with which we can seed the random number generator.
  randomSeed(analogRead(0));  

  //Serial.begin(9600);
  //Serial.println("Starting");

  //displayFreeMemory();

#ifdef ENABLE_DMX_AMBIENCE_LIGHTS
  //dmxAmbianceRainbowRotate(true);
  dmxAmbianceRedGreenWave(true);
#endif
}


void loop()
{
  static uint8_t patternNum = 255;
  static unsigned long nextPatternChangeMs = 0;
  static void (*loopFun)(void*);
  static byte patParamBuf[PAT_PARAMS_BUF_SIZE];  // used by pattern fn; must remain intact between calls to same fn
  static bool loopedWithoutPatternUpdate;
  static unsigned long lastPushbuttonEventMs = 0;
  static bool lastPushbuttonState = HIGH;
  static bool pushbuttonDebouncedState = HIGH;

  uint8_t patternSelection;
  uint8_t paramStructSize;
  unsigned long now = millis();


  // ----- pushbutton support -----

//  digitalWrite(ONBOARD_LED_PIN, pushbuttonDebouncedState);

  bool pushbuttonState = digitalRead(MODE_PUSHBUTTON_PIN);
  if (pushbuttonState != lastPushbuttonState) {
    lastPushbuttonState = pushbuttonState;
    lastPushbuttonEventMs = now;
  }

  if (pushbuttonState != pushbuttonDebouncedState
        && now - lastPushbuttonEventMs > PUSHBUTTON_DEBOUNCE_INTERVAL_MS) {
      pushbuttonDebouncedState = pushbuttonState;

      if (pushbuttonDebouncedState == LOW) {

        // The pushbutton has been pressed.  Change the pattern.

        patternSelection = EEPROM.read(EEPROM_ADDR_PATTERN_SELECTION);
        // If currently in auto mode, start the manual selection just after the current pattern.
        if (255 == patternSelection) {
          patternSelection = patternNum;
        }
        ++patternSelection;
        // Skip over the end-of-auto-patterns marker.
        if (patternSelection < NUM_PATTERNS) {
          uint8_t patternType;
          readPatternDefinitionFromFlash(patternSelection, &patternType, (unsigned long*) 0, (void**) 0);
          if (END_OF_AUTO_PATTERNS == patternType) {
            ++patternSelection;
          }
        }

        // Enter auto mode if past last pattern.
        if (patternSelection >= NUM_PATTERNS) {
          patternSelection = 255;
          patternNum = 255;
          // Flash the entire string white so that the buttonpusher knows we're back on the show.
          CRGB rgbColor = CRGB::White;
          rgbColor.nscale8_video(BG_INTENSITY_SCALE_FACTOR);
          fill_solid(pixels, NUM_PIXELS, rgbColor);
          FastLED.show();
          delay(150);
        }

        // Save the pattern selection so that the same pattern is displayed after a power cycle.
        EEPROM.write(EEPROM_ADDR_PATTERN_SELECTION, patternSelection);

        nextPatternChangeMs = now;
      }
  }


#ifdef ENABLE_DMX_AMBIENCE_LIGHTS
  //dmxAmbianceRainbow(false);
  dmxAmbianceRedGreenWave(false);
#endif


  if (now < nextPatternChangeMs) {
    
    if (now >= nextPatternUpdateMs && loopFun != 0) {

      // If loop has been called at least once without having to call loopFun
      // then we are probably keeping up with the update frequency that the
      // pattern wants.  However, if we are calling loopFun every time loop
      // is called, we probably can't satisfy the pattern's timing.  In that
      // case, turn on the onboard LED to indicate that there is a problem.
      digitalWrite(ONBOARD_LED_PIN, !loopedWithoutPatternUpdate);
      loopedWithoutPatternUpdate = false;
      
      (*loopFun)(NULL);
    }
    else {
      loopedWithoutPatternUpdate = true;
    }
  }
  
  else {

    // Time to change the pattern if in auto (show) mode, or
    // need to change pattern in response to button push.
    
    // If we are in auto mode, go to the next pattern.
    patternSelection = EEPROM.read(EEPROM_ADDR_PATTERN_SELECTION);
    if (patternSelection == 255) {
      if (++patternNum >= NUM_PATTERNS) {
        patternNum = 0;
      }
    }
    else {
      patternNum = patternSelection;
    }

    uint8_t patternType;
    unsigned long durationSec;
    void* patParams;
    readPatternDefinitionFromFlash(patternNum, &patternType, &durationSec, &patParams);

    // Skip over the end-of-auto-patterns marker.
    if (END_OF_AUTO_PATTERNS == patternType) {
      ++patternNum;
      return;
    }
    
    // Get the pattern metadata.
    paramStructSize = patternMetadataTable[patternType].paramStructSize;
    loopFun = patternMetadataTable[patternType].loopFun;

    // If the pattern params buffer isn't large enough, turn all
    // pixels red for one second then go on to the next pattern.
    if (paramStructSize > PAT_PARAMS_BUF_SIZE) {
      CRGB rgbColor = CRGB::Red;
      rgbColor.nscale8_video(BG_INTENSITY_SCALE_FACTOR);
      fill_solid(pixels, NUM_PIXELS, rgbColor);
      FastLED.show();
      loopFun = 0;
      nextPatternChangeMs = millis() + 1000;
      return;
    }

    // Get the pattern parameters from flash.    
    memcpy_P(patParamBuf, patParams, paramStructSize);

    // When a pattern is explicitly selected, it should run forever.
    nextPatternChangeMs = (patternSelection != 255) ? -1 : millis() + durationSec * 1000;
    
    // Turn off all the pixels, including skipped and non-pattern pixels.
    fill_solid(pixelArray, NUM_PHYSICAL_PIXELS, CRGB::Black);
    FastLED.show();

    // Let the pattern initialize itself.
    (*loopFun)((void*) patParamBuf);
    
    // Give the pattern a chance to draw itself right away.
    nextPatternUpdateMs = millis() - 1;
    
    // Blip the can't-keep-up light.
    loopedWithoutPatternUpdate = false;
  }  
}

