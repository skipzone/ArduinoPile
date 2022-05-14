/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Pattern Sequence Class                                          *
 *                                                                 *
 * by Ross Butler   Feb. 2016                                      *
 *                                                                 *
 *******************************************************************/

#ifndef __PATTERN_SEQUENCE_H
#define __PATTERN_SEQUENCE_H

#include <stdint.h>

class String;

namespace pixelPattern {

class PatternDef;
class PatternSelector;


class PatternSequence {

public:

    static constexpr uint8_t maxPatternNameLength = 32;

    PatternSequence(
        const PatternDef* patternDefs,
        uint8_t numPatterns,
        PatternSelector* patternSelector);

    ~PatternSequence() {}

    PatternSequence(const PatternSequence&) = delete;
    PatternSequence& operator =(const PatternSequence&) = delete;

    void changePattern();
    bool patternChangeRequested();

    void readPatternDefinitionFromFlash(
        uint8_t patternNum,
        uint8_t* pPatternId = nullptr,
        uint32_t* pDurationMs = nullptr,
        void** pPatternConfig = nullptr,
        String* pPatternName = nullptr);

    void readCurrentPatternDefinitionFromFlash(
        uint8_t* pPatternId = nullptr,
        uint32_t* pDurationMs = nullptr,
        void** pPatternConfig = nullptr,
        String* pPatternName = nullptr);

    uint8_t numPatterns;
    uint8_t currentPatternNum;
    const PatternDef* patternDefs;
    PatternSelector* patternSelector;
};

}

#endif  // #ifndef __PATTERN_SEQUENCE_H

