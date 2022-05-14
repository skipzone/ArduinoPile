/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * External-Control Pattern Selector Class                         *
 *                                                                 *
 * by Ross Butler   Mar. 2016                                      *
 *                                                                 *
 *******************************************************************/

#include "ExternalControlSelector.h"
#include "PatternSequence.h"
#include "pixelPatternFrameworkTypes.h"

using namespace pixelPattern;


ExternalControlSelector::ExternalControlSelector()
    : autoShowSelector()
    , patternChangeRequested(true)
{
}


void ExternalControlSelector::setPatternSequence(PatternSequence* ps)
{
    patternSequence = ps;
    autoShowSelector.setPatternSequence(ps);
}


bool ExternalControlSelector::setPatternNum(uint8_t newPatternNum)
{
    if (newPatternNum < patternSequence->numPatterns || newPatternNum == 255) {
        patternNum = newPatternNum;
        patternChangeRequested = true;
        if (newPatternNum == 255) {
            autoShowSelector.reset();
        }
        return true;
    }

    return false;
}


bool ExternalControlSelector::checkIfPatternChangeNeeded()
{
    if (patternChangeRequested) {
        // TODO:  Shouldn't it be set false in changePattern?
        patternChangeRequested = false;
        return true;
    }

    if (patternNum == 255) {
        return autoShowSelector.checkIfPatternChangeNeeded();
    }

    return false;
}


uint8_t ExternalControlSelector::changePattern()
{
    return patternNum != 255 ? patternNum : autoShowSelector.changePattern();
}

