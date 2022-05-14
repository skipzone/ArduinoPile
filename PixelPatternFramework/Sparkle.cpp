/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * $patternDesc Pattern                                            *
 *                                                                 *
 * by Ross Butler, Dec. 2015                                       *
 * (initial implementation created for EEkur Pyramid               *
 * at Esthetic Evolution 2014)                                     *
 *                                                                 *
 *******************************************************************/

#include "Sparkle.h"
#include "patternHelpers.h"

using namespace pixelPattern;


bool Sparkle::initPattern(bool configIsInFlash, void* patternConfig)
{
    // Returns true if successful, false if failed or pattern cannot run.

    if (configIsInFlash) {
        memcpy_P(&config, patternConfig, sizeof(PatternConfig));
    }
    else {
        memcpy(&config, patternConfig, sizeof(PatternConfig));
    }

    if (0 != selectedPixels) {
        delete [] selectedPixels;
        selectedPixels = 0;
    }
    selectedPixels = new uint16_t [config.density];
    // TODO:  Find out for sure if (nothrow) is the default behavior in Arduinolandia.
    if (0 == selectedPixels) {
        return false;
    }
    for (uint8_t i = 0; i < config.density; ++i) {
      selectedPixels[i] = 0;
    }

    if (config.fgColorCode != CRGB::Black || config.bgColorCode != CRGB::Black) {
        fgColor = config.fgColorCode;
        bgColor = config.bgColorCode;
    }
    else {
        selectRandomRgb(&fgColor, &bgColor);
    }

    bgColor.nscale8_video(pixelSet->backgroundIntensityScaleFactor);
    fgColor.nscale8_video(pixelSet->foregroundIntensityScaleFactor);
    
    // Fill with background color.
    fill_solid(pixelSet->pixels, pixelSet->numPixels, bgColor);

    sparklesAreOn = false;

    // We need update() to be called as soon as possible.
    nextUpdateMs = millis() - 1;

    return true;
}


bool Sparkle::update()
{
    uint32_t now = millis();

    if (sparklesAreOn) {
        sparklesAreOn = false;

        // Turn off the current set of sparkle pixels.
        for (uint8_t i = 0; i < config.density; ++i) {
            pixelSet->pixels[selectedPixels[i]] = bgColor;
        }

        // We need an update() call when it is time to turn on the next
        // sparkle set.  If that time is here or has already passed, we
        // will drop through to turn on the set now.
        nextUpdateMs = sparkleSetOnAtMs + config.changeMs;
        if (nextUpdateMs > now) {
            return true;
        }
    }

    sparklesAreOn = true;
    sparkleSetOnAtMs = now;

    // Turn on random sparkle pixels.
    for (uint8_t i = 0; i < config.density; ++i) {
        selectedPixels[i] = random16(pixelSet->numPixels);
        pixelSet->pixels[selectedPixels[i]] = fgColor;
    }

    // We need the next update() call when it is time to turn off the sparkles.
    nextUpdateMs = now + config.dwellMs;

    // Return true to request write to the LEDs.
    return true;
}

