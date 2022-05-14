/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Multiple Wave Pattern                                           *
 *                                                                 *
 * by Ross Butler, Dec. 2015                                       *
 * (initial implemntation created for John's Kaleidoscope Mirror,  *
 * Nov. 2014)                                                      *
 *                                                                 *
 *******************************************************************/

#ifndef __MULTI_WAVE_H
#define __MULTI_WAVE_H

#include "PixelPattern.h"


namespace pixelPattern {
 

// This structure holds a single waveform configuration
// that is part of a multiWave pattern.
struct ColorWave {

    // These are the waveforms that can be used in a multiWave pattern.  A good
    // explanation of the shape and computation cost of the quadratic and cubic
    // easing waveforms can be found here:
    //
    //     https://github.com/FastLED/FastLED/wiki/FastLED-Wave-Functions
    //
    // From that web page:
    //
    //     quadwave8(i) -- quadratic in/out easing applied to a triangle wave. This
    //     makes nearly a sine wave, but takes only about 2/3rds of the CPU cycles
    //     of FastLED's fastest sine function.
    //
    //     cubicwave8(i) -- cubic in/out easing applied to a triangle wave. This
    //     makes a 'higher contrast' wave than the quad or sine wave. 10-20% faster
    //     than FastLED's fastest sine function.
    enum WaveformType {
        none,
        cubicEasing,
        quadraticEasing,
        sine,
        square,
        triangle
    };

    bool            randomHue;       // true if the waveform hue should be selected at random
    HSVHue          posHue;          // hue of the positive half of the waveform (if randomHue is false)
    uint8_t         posSaturation;   // saturation of the positive half of the waveform (if randomHue is false)
    HSVHue          negHue;          // hue of the negative half of the waveform (if randomHue is false)
    uint8_t         negSaturation;   // saturation of the negative half of the waveform (if randomHue is false)
    int8_t          numWaves;        // no. of complete waveforms in pattern; neg. for halfwave
    WaveformType    waveformType;    // sine, square, etc.; none to disable
    bool            directionDown;   // true if the waves should move toward pixel 0, false if they should move the other direction
    // TODO:  express the delay instead as the period of a complete waveform movement across the pixels
    uint32_t        delayMs;         // the period between each stepped movement of a wave
};


class MultiWave : public PixelPattern {

public:

    static constexpr uint8_t id = 8;
    static constexpr uint8_t maxWaveforms = 3;

    struct PatternConfig {
        bool boundedByPanel;  // true if all the waveforms should appear across each panel, false if across the entire strip
        // TODO:  rename waveParams to colorWaves
        ColorWave waveParams[maxWaveforms];
    };

    MultiWave() {};
    ~MultiWave() {};
        
    MultiWave(const MultiWave&) = delete;
    MultiWave& operator =(const MultiWave&) = delete;

    bool initPattern(bool configIsInFlash, void* patternConfig);
    bool update();

private:

    PatternConfig config;
    uint16_t numPixelsForPattern;
    uint8_t numWaveforms;
    HSVHue posHue[maxWaveforms];
    uint8_t posSaturation[maxWaveforms];
    HSVHue negHue[maxWaveforms];
    uint8_t negSaturation[maxWaveforms];
    uint16_t angleInterval[maxWaveforms];
    uint16_t i0[maxWaveforms];
    uint16_t waveformDelayMs[maxWaveforms];
    uint16_t lastDelayMs;

};

}

#endif  // #ifndef __MULTI_WAVE_H

