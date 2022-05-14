/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * External-Control Pattern Selector Class                           *
 *                                                                 *
 * by Ross Butler   Mar. 2016                                      *
 *                                                                 *
 *******************************************************************/

#ifndef __EXTERNAL_CONTROL_SELECTOR_H
#define __EXTERNAL_CONTROL_SELECTOR_H

#include "AutoShowSelector.h"
#include "PatternSelector.h"
#include <stdint.h>


namespace pixelPattern {

class ExternalControlSelector : public PatternSelector {

public:

    ExternalControlSelector();

    ~ExternalControlSelector() {}

    ExternalControlSelector(const ExternalControlSelector&) = delete;
    ExternalControlSelector& operator =(const ExternalControlSelector&) = delete;

    bool setPatternNum(uint8_t newPatternNum);
    void setPatternSequence(PatternSequence* ps);
    bool checkIfPatternChangeNeeded();
    uint8_t changePattern();

protected:

private:

    AutoShowSelector autoShowSelector;
    bool patternChangeRequested;
};

}

#endif  // #ifndef __EXTERNAL_CONTROL_SELECTOR_H

