/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Pattern Sequence Class                                          *
 *                                                                 *
 * by Ross Butler   Feb. 2016                                      *
 *                                                                 *
 *******************************************************************/

#include <Arduino.h>
#include "AutoShowSelector.h"
#include "PatternSelector.h"
#include "PatternSequence.h"
#include "pixelPatternFrameworkTypes.h"

using namespace pixelPattern;


PatternSequence::PatternSequence(
    const PatternDef* patternDefs,
    uint8_t numPatterns,
    PatternSelector* patternSelector)
    :
    numPatterns(numPatterns),
    currentPatternNum(0),
    patternDefs(patternDefs),
    patternSelector(patternSelector)
{
    if (0 == this->patternSelector) {
        this->patternSelector = new AutoShowSelector();
    }
    this->patternSelector->setPatternSequence(this);
}


void PatternSequence::changePattern()
{
    currentPatternNum = patternSelector->changePattern();
}


bool PatternSequence::patternChangeRequested()
{
    return patternSelector->checkIfPatternChangeNeeded();
}


void PatternSequence::readPatternDefinitionFromFlash(
    uint8_t patternNum,
    uint8_t* pPatternId,
    uint32_t* pDurationMs,
    void** pPatternConfig,
    String* pPatternName)
{
    const PatternDef* patDef = patternDefs + patternNum;

    if (pPatternId) {
        uint8_t patternId = (uint8_t) pgm_read_byte(&patDef->patternId);
        *pPatternId = patternId;
    }

    if (pDurationMs) {
        uint32_t durationMs = (uint32_t) pgm_read_dword(&patDef->durationMs);
        *pDurationMs = durationMs;
    }

    if (pPatternConfig) {
        void* patternConfig;
        if (sizeof(patDef->patternConfig) == 4) {
            patternConfig = (void*) pgm_read_dword(&patDef->patternConfig);
        }
        else {
            patternConfig = (void*) pgm_read_word(&patDef->patternConfig);
        }
        *pPatternConfig = patternConfig;
    }

    if (pPatternName) {
        if (patDef->patternName != nullptr) {
            // TODO:  use this code (which might even work) when patternName is stored in program memory
            //char buf[maxPatternNameLength + 1];
            //strncpy_P(buf, (char*) pgm_read_word(&patDef->patternName), maxPatternNameLength);
            //buf[maxPatternNameLength] = 0;
            //*pPatternName = buf;
            *pPatternName = patDef->patternName;
        }
        else {
            *pPatternName = "Pattern " + String(patternNum);
        }
    }
}


void PatternSequence::readCurrentPatternDefinitionFromFlash(
    uint8_t* pPatternId,
    uint32_t* pDurationMs,
    void** pPatternConfig,
    String* pPatternName)
{
    return readPatternDefinitionFromFlash(currentPatternNum, pPatternId, pDurationMs, pPatternConfig, pPatternName);
}

