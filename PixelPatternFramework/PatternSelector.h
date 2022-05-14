/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Pattern Selector Interface and Common Data                      *
 *                                                                 *
 * by Ross Butler   Feb. 2016                                      *
 *                                                                 *
 *******************************************************************/

#ifndef __PATTERN_SELECTOR_H
#define __PATTERN_SELECTOR_H

#include <stdint.h>


namespace pixelPattern {

class PatternSequence;


class PatternSelector {

public:

    PatternSelector();

    virtual ~PatternSelector() {}

    PatternSelector(const PatternSelector&) = delete;
    PatternSelector& operator =(const PatternSelector&) = delete;

    uint8_t getPatternNum() { return patternNum; }

    virtual uint8_t changePattern() = 0;
    virtual bool checkIfPatternChangeNeeded() = 0;
    virtual bool setPatternNum(uint8_t newPatternNum);
    virtual void setPatternSequence(PatternSequence* ps);

protected:

    uint8_t patternNum;
    PatternSequence* patternSequence;

private:

};

}

#endif  // #ifndef __PATTERN_SELECTOR_H
