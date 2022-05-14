/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Pattern Controller Class                                        *
 *                                                                 *
 * by Ross Butler   Dec. 2015                                      *
 *                                                                 *
 *******************************************************************/

#ifndef __PIXEL_PATTERN_CONTROLLER_H
#define __PIXEL_PATTERN_CONTROLLER_H 

#include "pixelPatternFrameworkTypes.h"
#include <stdint.h>


namespace pixelPattern {

class PatternDef;
class PatternSequence;
class PixelPattern;
class PixelSet;

class PixelPatternController {

public:


    PixelPatternController()
        :
        numPatternSequences(0),
        useStatusLed(false),
        statusLedPin(-1),
        initDone(false)
        {};

    ~PixelPatternController() {};

    PixelPatternController(const PixelPatternController&) = delete;
    PixelPatternController& operator =(const PixelPatternController&) = delete;

    void enableStatusLed(int8_t pin);
    uint8_t addPatternSequence(PatternSequence* patternSequence, PixelSet* pixelSet);
    bool getUpdateLeds(uint8_t patternSequenceIdx);
    void init();
    uint32_t freeRam();
    void displayRelativeValue(PixelSet* pixelSet, uint8_t value, CRGB rgbColor);
    bool update();

private:

    struct PatternState {
        PatternSequence* patternSequence;
        PixelSet* pixelSet;
        PixelPattern* pixPat;
        bool loopedWithoutPatternUpdate;
        bool timingSatisfied;
        bool updateLeds;
    };

    PatternState* patternStates[maxPatternSequences];

    uint8_t numPatternSequences;
    bool useStatusLed;
    int8_t statusLedPin;

    bool initDone;

    void updatePattern(PatternState& patternState);
};

}

#endif  // #ifndef __PIXEL_PATTERN_CONTROLLER_H

