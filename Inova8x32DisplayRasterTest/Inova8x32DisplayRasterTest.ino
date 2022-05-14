#define DS_RED 2
#define DS_GREEN 4
#define STCP_nLE 5
#define SHCP 3
#define nMR 6
#define nE1 7
#define ROWA0 8
#define ROWA1 9
#define ROWA2 10
#define ONBOARD_LED_PIN 13

#define NUM_DISPLAY_ROWS 16
#define NUM_DISPLAY_COLS 32

// Fast pixel index calculation can be enabled when the display dimensions are a power of 2.
#define USE_FAST_PIXEL_IDX_CALC

#include "Inova8x32DisplayRasterTest.h"


ISR(TIMER2_COMPA_vect)
{
  static uint8_t row = 0;
  static uint8_t col = 0;
  static uint16_t frameChangeCount = 0;
  static bool isScan1 = false;
  static bool isFrame1 = false;

  if (++frameChangeCount > 30720 / 2) {
    frameChangeCount = 0;
    isFrame1 = !isFrame1;
  }
  isScan1 = frameChangeCount > 30720 / 8 ? false : true;

  pixel_t pixel;
  pixel.byteval = g_pixels[row][col].byteval;

  uint8_t redVal;
  uint8_t greenVal;
  if (!isFrame1) {
    if (!isScan1) {
      redVal = pixel.pixelBits.redScan0Frame0;
      greenVal = pixel.pixelBits.greenScan0Frame0;
    }
    else {
      redVal = pixel.pixelBits.redScan1Frame0;
      greenVal = pixel.pixelBits.greenScan1Frame0;
    }
  }
  else {
    if (!isScan1) {
      redVal = pixel.pixelBits.redScan0Frame1;
      greenVal = pixel.pixelBits.greenScan0Frame1;
    }
    else {
      redVal = pixel.pixelBits.redScan1Frame1;
      greenVal = pixel.pixelBits.greenScan1Frame1;
    }
  }

  digitalWrite(DS_RED, redVal);
  digitalWrite(DS_GREEN, greenVal);

  // Strobe SHCP to to set shift register stage 0 to DS and shift previous values to next stage.
  digitalWrite(SHCP, 1);
  digitalWrite(SHCP, 0);

  if (++col >= NUM_HW_COLS) {
    col = 0;

    // Turn off the display while we update the row.
    digitalWrite(nE1, 1);

    // Set the address of the row we just loaded and are about to display.
    digitalWrite(ROWA0, row & 0x01);
    digitalWrite(ROWA1, row & 0x02);
    digitalWrite(ROWA2, row & 0x04);

    // Strobe STCP to transfer the shift register to the storage register.
    digitalWrite(STCP_nLE, 1);
    digitalWrite(STCP_nLE, 0);

    // Turn the display back on.
    digitalWrite(nE1, 0);

    // Clear all the shift registers.
    digitalWrite(nMR, 0);
    digitalWrite(nMR, 1);

    if (++row >= NUM_HW_ROWS) {
      row = 0;
//      isScan1 = !isScan1;
    }
  }
}



void clearAllDisplayRegisters(void)
{
  digitalWrite(nMR, 0);
  digitalWrite(STCP_nLE, 1);
  digitalWrite(STCP_nLE, 0);
  digitalWrite(nMR, 1);
}


void clearDisplay(void)
{
  for (uint8_t i = 0; i < NUM_HW_ROWS; ++i) {
    for (uint8_t j = 0; j < NUM_HW_COLS; ++j) {
      g_pixels[i][j].byteval = 0;
    }
  }
}


void setPixel(uint8_t row, uint8_t col, illuminationMode_t redIm, illuminationMode_t greenIm)
{

#ifdef USE_FAST_PIXEL_IDX_CALC 
  uint8_t hwRow = row & HW_ROW_MASK;
  //uint8_t hwCol = (((15 - row) >> HW_ROW_SHIFT) << DISPLAY_COL_SHIFT) + col;
  uint8_t hwCol = (((15 - row) & 0b11111000) << (DISPLAY_COL_SHIFT - HW_ROW_SHIFT)) + col;
#else
  uint8_t hwRow = row % NUM_HW_ROWS;
  uint8_t hwCol = (15 - row) / NUM_HW_ROWS * NUM_DISPLAY_COLS + col;
#endif

  g_pixels[hwRow][hwCol].pixelIm.red = redIm;  
  g_pixels[hwRow][hwCol].pixelIm.green = greenIm;
}


// TODO:  This needs to be rewritten.  Won't work as-is.
//void getPixel(uint8_t row, uint8_t col, uint8_t* pRedVal, uint8_t* pGreenVal)
//{
//  uint16_t pixelIdx = ((uint16_t) row * 32 + (uint16_t) col) >> 1;
//  uint8_t upperPixel = col % 2;
//  
//  if (upperPixel) {
//    *pRedVal = g_pixels[pixelIdx].pixel.r1;
//    *pGreenVal = g_pixels[pixelIdx].pixel.g1;
//  }
//  else {
//    *pRedVal = g_pixels[pixelIdx].pixel.r0;
//    *pGreenVal = g_pixels[pixelIdx].pixel.g0;
//  }
//}



