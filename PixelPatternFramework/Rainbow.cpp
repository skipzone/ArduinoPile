/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Rainbow Pattern                                            *
 *                                                                 *
 * by Ross Butler, Dec. 2015                                       *
 * (initial implementation created for EEkur Pyramid               *
 * at Esthetic Evolution 2014)                                     *
 *                                                                 *
 *******************************************************************/

#include "Rainbow.h"
#include "patternHelpers.h"

using namespace pixelPattern;


bool Rainbow::initPattern(bool configIsInFlash, void* patternConfig)
{
    // Returns true if successful, false if failed or pattern cannot run.

    if (configIsInFlash) {
        memcpy_P(&config, patternConfig, sizeof(PatternConfig));
    }
    else {
        memcpy(&config, patternConfig, sizeof(PatternConfig));
    }

    stepNum = 0;
    delta = (uint8_t) (pixelSet->numPanelPixels > 256 ? 1 : (uint16_t) 256 / pixelSet->numPanelPixels);

    // We need update() to be called as soon as possible.
    nextUpdateMs = millis() - 1;

    return true;
}


bool Rainbow::update()
{
    nextUpdateMs = millis() + config.delayMs;

    // TODO:  need to make sure we can handle panels with more than 255 pixels
    fill_rainbow(pixelSet->pixels, pixelSet->numPanelPixels, stepNum, delta);
    replicatePixelPanels(pixelSet);

    // TODO:  use a helper for this
    for (uint16_t i = 0; i < pixelSet->numPixels; pixelSet->pixels[i++].nscale8_video(pixelSet->backgroundIntensityScaleFactor));

    if (config.directionDown) {
        ++stepNum;
    }
    else {
        --stepNum;
    }

    // Return true to request write to the LEDs.
    return true;
}

