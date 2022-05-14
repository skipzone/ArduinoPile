/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Pushbutton Pattern Selector Class                               *
 *                                                                 *
 * by Ross Butler   April 2016                                     *
 *                                                                 *
 *******************************************************************/

#include "PushbuttonSelector.h"
#include "PatternSequence.h"
#include "pixelPatternFrameworkTypes.h"

using namespace pixelPattern;


PushbuttonSelector::PushbuttonSelector(uint8_t pushbuttonPin)
    : pushbuttonPin(pushbuttonPin)
    , lastPushbuttonEventMs(0)
    , lastPushbuttonState(HIGH)
    , pushbuttonDebouncedState(HIGH)
    , autoShowSelector()
    , patternChangeRequestCount(0)
{
}


void PushbuttonSelector::setPatternSequence(PatternSequence* ps)
{
    patternSequence = ps;
    autoShowSelector.setPatternSequence(ps);

    patternSequence->currentPatternNum = 0;
}


bool PushbuttonSelector::setPatternNum(uint8_t newPatternNum)
{
    if (newPatternNum < patternSequence->numPatterns || newPatternNum == 255) {
        patternNum = newPatternNum;
        if (newPatternNum == 255) {
            autoShowSelector.reset();
        }
        else
        {
            patternChangeRequestCount = 1;
        }
        return true;
    }

    return false;
}


void PushbuttonSelector::poll()
{
    bool pushbuttonState = digitalRead(pushbuttonPin);

    if (pushbuttonState != lastPushbuttonState) {
        lastPushbuttonState = pushbuttonState;
        lastPushbuttonEventMs = millis();
        return;
    }

    if (pushbuttonState != pushbuttonDebouncedState
            && millis() - lastPushbuttonEventMs > pushbuttonDebounceIntervalMs) {
        pushbuttonDebouncedState = pushbuttonState;
        if (pushbuttonDebouncedState == LOW) {
            ++patternChangeRequestCount;
        }
    }
}


bool PushbuttonSelector::checkIfPatternChangeNeeded()
{
    if (patternChangeRequestCount > 0) {
        return true;
    }

    if (patternNum == 255) {
        return autoShowSelector.checkIfPatternChangeNeeded();
    }

    return false;
}


uint8_t PushbuttonSelector::changePattern()
{
    if (patternNum == 255 && 0 == patternChangeRequestCount) {
        return autoShowSelector.changePattern();
    }

    // The next pattern to select is the one after whichever pattern is
    // currently being displayed (which isn't patternNum when in auto mode).
    uint8_t newPatternNum = patternSequence->currentPatternNum;

    // For each time the button has been pushed, select the next pattern, or go
    // into auto mode if past all patterns.  Wrap around to the first pattern
    // after auto mode.
    do {
        if (++newPatternNum >= patternSequence->numPatterns) {
            newPatternNum = 255;
            autoShowSelector.reset();
        }
    } while (--patternChangeRequestCount > 0);

    patternNum = newPatternNum;

    // When switching to auto mode, start at the first pattern.
    // TODO:  maybe need to return autoShowSelector.changePattern() when patternNum == 255.
    //        testing suggests not, but you need to sober the fuck up and figure it out.
    return patternNum != 255 ? patternNum : 0;
}

