/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Automatic-Show Pattern Selector Class                           *
 *                                                                 *
 * by Ross Butler   Feb. 2016                                      *
 *                                                                 *
 *******************************************************************/

#include "AutoShowSelector.h"
#include "PatternSequence.h"
#include "pixelPatternFrameworkTypes.h"

using namespace pixelPattern;


AutoShowSelector::AutoShowSelector()
    : nextPatternChangeMs(0)
{
}


void AutoShowSelector::reset()
{
    nextPatternChangeMs = 0;
    patternNum = 255;
}


bool AutoShowSelector::checkIfPatternChangeNeeded()
{
    return millis() >= nextPatternChangeMs;
}


uint8_t AutoShowSelector::changePattern()
{
    // Select the next pattern (with wraparound) that has a duration.
    // (Patterns with zero duration should not be selected automatically.)
    // TODO:  Put in protection against infinite loop if all patterns have zero duration.
    uint32_t durationMs;
    do {
        if (++patternNum >= patternSequence->numPatterns) {
            patternNum = 0;
        }
        patternSequence->readPatternDefinitionFromFlash(patternNum, nullptr, &durationMs);
    } while (durationMs == 0);

    nextPatternChangeMs = millis() + durationMs;

    return patternNum;
}

