/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Pixel Set Class                                                 *
 *                                                                 *
 * by Ross Butler   Dec. 2015                                      *
 *                                                                 *
 *******************************************************************/

#include "PixelSet.h"
#include "FastLED.h"

using namespace pixelPattern;


PixelSet::PixelSet(
    CRGB* allPixels,
    uint16_t numPhysicalPixels,
    uint16_t numSkipPixels,
    uint16_t numNonsymmetricalPixels,
    uint8_t numPanels,
    uint16_t numPanelPixels,
    uint8_t foregroundIntensityScaleFactor,
    uint8_t backgroundIntensityScaleFactor)
    :
    allPixels(allPixels),
    numPhysicalPixels(numPhysicalPixels),
    numSkipPixels(numSkipPixels),
    numNonsymmetricalPixels(numNonsymmetricalPixels),
    numPanels(numPanels),
    numPanelPixels(numPanelPixels),
    foregroundIntensityScaleFactor(foregroundIntensityScaleFactor),
    backgroundIntensityScaleFactor(backgroundIntensityScaleFactor),
    numPixels(numPhysicalPixels - numSkipPixels),
    numSymmetricalPixels(numPhysicalPixels - numSkipPixels - numNonsymmetricalPixels)
{
    pixels = allPixels + numSkipPixels;
}

