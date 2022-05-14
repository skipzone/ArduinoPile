/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Pattern Selector Interface and Common Data                      *
 *                                                                 *
 * by Ross Butler   Feb. 2016                                      *
 *                                                                 *
 *******************************************************************/

#include "PatternSelector.h"

using namespace pixelPattern;


PatternSelector::PatternSelector()
    : patternNum(255)       // set to auto; derived class can change it
    , patternSequence(0)
{
}


bool PatternSelector::setPatternNum(uint8_t newPatternNum)
{
    // If the derived class doesn't implement setting the
    // pattern number, ignore the request and indicate failure.
    return false;
}


void PatternSelector::setPatternSequence(PatternSequence* ps)
{
    patternSequence = ps;
}

