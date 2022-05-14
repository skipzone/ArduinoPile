/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Pushbutton Pattern Selector Class                               *
 *                                                                 *
 * by Ross Butler   April 2016                                     *
 *                                                                 *
 *******************************************************************/

#ifndef __PUSHBUTTON_SELECTOR_H
#define __PUSHBUTTON_SELECTOR_H

#include "AutoShowSelector.h"
#include "PatternSelector.h"
#include <stdint.h>


namespace pixelPattern {

class PushbuttonSelector : public PatternSelector {

public:

    static constexpr unsigned long pushbuttonDebounceIntervalMs = 100;

    PushbuttonSelector(uint8_t pushbuttonPin);

    ~PushbuttonSelector() {}

    PushbuttonSelector() = delete;
    PushbuttonSelector(const PushbuttonSelector&) = delete;
    PushbuttonSelector& operator =(const PushbuttonSelector&) = delete;

    bool setPatternNum(uint8_t newPatternNum);
    void setPatternSequence(PatternSequence* ps);
    void poll();
    bool checkIfPatternChangeNeeded();
    uint8_t changePattern();

protected:

private:

    uint8_t pushbuttonPin;
    unsigned long lastPushbuttonEventMs;
    bool lastPushbuttonState;
    bool pushbuttonDebouncedState;
    AutoShowSelector autoShowSelector;
    uint8_t patternChangeRequestCount;
};

}

#endif  // #ifndef __PUSHBUTTON_SELECTOR_H
