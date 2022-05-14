/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Blocks Pattern                                                  *
 *                                                                 *
 * by Ross Butler   Dec. 2015                                      *
 *                                                                 *
 *******************************************************************/

#include "Blocks.h"
#include "patternHelpers.h"

using namespace pixelPattern;


bool Blocks::initPattern(bool configIsInFlash, void* patternConfig)
{
    // Returns true if successful, false if failed or pattern cannot run.

    if (configIsInFlash) {
        memcpy_P(&config, patternConfig, sizeof(PatternConfig));
    }
    else {
        memcpy(&config, patternConfig, sizeof(PatternConfig));
    }

    CHSV hsvColor;
    hsvColor.v = 255;
    CRGB rgbColor;

    uint8_t blockIdx = 0;
    uint16_t blockPixelCount = 0;
    uint16_t pixelIdx = 0;
    while (pixelIdx < pixelSet->numSymmetricalPixels) {
        if (blockPixelCount >= config.blocks[blockIdx].numPixels) {
            blockPixelCount = 0;
            if (++blockIdx >= numBlocks) {
                blockIdx = 0;
            }
        }
        else {
            if (0 == blockPixelCount) {
                hsvColor.h = config.blocks[blockIdx].hue;
                hsvColor.s = config.blocks[blockIdx].saturation;
                hsv2rgb_rainbow(hsvColor, rgbColor);
            }
            ++blockPixelCount;
            pixelSet->pixels[pixelIdx++] = rgbColor;
        }
    }

    // TODO:  use a helper for this
    for (uint16_t i = 0;
         i < pixelSet->numSymmetricalPixels;
         pixelSet->pixels[i++].nscale8_video(pixelSet->backgroundIntensityScaleFactor));

    nextUpdateMs = millis() + (config.delayMs > 0 ? config.delayMs : nonRotationalRefreshIntervalMs);

    return true;
}


bool Blocks::update()
{
    if (config.delayMs > 0) {
        rotateCrgbRight(pixelSet->pixels, pixelSet->numSymmetricalPixels);
        nextUpdateMs = millis() + config.delayMs;
    }
    else {
        nextUpdateMs = millis() + nonRotationalRefreshIntervalMs;
    }

    // Return true to request write to the LEDs.
    return true;
}
