/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Rainbow Pattern                                                 *
 *                                                                 *
 * by Ross Butler, Dec. 2015                                       *
 * (initial implementation created for EEkur Pyramid               *
 * at Esthetic Evolution 2014)                                     *
 *                                                                 *
 *******************************************************************/

#ifndef __RAINBOW_H
#define __RAINBOW_H

#include "PixelPattern.h"


namespace pixelPattern {

class Rainbow : public PixelPattern {

public:

    static constexpr uint8_t id = 4;

    struct PatternConfig {
        bool     directionDown;     // true to rotate one way, false to rotate the other way
        // TODO:  Need to change delay to be the period of a full rotation
        uint32_t delayMs;           // delay between rotation steps
    };

    Rainbow() {};
    ~Rainbow() {};
        
    Rainbow(const Rainbow&) = delete;
    Rainbow& operator =(const Rainbow&) = delete;

    bool initPattern(bool configIsInFlash, void* patternConfig);
    bool update();

private:

    PatternConfig config;
    uint8_t stepNum;
    uint8_t delta;
};

}

#endif  // #ifndef __RAINBOW_H