bool walkingLed(bool fill)
{
  // Returns true when the pattern is complete.

  static unsigned long nextChangeMs = 0;
  static uint8_t row = 0;
  static uint8_t col = 0;
  static uint8_t color = 1;  // 1 = red, 2 = green, 3 = yellow

  unsigned long now = millis();
  if (now < nextChangeMs) {
    return false;
  }
  if (row == 0 && col == 0 && color == 1 && nextChangeMs != 0) {
    nextChangeMs = 0;
    return true;
  }
  nextChangeMs = now + (fill ? 5 : 10);
  
  if (!fill) {
    uint8_t prevRow;
    uint8_t prevCol;
    if (col > 0) {
      prevRow = row;
      prevCol = col - 1;
    }
    else {
      prevRow = row > 0 ? row - 1 : NUM_DISPLAY_ROWS - 1;
      prevCol = NUM_DISPLAY_COLS - 1;
    }
    setPixel(prevRow, prevCol, off, off);
  }
  
  setPixel(row, col, color & 0x01 ? bright : off, color & 0x02 ? bright : off);

  if (++col >= NUM_DISPLAY_COLS) {
    col = 0;
    if (++row >= NUM_DISPLAY_ROWS) {
      row = 0;
      if (++color > 3) {
        color = 1;
      }
    }
  }

  return false;
}


bool walkingRow(bool fill)
{
  // Returns true when the pattern is complete.

  static unsigned long nextChangeMs = 0;
  static uint8_t row = 0;
  static uint8_t color = 1;  // 1 = red, 2 = green, 3 = yellow

  unsigned long now = millis();
  if (now < nextChangeMs) {
    return false;
  }
  if (row == 0 && color == 1 && nextChangeMs != 0) {
    nextChangeMs = 0;
    return true;
  }
  nextChangeMs = now + 100;

  uint8_t prevRow = row > 0 ? row - 1 : NUM_DISPLAY_ROWS - 1;
  illuminationMode_t redVal = color & 0x01 ? bright : off;
  illuminationMode_t greenVal = color & 0x02 ? bright : off;
  for (uint8_t col = 0; col < NUM_DISPLAY_COLS; ++col) {
    if (!fill) {
      setPixel(prevRow, col, off, off);
    }
    setPixel(row, col, redVal, greenVal);
  }

  if (++row >= NUM_DISPLAY_ROWS) {
    row = 0;
    if (++color > 3) {
      color = 1;
    }
  }
  
  return false;
}


bool walkingColumn(bool fill)
{
  // Returns true when the pattern is complete.

  static unsigned long nextChangeMs = 0;
  static uint8_t col = 0;
  static uint8_t color = 1;  // 1 = bright red, 2 = bright green, 3 = bright yellow, 4 = mixed

  unsigned long now = millis();
  if (now < nextChangeMs) {
    return false;
  }
  if (col == 0 && color == 1 && nextChangeMs != 0) {
    nextChangeMs = 0;
    return true;
  }
  nextChangeMs = fill && col == NUM_DISPLAY_COLS - 1 ? now + (color < 4 ? 1000 : 15000) : now + 25;

  illuminationMode_t redVal;
  illuminationMode_t greenVal;
  switch(color) {
    case 1:
      redVal = bright;
      greenVal = off;
      break;
    case 2:
      redVal = off;
      greenVal = bright;
      break;
    case 3:
      redVal = bright;
      greenVal = bright;
      break;
    case 4:
      switch(col % 24) {
        case 0:
          redVal = dimBlinkA;
          greenVal = off;
          break;
        case 1:
          redVal = dim;
          greenVal = off;
          break;
        case 2:
          redVal = brightBlinkA;
          greenVal = off;
          break;
        case 3:
          redVal = bright;
          greenVal = off;
          break;
        case 4:
          redVal = off;
          greenVal = dimBlinkA;
          break;
        case 5:
          redVal = dimBlinkA;
          greenVal = dimBlinkA;
          break;
        case 6:
          redVal = dim;
          greenVal = dimBlinkA;
          break;
        case 7:
          redVal = brightBlinkA;
          greenVal = dimBlinkA;
          break;
        case 8:
          redVal = bright;
          greenVal = dimBlinkA;
          break;
        case 9:
          redVal = off;
          greenVal = dim;
          break;
        case 10:
          redVal = dimBlinkA;
          greenVal = dim;
          break;
        case 11:
          redVal = dim;
          greenVal = dim;
          break;
        case 12:
          redVal = brightBlinkA;
          greenVal = dim;
          break;
        case 13:
          redVal = bright;
          greenVal = dim;
          break;
        case 14:
          redVal = off;
          greenVal = brightBlinkA;
          break;
        case 15:
          redVal = dimBlinkA;
          greenVal = brightBlinkA;
          break;
        case 16:
          redVal = dim;
          greenVal = brightBlinkA;
          break;
        case 17:
          redVal = brightBlinkA;
          greenVal = brightBlinkA;
          break;
        case 18:
          redVal = bright;
          greenVal = brightBlinkA;
          break;
        case 19:
          redVal = off;
          greenVal = bright;
          break;
        case 20:
          redVal = dimBlinkA;
          greenVal = bright;
          break;
        case 21:
          redVal = dim;
          greenVal = bright;
          break;
        case 22:
          redVal = brightBlinkA;
          greenVal = bright;
          break;
        case 23:
          redVal = bright;
          greenVal = bright;
          break;
      }
  }
  
  for (uint8_t row = 0; row < NUM_DISPLAY_ROWS; ++row) {
    if (!fill) {
      uint8_t prevCol = col > 0 ? col - 1 : NUM_DISPLAY_COLS - 1;
      setPixel(row, prevCol, off, off);
    }
    setPixel(row, col, redVal, greenVal);
  }

  if (++col >= NUM_DISPLAY_COLS) {
    col = 0;
    if (++color > 4) {
      color = 1;
    }
  }

  return false;
}



