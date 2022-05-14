/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Helper Functions for Patterns                                   *
 *                                                                 *
 * by Ross Butler   Dec. 2015                                      *
 *                                                                 *
 *******************************************************************/

#include "patternHelpers.h"
#include "FastLED.h"
#include "PixelSet.h"


namespace pixelPattern {


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


void replicatePixelPanels(PixelSet* pixelSet)
{
  if (pixelSet->numPanels <= 1)
    return;
  
  uint16_t i;
  uint16_t j;
  
  for (i = 0; i < pixelSet->numPanelPixels; ++i) {
    for (j = 1; j < pixelSet->numPanels; ++j) {
      pixelSet->pixels[i + pixelSet->numPanelPixels * j] = pixelSet->pixels[i];
    }
  }
}


}

