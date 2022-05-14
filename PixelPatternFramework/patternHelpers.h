/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Helper Functions for Patterns                                   *
 *                                                                 *
 * by Ross Butler   Dec. 2015                                      *
 *                                                                 *
 *******************************************************************/

#ifndef __PATTERN_HELPERS_H
#define __PATTERN_HELPERS_H

#include <stdint.h>
#include <FastLED.h>


namespace pixelPattern {

class PixelSet;

// TODO:  change pointers to references where possible
void selectRandomRgb(CRGB* rgbColor, CRGB* rgbColorInverse);
void selectRandomHue(HSVHue* hue, HSVHue* hueInverse);
void rotateCrgbRight(CRGB* a, uint16_t length);
void rotateCrgbLeft(CRGB* a, uint16_t length);
void replicatePixelPanels(PixelSet* pixelSet);

}

#endif  // #ifdef __PATTERN_HELPERS_H

