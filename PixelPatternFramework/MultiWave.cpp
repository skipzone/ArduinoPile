/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Multi-Wave Pattern                                              *
 *                                                                 *
 * by Ross Butler   Jan. 2016                                      *
 * (initial implemntation created for John's Kaleidoscope Mirror,  *
 * Nov. 2014)                                                      *
 *                                                                 *
 *******************************************************************/

#include "MultiWave.h"
#include "patternHelpers.h"


using namespace pixelPattern;


bool MultiWave::initPattern(bool configIsInFlash, void* patternConfig)
{
    // Returns true if successful, false if failed or pattern cannot run.

    if (configIsInFlash) {
        memcpy_P(&config, patternConfig, sizeof(PatternConfig));
    }
    else {
        memcpy(&config, patternConfig, sizeof(PatternConfig));
    }

    numPixelsForPattern = config.boundedByPanel ? pixelSet->numPanelPixels : pixelSet->numSymmetricalPixels;

    numWaveforms = 0;
    lastDelayMs = UINT16_MAX;
    for (uint8_t w = 0; w < maxWaveforms; ++w)
    {
        // The first ColorWave struct with waveform type "none" or
        // no waveforms marks the end of the waveform definitions.
        if (ColorWave::none == config.waveParams[w].waveformType || 0 == config.waveParams[w].numWaves) {
            break;
        }
        ++numWaveforms;

        if (config.waveParams[w].randomHue) {
            selectRandomHue(&posHue[w], &negHue[w]);
            // Random colors are always fully saturated to avoid
            // the washed-out, easter-egg color problem.
            posSaturation[w] = negSaturation[w] = 255;
        }
        else {
            posHue[w] = config.waveParams[w].posHue;
            posSaturation[w] = config.waveParams[w].posSaturation;
            negHue[w] = config.waveParams[w].negHue;
            negSaturation[w] = config.waveParams[w].negSaturation;
        }
  
        // We use 16 bits for the angle interval and "time" so that we have sufficient
        // resultion to fit a complete set of the requested number of waves.
        angleInterval[w] = UINT16_MAX / numPixelsForPattern * abs(config.waveParams[w].numWaves);

        i0[w] = 0;
      
        // Use an immediate update to initially display the waveform.
        waveformDelayMs[w] = 1;
    }
    
    // We need update() to be called as soon as possible.
    nextUpdateMs = millis() - 1;

    return true;
}


bool MultiWave::update()
{
    uint32_t now = millis();

    // Figure out if it is time to move a waveform and update the pixels.
    // Also figure out how long to delay before the next update.
    bool needToDisplay = false;
    uint16_t shortestWaveformDelay = UINT16_MAX;
    for (uint8_t w = 0; w < numWaveforms; ++w) {
        waveformDelayMs[w] -= lastDelayMs;
        if (0 == waveformDelayMs[w]) {
            needToDisplay = true;
            waveformDelayMs[w] = config.waveParams[w].delayMs > 0 ? config.waveParams[w].delayMs : 1;
            if (config.waveParams[w].directionDown) {
                if (++i0[w] >= numPixelsForPattern) {
                    i0[w] = 0;
                }
            }
            else {
                if (0 == i0[w]--) {
                    i0[w] = numPixelsForPattern - 1;
                }
            }
        }
        if (waveformDelayMs[w] < shortestWaveformDelay) {
          shortestWaveformDelay = waveformDelayMs[w];
        }
    }

    nextUpdateMs = now + shortestWaveformDelay;
    lastDelayMs = shortestWaveformDelay;

    if (!needToDisplay) {
        return false;
    }

    for (uint16_t i = 0; i < numPixelsForPattern; ++i) {

        CHSV hsvWaveformPixels[maxWaveforms];
        uint16_t waveformPixelsValueSum = 0;

        for (uint8_t w = 0; w < numWaveforms; ++w) {
            uint8_t t = (i + i0[w]) * angleInterval[w] / 256;

            CHSV hsvColor;
            uint8_t y;

            switch (config.waveParams[w].waveformType) {
                case ColorWave::square:
                    y = triwave8(t) >= 128 ? 255 : 0;
                    break;
                case ColorWave::triangle:
                    y = triwave8(t);
                    break;
                case ColorWave::quadraticEasing:
                    y = quadwave8(t);
                    break;
                case ColorWave::cubicEasing:
                    y = cubicwave8(t);
                    break;
                case ColorWave::sine:
                    y = sin8(t);
                    break;
                default:
                    y = 0;
            }
            if (y >= 128) {
                hsvColor.h = posHue[w];
                hsvColor.s = posSaturation[w];
                hsvColor.v = (y - 128) * 2;
            }
            else {
                hsvColor.h = negHue[w];
                hsvColor.s = negSaturation[w];
                // A negative number of waves means make a half wave.
                hsvColor.v = (config.waveParams[w].numWaves >= 0) ? (127 - y) * 2 : 0;
            }

            hsvWaveformPixels[w] = hsvColor;
            waveformPixelsValueSum += hsvColor.v;
        } 

        if (waveformPixelsValueSum > 0) {
            CHSV hsvBlended;
            for (uint8_t w = 0; w < numWaveforms; ++w) {
                if (0 == w) {
                    hsvBlended = hsvWaveformPixels[w];
                }
                else {
                    // We'll try to make each wave have equal influence on the final display.
                    // When there are two waves, split 50%/50%.  When there are three, overlay
                    // 66%/33% -> (33%+33%)/33%, where the last 33% (the denominator)--the
                    // amount of overlay below--is how the current wave w is to be weighted.
                    // When there are four, split 75%/25% -> (25%+25%+25%)/25%, and so on.
                    // TODO:  Maybe make this an option.  Value-based blending is preferrable.
                    //fract8 amountOfOverlay = 255 - 255 / (w + 1);

                    // The influence of each waveform's pixel is proportional to that pixel's value (intensity).
                    fract8 amountOfOverlay = 255 * hsvWaveformPixels[w].v / waveformPixelsValueSum;

                    nblend(hsvBlended, hsvWaveformPixels[w], amountOfOverlay);
                }
            }
            hsv2rgb_rainbow(hsvBlended, pixelSet->pixels[i]);
            pixelSet->pixels[i].nscale8_video(pixelSet->backgroundIntensityScaleFactor);
        }
        else {
            pixelSet->pixels[i] = CRGB::Black;
        }
    }

    if (config.boundedByPanel) {
        replicatePixelPanels(pixelSet);
    }

    // Return true to request write to the LEDs.
    return true;
}
