/***********
 * Options *
 ***********/

//#define ENABLE_PRINT_TO_SERIAL


/************
 * Includes *
 ************/

#include <avr/pgmspace.h>
#include <LiquidCrystal.h>


/*****************
 * Configuration *
 *****************/

constexpr uint8_t lcd_numRows = 4;
constexpr uint8_t lcd_numCols = 20;

//constexpr uint8_t lcd_pin_rs = 8;
//constexpr uint8_t lcd_pin_e = 9;
//constexpr uint8_t lcd_pin_d4 = 10;
//constexpr uint8_t lcd_pin_d5 = 11;
//constexpr uint8_t lcd_pin_d6 = 12;
//constexpr uint8_t lcd_pin_d7 = 13;

constexpr uint8_t lcd_pin_rs = A1;
constexpr uint8_t lcd_pin_e = A0;
constexpr uint8_t lcd_pin_d4 = 3;
constexpr uint8_t lcd_pin_d5 = 4;
constexpr uint8_t lcd_pin_d6 = 5;
constexpr uint8_t lcd_pin_d7 = 6;


/*************
 * Constants *
 *************/

const uint8_t PROGMEM lcdCustomChars[8][8] = {
  // happy cat, upper left
  { 0b01000,
    0b01100,
    0b10010,
    0b10001,
    0b10100,
    0b11000,
    0b10010 },
  // happy cat, upper right
  { 0b00010,
    0b00110,
    0b01001,
    0b10001,
    0b00101,
    0b00011,
    0b01001 },
  // happy cat, lower left
  { 0b10100,
    0b10010,
    0b00001,
    0b01000,
    0b00100,
    0b00011,
    0b00000 },
  // happy cat, lower right
  { 0b00101,
    0b01001,
    0b10000,
    0b00010,
    0b00100,
    0b11000,
    0b00000 },
  // sad cat, upper left
  { 0b01000,
    0b01100,
    0b10010,
    0b10001,
    0b10100,
    0b11000,
    0b10010 },
  // sad cat, upper right
  { 0b00010,
    0b00110,
    0b01001,
    0b10001,
    0b00101,
    0b00011,
    0b01001 },
  // sad cat, lower left
  { 0b10001,
    0b10010,
    0b00000,
    0b01000,
    0b00100,
    0b00011,
    0b00000 },
  // sad cat, lower right
  { 0b10001,
    0b01001,
    0b00000,
    0b00010,
    0b00100,
    0b11000,
    0b00000 },
};


/***********
 * Globals *
 ***********/

static LiquidCrystal lcd(lcd_pin_rs, lcd_pin_e, lcd_pin_d4, lcd_pin_d5, lcd_pin_d6, lcd_pin_d7);


/***********
 * Helpers *
 ***********/

void lcdClearScreen() {

  lcd.clear();
  delay(1);
  lcd.setCursor(0, 0);  // zero-based col, line; (0, 0) is upper left corner
  lcd.print(F("CAT SCANner 9000"));
}


void lcdClearLine(uint8_t line) {

    lcd.setCursor(0, line);
    for (uint8_t i = 0; i < lcd_numCols; ++i) {
      lcd.write((uint8_t) ' ');
    }
}


void lcdWriteLine(uint8_t line, const char* text) {

  lcd.setCursor(0, line);
  lcd.print(text);
  uint8_t i = lcd_numCols - strlen(text);
  while (i != 0) {
    lcd.write(' ');
    --i;
  }
}


void drawCat(uint8_t col, uint8_t line, bool isHappy) {

  uint8_t i = isHappy ? 0 : 4;
  lcd.setCursor(col, line);
  lcd.write(i++);
  lcd.write(i++);
  lcd.setCursor(col, line + 1);
  lcd.write(i++);
  lcd.write(i);
}


void lcdShowCatProgress(int8_t stepNum, bool isHappy) {

  if (stepNum < 0) {
    uint8_t numSteps = -stepNum;
    // Clear the lines and draw the boundary bars.
    lcdClearLine(2);
    lcdClearLine(3);
    lcd.setCursor(0, 2);
    lcd.write(0xff);
    lcd.setCursor(0, 3);
    lcd.write(0xff);
    lcd.setCursor(1 + (numSteps << 1), 2);
    lcd.write(0xff);
    lcd.setCursor(1 + (numSteps << 1), 3);
    lcd.write(0xff);
  }
  else {
    drawCat(1 + (stepNum << 1), 2, isHappy);
  }
}


void lcdTest() {

  lcd.clear();
  delay(1);
  lcd.setCursor(0, 0);  // zero-based col, line; (0, 0) is upper left corner
  lcd.print(F("0123456789ABCDEF"));
  for (uint8_t i = 0; i < lcd_numCols - 16; ++i) lcd.write('+');
  lcd.setCursor(0, 1);
  lcd.print(F("-----Line 2-----"));
  for (uint8_t i = 0; i < lcd_numCols - 16; ++i) lcd.write('+');
  if (lcd_numRows == 4) {
    lcd.setCursor(0, 2);
    lcd.print(F("-----Line 3-----"));
    for (uint8_t i = 0; i < lcd_numCols - 16; ++i) lcd.write('+');
    lcd.setCursor(0, 3);
    lcd.print(F("-----Line 4-----"));
    for (uint8_t i = 0; i < lcd_numCols - 16; ++i) lcd.write('+');
    lcd.setCursor(lcd_numCols - 1, 3);
    lcd.write((uint8_t) 0xfc);  // ROM code A00:  little table    ROM code A02:  u with umlaut
    lcd.setCursor(lcd_numCols - 1, 2);
    lcd.write((uint8_t) 0xfd);  // ROM code A00:  division sign   ROM code A02:  y with accent
  }
  lcd.setCursor(lcd_numCols - 1, 1);
  lcd.write((uint8_t) 0xfe);  // ROM code A00:  blank           ROM code A02:  l with titty
  lcd.setCursor(lcd_numCols - 1, 0);
  lcd.write((uint8_t) 0xff);  // ROM code A00:  block           ROM code A02:  y with umlaut
  if (lcd_numRows == 4) {
    lcd.setCursor(0, 3);
    lcd.print(F("z"));
    lcd.setCursor(0, 2);
    lcd.print(F("y"));
  }
  lcd.setCursor(0, 1);
  lcd.print(F("x"));
  lcd.setCursor(0, 0);
  lcd.print(F("w"));
  if (lcd_numRows == 4) {
    lcd.setCursor(9, 3);
    lcd.print(F("|"));
    lcd.setCursor(9, 2);
    lcd.print(F("|"));
  }
  lcd.setCursor(9, 1);
  lcd.print(F("|"));
  lcd.setCursor(9, 0);
  lcd.print(F("|"));
}


/******************
 * Initialization *
 ******************/

void initLcd() {

  lcd.begin(lcd_numCols, lcd_numRows);

  // Download cat characters to the LCD controller.
  for (uint8_t i = 0; i < 8; ++i) {
    uint8_t buf[8];
    memcpy_P(buf, lcdCustomChars[i], 8);
    delay(1);
    lcd.createChar(i, buf);
  }

  // We never want scrolling.
  lcd.noAutoscroll();
}


void setup() {

#ifdef ENABLE_PRINT_TO_SERIAL
  Serial.begin(115200);
#endif

  initLcd();
  lcdTest();
  drawCat(2, 0, true);
  drawCat(12, 0, false);
}


/************
 * Run Loop *
 ************/

void loop() {

//lcdWriteLine(1, "Here pussy pussy");

}
