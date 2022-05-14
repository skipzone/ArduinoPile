/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Hardware and Application Implementation-Specific Configuration  *
 *                                                                 *
 * by Ross Butler   Nov. 2014 - Nov. 2015                          *
 *                                                                 *
 *******************************************************************/

#ifndef __IMPLEMENTATION_CONFIG_H
#define __IMPLEMENTATION_CONFIG_H


#define DEVELOPMENT_STRIP
//#define HOUSE_PATIO_AND_SOFFIT_LIGHTS
//#define SANTA_HAT
//#define ROZANNS_PATIO_LIGHTS



/*********************
 * Development Strip *
 *********************/

#ifdef DEVELOPMENT_STRIP

#define PIXEL_DATA_PIN 2
#define MODE_PUSHBUTTON_PIN 10
#define GND_PIN 12
#define ONBOARD_LED_PIN 13

// the total number of pixels in the strip
#define NUM_PHYSICAL_PIXELS 99

// the number of pixels at the start of the strip that should remain unused (off)
#define NUM_SKIP_PIXELS 0

// the number of pixels at the end of the strip that shouldn't be part of patterns that depend on symmetry
#define NUM_NONSYMMETRICAL_PIXELS 0

// the number of active pixels
#define NUM_PIXELS (NUM_PHYSICAL_PIXELS - NUM_SKIP_PIXELS)

// the number of pixels attached to each panel
// (must be a factor of the number of physical pixels)
#define NUM_PANELS 1
#define PANEL_NUM_PIXELS (NUM_PIXELS / NUM_PANELS)

// BG_INTENSITY_SCALE_FACTOR sets the maximum brightness for backgrounds.  It also
// sets the brightness of the foreground of patterns that aren't doing some sort
// of flashing or sparkle effect.  Although the range is 0 to 255, practical
// values are more like 16 to 128.  48 is pretty darn bright in a darkened room.
#define BG_INTENSITY_SCALE_FACTOR 64

// MAX_WAVEFORMS is the maximum number of waveforms
// that can be configured for a multiWave pattern.
#define MAX_WAVEFORMS 3

// MAX_SPARKLE_DENSITY is the maximum number of foreground pixels
// that can be illuminated simultaneously in a sparkle pattern.
#define MAX_SPARKLE_DENSITY 5

// Approximate millisecond delay between sparkle changes.
#define SPARKLE_DELAY 15

#define PUSHBUTTON_DEBOUNCE_INTERVAL_MS 100

#endif  // #ifdef DEVELOPMENT_STRIP



/*********************************
 * House Patio and Soffit Lights *
 *********************************/

#ifdef HOUSE_PATIO_AND_SOFFIT_LIGHTS

#define PIXEL_DATA_PIN 2
#define DMX_DATA_PIN 3
#define MODE_PUSHBUTTON_PIN A2
#define GND_PIN 12
#define ONBOARD_LED_PIN 13

// the total number of pixels in the strip
#define NUM_PHYSICAL_PIXELS 123

// the number of pixels at the start of the strip that should remain unused (off)
#define NUM_SKIP_PIXELS 0

// the number of pixels at the end of the strip that shouldn't be part of patterns that depend on symmetry
#define NUM_NONSYMMETRICAL_PIXELS 0

// the number of active pixels
#define NUM_PIXELS (NUM_PHYSICAL_PIXELS - NUM_SKIP_PIXELS)

// the number of pixels attached to each panel
// (must be a factor of the number of physical pixels)
#define NUM_PANELS 1
#define PANEL_NUM_PIXELS (NUM_PIXELS / NUM_PANELS)

// BG_INTENSITY_SCALE_FACTOR sets the maximum brightness for backgrounds.  It also
// sets the brightness of the foreground of patterns that aren't doing some sort
// of flashing or sparkle effect.  Although the range is 0 to 255, practical
// values are more like 16 to 128.  48 is pretty darn bright in a darkened room.
#define BG_INTENSITY_SCALE_FACTOR 255

// MAX_WAVEFORMS is the maximum number of waveforms
// that can be configured for a multiWave pattern.
#define MAX_WAVEFORMS 3

// MAX_SPARKLE_DENSITY is the maximum number of foreground pixels
// that can be illuminated simultaneously in a sparkle pattern.
#define MAX_SPARKLE_DENSITY 5

// Approximate millisecond delay between sparkle changes.
#define SPARKLE_DELAY 15

#define PUSHBUTTON_DEBOUNCE_INTERVAL_MS 100

