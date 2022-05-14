/*******************************************************************
 *                                                                 *
 * Pixel Pattern Library Test                                      *
 *                                                                 *
 * Hardware and Application Implementation-Specific Configuration  *
 *                                                                 *
 * by Ross Butler  Dec.. 2015                                      *
 *                                                                 *
 *******************************************************************/

#ifndef __IMPLEMENTATION_CONFIG_H
#define __IMPLEMENTATION_CONFIG_H


#define PIXEL_DATA_PIN 2
#define MODE_PUSHBUTTON_PIN 10
#define GND_PIN 12
#define ONBOARD_LED_PIN 13

// the total number of pixels in the strip
//#define NUM_PHYSICAL_PIXELS 99
#define NUM_PHYSICAL_PIXELS 96

// the number of pixels at the start of the strip that should remain unused (off)
//#define NUM_SKIP_PIXELS 0
#define NUM_SKIP_PIXELS 0

// the number of pixels at the end of the strip that shouldn't be part of patterns that depend on symmetry
#define NUM_NONSYMMETRICAL_PIXELS 0

// the number of active pixels
#define NUM_PIXELS (NUM_PHYSICAL_PIXELS - NUM_SKIP_PIXELS)

// the number of pixels that should be used by patterns that depend upon physical layout symmetry
#define NUM_SYMMETRICAL_PIXELS (NUM_PIXELS - NUM_NONSYMMETRICAL_PIXELS)

// the number of pixels attached to each panel
// (must be a factor of the number of physical pixels)
#define NUM_PANELS 6
#define NUM_PANEL_PIXELS (NUM_SYMMETRICAL_PIXELS / NUM_PANELS)

#define FG_INTENSITY_SCALE_FACTOR 255

// BG_INTENSITY_SCALE_FACTOR sets the maximum brightness for backgrounds.  It also
// sets the brightness of the foreground of patterns that aren't doing some sort
// of flashing or sparkle effect.  Although the range is 0 to 255, practical
// values are more like 16 to 128.  48 is pretty darn bright in a darkened room.
#define BG_INTENSITY_SCALE_FACTOR 128

#define PUSHBUTTON_DEBOUNCE_INTERVAL_MS 100


#endif  // #ifndef __IMPLEMENTATION_CONFIG_H
