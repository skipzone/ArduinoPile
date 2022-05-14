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

#include "MovingDot.h"
#include "patternHelpers.h"


using namespace pixelPattern;


bool MovingDot::initPattern(bool configIsInFlash, void* patternConfig)
{
    // Returns true if successful, false if failed or pattern cannot run.

    if (configIsInFlash) {
        memcpy_P(&config, patternConfig, sizeof(PatternConfig));
    }
    else {
        memcpy(&config, patternConfig, sizeof(PatternConfig));
    }

    stepNum = 0;
    stepDir = 0;

    selectRandomRgb(&fgColor, &bgColor);
    if (!config.randomFgColor) {
        fgColor = config.fgColorCode;
    }
    if (!config.randomBgColor) {
        bgColor = config.bgColorCode;
    }
    fgColor.nscale8_video(pixelSet->backgroundIntensityScaleFactor);
    bgColor.nscale8_video(pixelSet->backgroundIntensityScaleFactor);

    if (!config.zipperBg) {
        fill_solid(pixelSet->pixels, pixelSet->numPixels, bgColor);
    }

    // We need update() to be called as soon as possible.
    nextUpdateMs = millis() - 1;

    return true;
}


bool MovingDot::update()
{
    uint32_t now = millis();

    uint16_t prevStepNum = stepNum;
    uint8_t prevStepDir = stepDir;

    switch (stepDir) {

        // moving away from pixel 0
        case 0:
            if (++stepNum >= pixelSet->numPanelPixels) {
                stepDir = 2;
                stepNum = pixelSet->numPanelPixels - 1;
                nextUpdateMs = now + config.holdTimeFarMs;
            }
            else {
                nextUpdateMs = now + config.delay0Ms;
            }
            break;

        // moving toward pixel 0
        case 1:
            if (--stepNum == 0) {
                stepDir = 3;
            }
            nextUpdateMs = now + config.delay1Ms;
            break;

        // held at far end
        case 2:
            if (config.bidirectional) {
                stepDir = 1;
                stepNum = pixelSet->numPanelPixels - 2;
            }
            else {
                stepDir = 0;
                stepNum = 0;
            }
            break;

        // held at near end
        case 3:
            stepDir = 0;
            nextUpdateMs = now + config.holdTimeNearMs;
            break;
    }

    if (0 == stepDir && 0 != prevStepDir && config.changeFgColor) {
        if (config.zipperBg) {
            // When doing the zipper effect, change the background color, too.
            selectRandomRgb(&fgColor, &bgColor);
            fgColor.nscale8_video(pixelSet->backgroundIntensityScaleFactor);
            bgColor.nscale8_video(pixelSet->backgroundIntensityScaleFactor);
        }
        else {
            selectRandomRgb(&fgColor, NULL);
            fgColor.nscale8_video(pixelSet->backgroundIntensityScaleFactor);
        }
    }

    CRGB rgbColor = ((stepDir == 1 || prevStepDir == 1) && config.zipperBg) ? CRGB::Black : bgColor;
    pixelSet->pixels[prevStepNum] = rgbColor;
    for (uint16_t j = 1; j < pixelSet->numPanels; ++j) {
        pixelSet->pixels[prevStepNum + pixelSet->numPanelPixels * j] = rgbColor;
    }

    pixelSet->pixels[stepNum] = fgColor;
    for (uint16_t j = 1; j < pixelSet->numPanels; ++j) {
        pixelSet->pixels[stepNum + pixelSet->numPanelPixels * j] = fgColor;
    }
  
    // Return true to request write to the LEDs.
    return true;
}

