#include <stdint.h>

#define NUM_HW_ROWS 8    // there are always 8 physical rows across all daisy-chained panels
#define NUM_HW_COLS (NUM_DISPLAY_COLS * (NUM_DISPLAY_ROWS / 8))

#define NUM_PIXELS (NUM_HW_ROWS * NUM_HW_COLS)

#ifdef USE_FAST_PIXEL_IDX_CALC 
  #define HW_ROW_MASK (NUM_HW_ROWS - 1)
  // TODO:  need to make this log2(NUM_HW_ROWS) somehow
  #define HW_ROW_SHIFT 3
  // TODO:  need to make this log2(NUM_DISPLAY_COLS) somehow
  #define DISPLAY_COL_SHIFT 5
#endif

typedef enum {
  off          = 0b0000,
  dimBlinkA    = 0b1000,
  dimBlinkB    = 0b0010,
  dim          = 0b1010,
  brightBlinkA = 0b1100,
  brightBlinkB = 0b0011,
  bright       = 0b1111
} illuminationMode_t;

typedef union {
  uint8_t byteval;
  struct {
    uint8_t red : 4;
    uint8_t green : 4;
  } pixelIm;
  struct {
    uint8_t redScan0Frame0 : 1;
    uint8_t redScan1Frame0 : 1;
    uint8_t redScan0Frame1 : 1;
    uint8_t redScan1Frame1 : 1;
    uint8_t greenScan0Frame0 : 1;
    uint8_t greenScan1Frame0 : 1;
    uint8_t greenScan0Frame1 : 1;
    uint8_t greenScan1Frame1 : 1;
  } pixelBits;
} pixel_t;


pixel_t g_pixels[NUM_HW_ROWS][NUM_HW_COLS];


