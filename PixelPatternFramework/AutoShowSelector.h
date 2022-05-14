/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Automatic-Show Pattern Selector Class                           *
 *                                                                 *
 * by Ross Butler   Feb. 2016                                      *
 *                                                                 *
 *******************************************************************/

#ifndef __AUTO_SHOW_SELECTOR_H
#define __AUTO_SHOW_SELECTOR_H

#include "PatternSelector.h"
#include <stdint.h>


namespace pixelPattern {

class AutoShowSelector : public PatternSelector {

public:

    AutoShowSelector();

    ~AutoShowSelector() {}

    AutoShowSelector(const AutoShowSelector&) = delete;
    AutoShowSelector& operator =(const AutoShowSelector&) = delete;

    void reset();
    bool checkIfPatternChangeNeeded();
    uint8_t changePattern();

protected:

private:

    unsigned long nextPatternChangeMs;
    uint8_t patternNum;
};

}

#endif  // #ifndef __AUTO_SHOW_SELECTOR_H
