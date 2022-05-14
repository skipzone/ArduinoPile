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
#define MODE_PUSHBUTTON_PIN 9
#define GND_PIN 7
#define ONBOARD_LED_PIN 13

#define TEST_STRIP_NUM_PHYSICAL_PIXELS 99

// the total number of pixels in the strip
//#define NUM_PHYSICAL_PIXELS 99
#define NUM_PHYSICAL_PIXELS 89

// the number of pixels at the start of the strip that should remain unused (off)
//#define NUM_SKIP_PIXELS 0
#define NUM_SKIP_PIXELS 2

// the number of pixels at the end of the strip that shouldn't be part of patterns that depend on symmetry
//#define NUM_NONSYMMETRICAL_PIXELS 0
#define NUM_NONSYMMETRICAL_PIXELS 3

// the number of active pixels
#define NUM_PIXELS (NUM_PHYSICAL_PIXELS - NUM_SKIP_PIXELS)

// the number of pixels that should be used by patterns that depend upon physical layout symmetry
#define NUM_SYMMETRICAL_PIXELS (NUM_PIXELS - NUM_NONSYMMETRICAL_PIXELS)

// the number of pixels attached to each panel
// (must be a factor of the number of physical pixels)
#define NUM_PANELS 3
#define NUM_PANEL_PIXELS (NUM_SYMMETRICAL_PIXELS / NUM_PANELS)

#define FG_INTENSITY_SCALE_FACTOR 64

// BG_INTENSITY_SCALE_FACTOR sets the maximum brightness for backgrounds.  It also
// sets the brightness of the foreground of patterns that aren't doing some sort
// of flashing or sparkle effect.  Although the range is 0 to 255, practical
// values are more like 16 to 128.  48 is pretty darn bright in a darkened room.
#define BG_INTENSITY_SCALE_FACTOR 32

#define PUSHBUTTON_DEBOUNCE_INTERVAL_MS 100

#define ENABLE_AMBIENCE_LIGHTS
#define AMBIENCE_NUM_PHYSICAL_PIXELS 9
#define AMBIENCE_NUM_SKIP_PIXELS 0
#define AMBIENCE_NUM_NONSYMMETRICAL_PIXELS 0
#define AMBIENCE_NUM_PIXELS (AMBIENCE_NUM_PHYSICAL_PIXELS - AMBIENCE_NUM_SKIP_PIXELS)
#define AMBIENCE_NUM_PANELS 1
#define AMBIENCE_NUM_PANEL_PIXELS (AMBIENCE_NUM_PIXELS / AMBIENCE_NUM_PANELS)
#define AMBIENCE_FG_INTENSITY_SCALE_FACTOR 32
#define AMBIENCE_BG_INTENSITY_SCALE_FACTOR 32


#ifdef ENABLE_DMX
  #define FASTSPI_USE_DMX_SIMPLE
  #define DMX_SIZE 27
#endif


#endif  // #ifndef __IMPLEMENTATION_CONFIG_H
