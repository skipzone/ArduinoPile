/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Pixel Pattern Interface and Common Data                         *
 *                                                                 *
 * by Ross Butler   Dec. 2015                                      *
 *                                                                 *
 *******************************************************************/

#ifndef __PIXEL_PATTERN_H
#define __PIXEL_PATTERN_H

#include <stdint.h>
#include "FastLED.h"
#include "PixelSet.h"


namespace pixelPattern {

class PixelPattern {

public:

    PixelPattern() {}
    virtual ~PixelPattern() {}

    PixelPattern(const PixelPattern&) = delete;
    PixelPattern& operator =(const PixelPattern&) = delete;

    bool init(bool configIsInFlash, void* patternConfig, PixelSet* pixelSet) {
        this->pixelSet = pixelSet;
        return initPattern(configIsInFlash, patternConfig);
    }

    virtual bool initPattern(bool configIsInFlash, void* patternConfig) = 0;
    virtual bool update() = 0;

    uint32_t nextUpdateMs;

protected:

    PixelSet* pixelSet;

private:

};

}

#endif  // #ifndef __PIXEL_PATTERN_H
