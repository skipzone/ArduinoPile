/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Pattern Configuration Constants and Structures                  *
 *                                                                 *
 * by Ross Butler   Nov. 2014 - Nov. 2015                          *
 *                                                                 *
 *******************************************************************/

#ifndef __PATTERN_FRAMEWORK_TYPES_H
#define __PATTERN_FRAMEWORK_TYPES_H


#include "FastLED.h"

namespace pixelPattern {

static constexpr uint8_t maxPatternSequences = 8;

// This structure contains the definition of an actual pattern to be displayed.
// It associates a pattern type with the values that control the pattern's
// appearance and the length of time the pattern should be displayed.
struct PatternDef {
  uint8_t     patternId;
  uint32_t    durationMs;
  const void* patternConfig;
  const char* patternName;      // when not needed, can be null to save memory
};

}

#endif  // #ifndef __PATTERN_FRAMEWORK_TYPES_H
