/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Sparkle Pattern                                                 *
 *                                                                 *
 * by Ross Butler, Dec. 2015                                       *
 * (initial implementation created for EEkur Pyramid               *
 * at Esthetic Evolution 2014)                                     *
 *                                                                 *
 *******************************************************************/

#ifndef __SPARKLE_H
#define __SPARKLE_H

#include "PixelPattern.h"


namespace pixelPattern {

class Sparkle : public PixelPattern {

public:

    static constexpr uint8_t id = 2;

    struct PatternConfig {
        CRGB::HTMLColorCode fgColorCode;    // color code of the sparkle color (set both fg and bg to black for random colors)
        CRGB::HTMLColorCode bgColorCode;    // color code of the background color
        uint8_t             density;        // number of foreground (sparkle) pixels on simultaneously
        uint32_t            dwellMs;        // period that a set of sparkles is on (lit)
        uint32_t            changeMs;       // period between sparkle sets (changes)
    };

    Sparkle()
        :
        selectedPixels(0)
        {}

    ~Sparkle() { delete [] selectedPixels; }
        
    Sparkle(const Sparkle&) = delete;
    Sparkle& operator =(const Sparkle&) = delete;

    bool initPattern(bool configIsInFlash, void* patternConfig);
    bool update();

private:

    PatternConfig config;
    CRGB fgColor;
    CRGB bgColor;
    uint16_t *selectedPixels;
    bool sparklesAreOn;
    uint32_t sparkleSetOnAtMs;

};

}

#endif  // #ifndef __SPARKLE_H

