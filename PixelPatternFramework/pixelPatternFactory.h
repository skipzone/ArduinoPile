/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Pixel Pattern Object Factory                                    *
 *                                                                 *
 * by Ross Butler   Dec. 2015                                      *
 *                                                                 *
 *******************************************************************/

#ifndef __PIXEL_PATTERN_FACTORY_H
#define __PIXEL_PATTERN_FACTORY_H 

#include "PixelPattern.h"
#include "SolidColor.h"
#include "Sparkle.h"
#include "Blocks.h"
#include "Rainbow.h"
#include "Glint.h"
#include "MovingDot.h"
#include "SplitRotation.h"
#include "MultiWave.h"


namespace pixelPattern {

static PixelPattern* pixelPatternFactory(uint8_t patternId)
{
    switch(patternId) {
        case SolidColor::id:
            return new SolidColor;
        case Sparkle::id:
            return new Sparkle;
        case Blocks::id:
            return new Blocks;
        case Rainbow::id:
            return new Rainbow;
        case Glint::id:
            return new Glint;
        case MovingDot::id:
            return new MovingDot;
        case SplitRotation::id:
            return new SplitRotation;
        case MultiWave::id:
            return new MultiWave;
        default:
            // TODO:  We need to return an ErrorPattern object that ignores the config and flashes all pixels a dim red.
            return new SolidColor;
    }
}

}

#endif  // #ifdef __PIXEL_PATTERN_FACTORY_H 

