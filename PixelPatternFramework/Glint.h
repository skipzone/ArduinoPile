/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Glint Pattern                                                   *
 *                                                                 *
 * by Ross Butler, Dec. 2015                                       *
 * (initial implemntation created for Drew's Diamond, July 2015)   *
 *                                                                 *
 *******************************************************************/

#ifndef __GLINT_H
#define __GLINT_H

#include "PixelPattern.h"


namespace pixelPattern {

class Glint : public PixelPattern {

public:

    static constexpr uint8_t id = 5;

    struct PatternConfig {
        HSVHue  bgHue;              // background hue
        uint32_t glintIntervalMs;   // interval between glints
        // TODO:  express delay as the period of a complete glint
        uint32_t delayMs;           // ms delay between glint steps
    };

    Glint() {};
    ~Glint() {};
        
    Glint(const Glint&) = delete;
    Glint& operator =(const Glint&) = delete;

    bool initPattern(bool configIsInFlash, void* patternConfig);
    bool update();

private:

    PatternConfig config;
    uint32_t nextGlintMs;
    bool doingGlint = false;
    //uint8_t glintStepNum;
    float n0;
    float s;
    float w;
    float ns;
};

}

#endif  // #ifndef __GLINT_H

