/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Blocks Pattern                                                  *
 *                                                                 *
 * by Ross Butler   Dec. 2015                                      *
 *                                                                 *
 *******************************************************************/

#ifndef __BLOCKS_H
#define __BLOCKS_H

#include "PixelPattern.h"


namespace pixelPattern {

class Blocks : public PixelPattern {

public:

    static constexpr uint8_t id = 3;

    static constexpr uint8_t numBlocks = 8;
    static constexpr uint32_t nonRotationalRefreshIntervalMs = 1000;

    struct Block {
        HSVHue   hue;
        uint8_t  saturation;
        // TODO 10/23/2016:  add value so we can turn pixels off
        uint16_t numPixels;
    };

    struct PatternConfig {
        uint32_t delayMs;               // delay between rotation steps (0 for no rotation)
        // TODO:  add option to blank (set black) the last, partial block
        // TODO:  add option to blank (set black) the last, incomplete block set
        Block    blocks[numBlocks];
    };

    Blocks() {}
    ~Blocks() {}
        
    Blocks(const Blocks&) = delete;
    Blocks& operator =(const Blocks&) = delete;

    bool initPattern(bool configIsInFlash, void* patternConfig);
    bool update();

private:

    PatternConfig config;

};

}

#endif  // #ifndef __BLOCKS_H

