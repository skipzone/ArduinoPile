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

#include "Glint.h"

using namespace pixelPattern;


bool Glint::initPattern(bool configIsInFlash, void* patternConfig)
{
    // Returns true if successful, false if failed or pattern cannot run.

    if (configIsInFlash) {
        memcpy_P(&config, patternConfig, sizeof(PatternConfig));
    }
    else {
        memcpy(&config, patternConfig, sizeof(PatternConfig));
    }

    nextGlintMs = millis() + config.glintIntervalMs;
    s = 0.025;
    w = (pixelSet->numSymmetricalPixels <= 30) ? 6 : pixelSet->numSymmetricalPixels / 5;
    ns = 255.0 / (float) w;

    // We need update() to be called as soon as possible.
    nextUpdateMs = millis() - 1;

    return true;
}


bool Glint::update()
{
    uint32_t now = millis();

    nextUpdateMs = now + config.delayMs;

    if (!doingGlint && now >= nextGlintMs) {
        n0 = - w;
        doingGlint = true;
//        Serial.println("starting glint");
//        Serial.print("s=");
//        Serial.print(s);
//        Serial.print("  w=");
//        Serial.print(w);
//        Serial.print("  ns=");
//        Serial.print(ns);
//        Serial.print("  n0=");
//        Serial.println(n0);
    }

    CHSV hsvColor;
    CRGB rgbColor;
    hsvColor.h = config.bgHue;
    hsvColor.v = pixelSet->backgroundIntensityScaleFactor;
    for (uint16_t i = 0; i < pixelSet->numSymmetricalPixels; ++i) {

        if (doingGlint) {
            if (i < n0 || i > n0 + w) {
                hsvColor.s = 255;
            }
            else {
                float n = (((float) i) - n0) * ns;
                float y = quadwave8(n);
                hsvColor.s = 255 - y;
//                Serial.print("i=");
//                Serial.print(i);
//                Serial.print("  n0=");
//                Serial.print(n0);
//                Serial.print("  n=");
//                Serial.print(n);
//                Serial.print("  y=");
//                Serial.print(y);
//                Serial.print("  hsvColor.s=");
//                Serial.print(hsvColor.s);
//                Serial.println("----------");
            }
        }
        else {
            hsvColor.s = 255;
        }

        hsv2rgb_rainbow(hsvColor, rgbColor);
        pixelSet->pixels[i] = rgbColor;  
    }

    // Make the non-symmetrical pixels the background color.
    hsvColor.s = 255;
    hsv2rgb_rainbow(hsvColor, rgbColor);
    for (uint16_t i = pixelSet->numSymmetricalPixels;
         i < pixelSet->numSymmetricalPixels + pixelSet->numNonsymmetricalPixels;
         pixelSet->pixels[i++] = rgbColor);

    if (doingGlint) {
        n0 += s;
        if (n0 >= (float) pixelSet->numSymmetricalPixels) {
            doingGlint = false;
            nextGlintMs = now + config.glintIntervalMs;
//            Serial.println("glint done");
        }
    }

    // Return true to request write to the LEDs.
    return true;
}