// These DMX configuration values control the ambiance lighting.
// They work like their similarly named pixel counterparts.
// They are ignored if ENABLE_DMX_AMBIENCE_LIGHTS is not defined.
#define ENABLE_DMX_AMBIENCE_LIGHTS
#define NUM_DMX_RGB_CHANNELS 9          // each DMX channel is essentially one RGB led or strip
#define NUM_DMX_PANELS 1
#define DMX_PANEL_NUM_RGB_CHANNELS 9
#define DMX_AMBIANCE_INTENSITY_SCALE_FACTOR 128

#endif  // #ifdef HOUSE_PATIO_AND_SOFFIT_LIGHTS



/*************
 * Santa Hat *
 *************/

#ifdef SANTA_HAT

#define PIXEL_DATA_PIN 2
#define MODE_PUSHBUTTON_PIN 6
#define GND_PIN 9
#define ONBOARD_LED_PIN 13

// the total number of pixels in the strip
#define NUM_PHYSICAL_PIXELS 93

// the number of pixels at the start of the strip that should remain unused (off)
#define NUM_SKIP_PIXELS 0

// the number of pixels at the end of the strip that shouldn't be part of patterns that depend on symmetry
#define NUM_NONSYMMETRICAL_PIXELS 0

// the number of active pixels
#define NUM_PIXELS (NUM_PHYSICAL_PIXELS - NUM_SKIP_PIXELS)

// the number of pixels attached to each panel
// (must be a factor of the number of physical pixels)
#define NUM_PANELS 1
#define PANEL_NUM_PIXELS (NUM_PIXELS / NUM_PANELS)

// BG_INTENSITY_SCALE_FACTOR sets the maximum brightness for backgrounds.  It also
// sets the brightness of the foreground of patterns that aren't doing some sort
// of flashing or sparkle effect.  Although the range is 0 to 255, practical
// values are more like 16 to 128.  48 is pretty darn bright in a darkened room.
#define BG_INTENSITY_SCALE_FACTOR 64

// MAX_WAVEFORMS is the maximum number of waveforms
// that can be configured for a multiWave pattern.
#define MAX_WAVEFORMS 3

// MAX_SPARKLE_DENSITY is the maximum number of foreground pixels
// that can be illuminated simultaneously in a sparkle pattern.
#define MAX_SPARKLE_DENSITY 5

// Approximate millisecond delay between sparkle changes.
#define SPARKLE_DELAY 15

#define PUSHBUTTON_DEBOUNCE_INTERVAL_MS 100

#endif  // #ifdef SANTA_HAT



/*************************
 * Rozann's Patio Lights *
 *************************/

#ifdef ROZANNS_PATIO_LIGHTS

#define PIXEL_DATA_PIN 2
#define MODE_PUSHBUTTON_PIN A2
#define GND_PIN 12
#define ONBOARD_LED_PIN 13

// the total number of pixels in the strip
#define NUM_PHYSICAL_PIXELS 250

// the number of pixels at the start of the strip that should remain unused (off)
#define NUM_SKIP_PIXELS 0

// the number of pixels at the end of the strip that shouldn't be part of patterns that depend on symmetry
#define NUM_NONSYMMETRICAL_PIXELS 0

// the number of active pixels
#define NUM_PIXELS (NUM_PHYSICAL_PIXELS - NUM_SKIP_PIXELS)

// the number of pixels attached to each panel
// (must be a factor of the number of physical pixels)
#define NUM_PANELS 1
#define PANEL_NUM_PIXELS (NUM_PIXELS / NUM_PANELS)

// BG_INTENSITY_SCALE_FACTOR sets the maximum brightness for backgrounds.  It also
// sets the brightness of the foreground of patterns that aren't doing some sort
// of flashing or sparkle effect.  Although the range is 0 to 255, practical
// values are more like 16 to 128.  48 is pretty darn bright in a darkened room.
#define BG_INTENSITY_SCALE_FACTOR 192

// MAX_WAVEFORMS is the maximum number of waveforms
// that can be configured for a multiWave pattern.
#define MAX_WAVEFORMS 3

// MAX_SPARKLE_DENSITY is the maximum number of foreground pixels
// that can be illuminated simultaneously in a sparkle pattern.
#define MAX_SPARKLE_DENSITY 15

// Approximate millisecond delay between sparkle changes.
#define SPARKLE_DELAY 15

#define PUSHBUTTON_DEBOUNCE_INTERVAL_MS 100

#endif  // #ifdef ROZANNS_PATIO_LIGHTS



/**********
 * Common *
 **********/
 
#ifdef ENABLE_DMX_AMBIENCE_LIGHTS
  #define FASTSPI_USE_DMX_SIMPLE
  #define DMX_SIZE 27
#endif


#endif  // #ifndef __IMPLEMENTATION_CONFIG_H
