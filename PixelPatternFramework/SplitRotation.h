/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Split Rotation Pattern                                          *
 *                                                                 *
 * by Ross Butler, Dec. 2015                                       *
 * (initial implementation created for EEkur Pyramid               *
 * at Esthetic Evolution 2014)                                     *
 *                                                                 *
 *******************************************************************/

#ifndef __SPLIT_ROTATION_H
#define __SPLIT_ROTATION_H

#include "PixelPattern.h"
#include "patternHelpers.h"


namespace pixelPattern {

class SplitRotation : public PixelPattern {

public:

    static constexpr uint8_t id = 7;

    enum Mode {
        original,
        symmetrical
    };

    struct PatternConfig {
        Mode     mode;              
        bool     randomHue;         // true if the hues should be selected at random
        HSVHue   fgHue;             // foreground (moving pixels) hue
        HSVHue   bgHue;             // background hue
        uint8_t  fgInterval;        // number of background pixels minus one between each moving pixel
        bool     directionDown;     // true to rotate one way, false to rotate the other way
        uint32_t delayMs;           // delay between rotation steps
    };

    SplitRotation() {};
    ~SplitRotation() {};
        
    SplitRotation(const SplitRotation&) = delete;
    SplitRotation& operator =(const SplitRotation&) = delete;

    bool initPattern(bool configIsInFlash, void* patternConfig);
    bool update();

private:

    PatternConfig config;

};

}

#endif  // #ifndef __SPLIT_ROTATION_H

