/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Solid Color Pattern                                             *
 *                                                                 *
 * by Ross Butler   Dec. 2015                                      *
 *                                                                 *
 *******************************************************************/

#include "SolidColor.h"

using namespace pixelPattern;


bool SolidColor::initPattern(bool configIsInFlash, void* patternConfig)
{
    // Returns true because this pattern can always run.

    if (configIsInFlash) {
        memcpy_P(&config, patternConfig, sizeof(PatternConfig));
    }
    else {
        memcpy(&config, patternConfig, sizeof(PatternConfig));
    }

    stepNum = config.startHue;
    stepDir = 1;

    // We need update() to be called as soon as possible.
    nextUpdateMs = millis() - 1;

    return true;
}


bool SolidColor::update()
{
    nextUpdateMs = millis() + config.delay;

    CHSV hsvColor;
    hsvColor.h = stepNum;
    hsvColor.s = config.saturation;
    hsvColor.v = 255;

    CRGB rgbColor;
    hsv2rgb_rainbow(hsvColor, rgbColor);

    fill_solid(pixelSet->pixels, pixelSet->numPixels, rgbColor);

    // TODO:  use a helper for this
    for (uint16_t i = 0; i < pixelSet->numPixels; pixelSet->pixels[i++].nscale8_video(pixelSet->backgroundIntensityScaleFactor));

    if (config.startHue != config.endHue) {
        if (stepDir) {
            if (++stepNum == config.endHue) {
                stepDir = 0;
            }
        }
        else {
            if (--stepNum == config.startHue) {
                stepDir = 1;
            }
        }
    }

    return true;
}