void setup() {

  pinMode(DS_RED, OUTPUT);
  pinMode(DS_GREEN, OUTPUT);
  pinMode(STCP_nLE, OUTPUT);
  pinMode(SHCP, OUTPUT);
  pinMode(nMR, OUTPUT);
  pinMode(nE1, OUTPUT);
  pinMode(ROWA0, OUTPUT);
  pinMode(ROWA1, OUTPUT);
  pinMode(ROWA2, OUTPUT);
  pinMode(ONBOARD_LED_PIN, OUTPUT);

  digitalWrite(DS_RED, 0);
  digitalWrite(DS_GREEN, 0);
  digitalWrite(STCP_nLE, 0);
  digitalWrite(SHCP, 0);
  digitalWrite(nMR, 1);
  digitalWrite(nE1, 1);
  digitalWrite(ROWA0, 0);
  digitalWrite(ROWA1, 0);
  digitalWrite(ROWA2, 0);

  clearAllDisplayRegisters();
  clearDisplay();

  noInterrupts();           // disable all interrupts
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2 = 0;
  //OCR2A = 130;              // compare match register = 16MHz / 8 / 15.360 kHz for ~60 fps
  //OCR2A = 65;               // compare match register = 16MHz / 8 / 30.720 kHz for ~120 fps
  OCR2A = 80;
  //OCR2A = 32;               // compare match register = 16MHz / 8 / 61.440 kHz for ~240 fps
  TCCR2A |= (1 << WGM21);   // CTC mode
  TCCR2B |= (1 << CS21);    // 8 prescaler 
  TIMSK2 |= (1 << OCIE2A);  // enable timer compare interrupt
  interrupts();             // enable all interrupts  

  // Turn on the display.
  digitalWrite(nE1, 0);
}



void loop() {
  static uint8_t patternNum = 0;

  bool changePattern;
  
  switch(patternNum) {
    case 0:
      changePattern = walkingLed(false);
      break;
    case 1:
      changePattern = walkingLed(true);
      break;
    case 2:
      changePattern = walkingRow(false);
      break;
    case 3:
      changePattern = walkingRow(true);
      break;
    case 4:
      changePattern = walkingColumn(false);
      break;
    case 5:
      changePattern = walkingColumn(true);
      break;
    case 98:
      setPixel(0, 0, bright, off);  // (0,0) is bright red
      setPixel(0, 1, dim, off);  // (0,1) is dim red
      setPixel(0, 30, off, dim);  // (0,30) is dim green
      setPixel(0, 31, off, bright);  // (0,31) is bright green
      setPixel(8, 31, bright, bright);  // (8,31) is yellow
      setPixel(15, 0, bright, brightBlinkA);  // (15,0) is red/yellow
      setPixel(15, 31, off, bright);  // (15,31) is green
      break;
    case 99:
      g_pixels[0][0].pixelIm.red = 0b1111;  // 0 is bright red
      g_pixels[0][0].pixelIm.green = 0b0000;

      g_pixels[0][1].byteval = 0;
      g_pixels[0][1].pixelBits.redScan0Frame0 = 0;
      g_pixels[0][1].pixelBits.redScan1Frame0 = 1;  // 0 is dim red
      g_pixels[0][1].pixelIm.green = 0b0000;

      g_pixels[0][31].pixelIm.red = off;
      g_pixels[0][31].pixelIm.green = bright;  // 31 is green

      g_pixels[1][0].pixelIm.red = bright;  // 32 is red
      g_pixels[1][0].pixelIm.green = off;

      g_pixels[1][31].pixelIm.red = bright;  // 63 is yellow
      g_pixels[1][31].pixelIm.green = bright;
      break;
  }

  if (changePattern) {
    if (++patternNum > 5) {
      patternNum = 0;
    }
    clearDisplay();
  }
}
