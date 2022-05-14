/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Moving Dot Pattern                                              *
 *                                                                 *
 * by Ross Butler, Dec. 2015                                       *
 * (initial implemntation created for John's Kaleidoscope Mirror,  *
 * Nov. 2014)                                                      *
 *                                                                 *
 *******************************************************************/

#ifndef __MOVING_DOT_H
#define __MOVING_DOT_H

#include "PixelPattern.h"


namespace pixelPattern {

class MovingDot : public PixelPattern {

public:

    static constexpr uint8_t id = 6;

    struct PatternConfig {
        CRGB::HTMLColorCode fgColorCode;    // dot color
        CRGB::HTMLColorCode bgColorCode;    // background color
        bool                randomFgColor;  // true if the foreground color should be selected at random
        bool                randomBgColor;  // true if the background color should be selected at random
        bool                changeFgColor;  // true if the foreground color should be re-selected at random for each movement cycle
        bool                zipperBg;       // zipper effect:  set color when moving away from pixel 0, clear when moving other way
        bool                bidirectional;  // true if the dot should bounce back and forth
        uint32_t            delay0Ms;       // delay between dot steps when moving away from pixel 0
        uint32_t            delay1Ms;       // delay between dot steps when moving toward pixel 0
        uint32_t            holdTimeFarMs;  // how long to hold the dot at the far end before it moves back toward pixel 0
        uint32_t            holdTimeNearMs; // how long to hold the dot at the near end before it moves toward the last pixel
    };

    MovingDot() {};
    ~MovingDot() {};
        
    MovingDot(const MovingDot&) = delete;
    MovingDot& operator =(const MovingDot&) = delete;

    bool initPattern(bool configIsInFlash, void* patternConfig);
    bool update();

private:

    PatternConfig config;
    uint16_t stepNum;
    uint8_t stepDir;    // 0 = away from pixel 0, 1 = toward pixel 0, 2 = hold at far end, 3 = hold at near end
    CRGB fgColor;
    CRGB bgColor;
};

}

#endif  // #ifndef __MOVING_DOT_H

