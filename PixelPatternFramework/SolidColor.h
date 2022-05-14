/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Solid Color Pattern                                             *
 *                                                                 *
 * by Ross Butler   Dec. 2015                                      *
 *                                                                 *
 *******************************************************************/

#ifndef __SOLID_COLOR_H
#define __SOLID_COLOR_H

#include "PixelPattern.h"


namespace pixelPattern {

class SolidColor : public PixelPattern {

public:

    static constexpr uint8_t id = 1;

    struct PatternConfig {
      HSVHue   startHue;      // starting hue
      HSVHue   endHue;        // end hue
      uint8_t  saturation;    // the S in HSV
      uint32_t delay;         // ms delay between rotation steps
    };

    SolidColor() {}
    ~SolidColor() {}
        
    SolidColor(const SolidColor&) = delete;
    SolidColor& operator =(const SolidColor&) = delete;

    bool initPattern(bool configIsInFlash, void* patternConfig);
    bool update();

private:

    PatternConfig config;
    uint8_t stepNum;
    uint8_t stepDir;  // non-zero for start->end, zero for end->start

};

}

#endif  // #ifndef __SOLID_COLOR_H

