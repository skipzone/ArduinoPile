/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Pixel Set Class                                                 *
 *                                                                 *
 * by Ross Butler   Dec. 2015                                      *
 *                                                                 *
 *******************************************************************/

#ifndef __PIXEL_SET_H
#define __PIXEL_SET_H

#include <stdint.h>


class CRGB;


namespace pixelPattern {

class PixelSet {

public:

    PixelSet(
        CRGB* allPixels,
        uint16_t numPhysicalPixels,
        uint16_t numSkipPixels,
        uint16_t numNonsymmetricalPixels,
        uint8_t numPanels,
        uint16_t numPanelPixels,
        uint8_t foregroundIntensityScaleFactor,
        uint8_t backgroundIntensityScaleFactor);

    ~PixelSet() {}

    PixelSet(const PixelSet&) = delete;
    PixelSet& operator =(const PixelSet&) = delete;

    CRGB* allPixels;
    uint16_t numPhysicalPixels;
    uint16_t numSkipPixels;
    uint16_t numNonsymmetricalPixels;
    uint8_t numPanels;
    uint16_t numPanelPixels;
    uint8_t foregroundIntensityScaleFactor;
    uint8_t backgroundIntensityScaleFactor;
    CRGB* pixels;
    uint16_t numPixels;
    uint16_t numSymmetricalPixels;
};

}

#endif  // #ifndef __PIXEL_SET_H

