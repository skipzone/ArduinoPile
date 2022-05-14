/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * SplitRotation Pattern                                           *
 *                                                                 *
 * by Ross Butler, Dec. 2015                                       *
 * (initial implementation created for EEkur Pyramid               *
 * at Esthetic Evolution 2014)                                     *
 *                                                                 *
 *******************************************************************/

#include "SplitRotation.h"

using namespace pixelPattern;


bool SplitRotation::initPattern(bool configIsInFlash, void* patternConfig)
{
    // Returns true if successful, false if failed or pattern cannot run.

    if (configIsInFlash) {
        memcpy_P(&config, patternConfig, sizeof(PatternConfig));
    }
    else {
        memcpy(&config, patternConfig, sizeof(PatternConfig));
    }

    HSVHue fgHue;
    HSVHue bgHue;
    if (config.randomHue) {
      selectRandomHue(&fgHue, &bgHue);
    }
    else {
      fgHue = config.fgHue;
      bgHue = config.bgHue;
    }

    CHSV fgHsv;
    CHSV bgHsv;
    fgHsv.h = fgHue;
    fgHsv.s = 255;
    fgHsv.v = 255;
    bgHsv.h = bgHue;
    bgHsv.s = 255;
    bgHsv.v = 255;

    // Fill with background color.
    CRGB rgbColor;
    hsv2rgb_rainbow(bgHsv, rgbColor);
    rgbColor.nscale8_video(pixelSet->backgroundIntensityScaleFactor);
    fill_solid(pixelSet->pixels, pixelSet->numPanelPixels, rgbColor);

    switch (config.mode) {
        case original:
            // Set every nth pixel to the foreground color.
            hsv2rgb_rainbow(fgHsv, rgbColor);
            rgbColor.nscale8_video(pixelSet->backgroundIntensityScaleFactor);
            for (uint16_t i = 0; i < pixelSet->numPanelPixels; i += config.fgInterval) {
                pixelSet->pixels[i] = rgbColor;
            }
            break;

        case symmetrical:
        default:
            // Create an initial pattern containing rotation sets.
            // Each set has one fg pixel and fgInterval bg pixels.  
            hsv2rgb_rainbow(fgHsv, rgbColor);
            rgbColor.nscale8_video(pixelSet->backgroundIntensityScaleFactor);
            uint16_t i = 0;
            while (i < pixelSet->numPanelPixels) {
                pixelSet->pixels[i++] = rgbColor;
                i += config.fgInterval * 2;
                if (i < pixelSet->numPanelPixels) {
                    pixelSet->pixels[i++] = rgbColor;
                }
            }
    }

    replicatePixelPanels(pixelSet);

    // We need update() to be called as soon as possible.
    nextUpdateMs = millis() - 1;

    return true;
}


bool SplitRotation::update()
{
    nextUpdateMs = millis() + config.delayMs;

    uint16_t i = pixelSet->numPanelPixels / 4;
    if (config.directionDown) {
        rotateCrgbLeft(pixelSet->pixels, i);
        rotateCrgbRight(pixelSet->pixels + i, i);
        rotateCrgbRight(pixelSet->pixels + i * 3, i);
        rotateCrgbLeft(pixelSet->pixels + i * 2, i);
    }
    else {
        rotateCrgbRight(pixelSet->pixels, i);
        rotateCrgbLeft(pixelSet->pixels + i, i);
        rotateCrgbLeft(pixelSet->pixels + i * 3, i);
        rotateCrgbRight(pixelSet->pixels + i * 2, i);
    }

    replicatePixelPanels(pixelSet);

    // Return true to request write to the LEDs.
    return true;
}

