

#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>


#define YP_PIN A3   // Y+, must be an analog pin
#define XM_PIN A2   // X-, must be an analog pin
#define YM_PIN 9    // Y-, can be a digital pin
#define XP_PIN 8    // X+, can be a digital pin

#define TS_X_PLATE_RESISTANCE_OHMS 300

#define MINPRESSURE 10
#define MAXPRESSURE 1000

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

// common 16-bit color values
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Note that it is assumed that the touch coordinates are always based on
// portrait orientation and do not change with the display's orientation.

// for Elegoo UNO R3 2.8 Inches TFT Touch Screen
static constexpr uint8_t rotationLandscape = 3;
static constexpr uint8_t rotationPortrait = 2;
static constexpr uint8_t rotationLandscapeUpsideDown = 1;
static constexpr uint8_t rotationPortraitUpsideDown = 0;
// for mcufriend.com 2.4" TFT LCD Shield (9325)
//static constexpr uint8_t rotationLandscape = 1;
//static constexpr uint8_t rotationPortrait = 0;
//static constexpr uint8_t rotationLandscapeUpsideDown = 3;
//static constexpr uint8_t rotationPortraitUpsideDown = 2;

static constexpr uint8_t lcdRotation = rotationPortrait;

static constexpr uint16_t dotColor = MAGENTA;
static constexpr uint16_t dotRadius = 3;
static constexpr uint16_t drawColor = CYAN;
static constexpr uint16_t drawRadius = 1;
static constexpr uint16_t textFgColor = WHITE;
static constexpr uint16_t textBgColor = BLACK;
static constexpr uint8_t textSize = 2;

enum class OperatingState {
  INIT,
  UPPER_LEFT_START,
  UPPER_LEFT_WAIT_FOR_TAP,
  LOWER_LEFT_START,
  LOWER_LEFT_WAIT_FOR_TAP,
  UPPER_RIGHT_START,
  UPPER_RIGHT_WAIT_FOR_TAP,
  LOWER_RIGHT_START,
  LOWER_RIGHT_WAIT_FOR_TAP,
  SHOW_RESULTS,
  TEST,
  DONE
};


static Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
static TouchScreen ts = TouchScreen(XP_PIN, YP_PIN, XM_PIN, YM_PIN, TS_X_PLATE_RESISTANCE_OHMS);
static OperatingState opState;


void setup(void)
{
  Serial.begin(115200);
  
  uint16_t identifier = tft.readID();
  Serial.print(F("LCD indentifier = 0x"));
  Serial.println(identifier, HEX);
  tft.begin(identifier);
  tft.setRotation(lcdRotation);

  opState = OperatingState::INIT;
}


TSPoint getPoint()
{
  TSPoint p = ts.getPoint();

  // XM and YP are ADC pins used for LCD CD and CS.  XP and YM are digital pins
  // used for LCD D0 and D1.  We must set the ADC pins back to (digital) outputs.
  // We don't have to set the digital pins back to outputs if we're willing to
  // trust TouchScreen::getPoint to leave them in that state.
  pinMode(XM_PIN, OUTPUT);
  pinMode(YP_PIN, OUTPUT);
  pinMode(XP_PIN, OUTPUT);
  pinMode(YM_PIN, OUTPUT);

  return p;
}


void loop()
{
  static TSPoint p;
  static uint16_t xMin;
  static uint16_t xMin1;
  static uint16_t xMin2;
  static uint16_t xMax;
  static uint16_t xMax1;
  static uint16_t xMax2;
  static uint16_t yMin;
  static uint16_t yMin1;
  static uint16_t yMin2;
  static uint16_t yMax;
  static uint16_t yMax1;
  static uint16_t yMax2;

  uint16_t cursorX;
  uint16_t cursorY;
  uint16_t drawX;
  uint16_t drawY;

  switch(opState) {

    case OperatingState::INIT:
      tft.setTextColor(textFgColor);
      tft.setTextSize(textSize);
      opState = OperatingState::UPPER_LEFT_START;
      break;

    case OperatingState::UPPER_LEFT_START:
      tft.fillScreen(BLACK);
      tft.fillCircle(dotRadius, dotRadius, dotRadius, dotColor);
      tft.setCursor(0, tft.height() / 2);
      tft.print(F("Tap on dot in upper left corner."));
      p = getPoint();
      // Start waiting for the tap if the screen isn't being touched.
      if (p.z <= MINPRESSURE || p.z >= MAXPRESSURE) {
        opState = OperatingState::UPPER_LEFT_WAIT_FOR_TAP;
      }
      break;

    case OperatingState::UPPER_LEFT_WAIT_FOR_TAP:
      p = getPoint();
      if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
        Serial.print("(");
        Serial.print(p.x);
        Serial.print(",");
        Serial.print(p.y);
        Serial.println(")");
        if (lcdRotation == rotationPortrait || lcdRotation == rotationPortraitUpsideDown) {
          xMin1 = p.x - dotRadius;
          yMin1 = p.y - dotRadius;
        }
        else {
          xMin1 = p.x - dotRadius;
          yMin1 = p.y - dotRadius;
        }
        opState = OperatingState::LOWER_LEFT_START;
      }
      break;

    case OperatingState::LOWER_LEFT_START:
      tft.fillScreen(BLACK);
      tft.fillCircle(dotRadius, tft.height() - dotRadius, dotRadius, dotColor);
      tft.setCursor(0, tft.height() / 2);
      tft.print(F("Tap on dot in lower left corner."));
      p = getPoint();
      // Start waiting for the tap if the screen isn't being touched.
      if (p.z <= MINPRESSURE || p.z >= MAXPRESSURE) {
        opState = OperatingState::LOWER_LEFT_WAIT_FOR_TAP;
      }
      break;

    case OperatingState::LOWER_LEFT_WAIT_FOR_TAP:
      p = getPoint();
      if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
        Serial.print("(");
        Serial.print(p.x);
        Serial.print(",");
        Serial.print(p.y);
        Serial.println(")");
        if (lcdRotation == rotationPortrait || lcdRotation == rotationPortraitUpsideDown) {
          xMin2 = p.x - dotRadius;
          yMax1 = p.y - dotRadius;
        }
        else {
          xMax1 = p.x - dotRadius;
          yMin2 = p.y - dotRadius;
        }
        opState = OperatingState::UPPER_RIGHT_START;
      }
      break;

    case OperatingState::UPPER_RIGHT_START:
      tft.fillScreen(BLACK);
      tft.fillCircle(tft.width() - dotRadius, dotRadius, dotRadius, dotColor);
      tft.setCursor(0, tft.height() / 2);
      tft.print(F("Tap on dot in upper right corner."));
      p = getPoint();
      // Start waiting for the tap if the screen isn't being touched.
      if (p.z <= MINPRESSURE || p.z >= MAXPRESSURE) {
        opState = OperatingState::UPPER_RIGHT_WAIT_FOR_TAP;
      }
      break;

    case OperatingState::UPPER_RIGHT_WAIT_FOR_TAP:
      p = getPoint();
      if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
        Serial.print("(");
        Serial.print(p.x);
        Serial.print(",");
        Serial.print(p.y);
        Serial.println(")");
        if (lcdRotation == rotationPortrait || lcdRotation == rotationPortraitUpsideDown) {
          xMax1 = p.x - dotRadius;
          yMin2 = p.y - dotRadius;
        }
        else {
          xMin2 = p.x - dotRadius;
          yMax1 = p.y - dotRadius;
        }
        opState = OperatingState::LOWER_RIGHT_START;
      }
      break;

    case OperatingState::LOWER_RIGHT_START:
      tft.fillScreen(BLACK);
      tft.fillCircle(tft.width() - dotRadius, tft.height() - dotRadius, dotRadius, dotColor);
      tft.setCursor(0, tft.height() / 2);
      tft.print(F("Tap on dot in lower right corner."));
      p = getPoint();
      // Start waiting for the tap if the screen isn't being touched.
      if (p.z <= MINPRESSURE || p.z >= MAXPRESSURE) {
        opState = OperatingState::LOWER_RIGHT_WAIT_FOR_TAP;
      }
      break;

    case OperatingState::LOWER_RIGHT_WAIT_FOR_TAP:
      p = getPoint();
      if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
        Serial.print("(");
        Serial.print(p.x);
        Serial.print(",");
        Serial.print(p.y);
        Serial.println(")");
        if (lcdRotation == rotationPortrait || lcdRotation == rotationPortraitUpsideDown) {
          xMax2 = p.x - dotRadius;
          yMax2 = p.y - dotRadius;
        }
        else {
          xMax2 = p.x - dotRadius;
          yMax2 = p.y - dotRadius;
        }
        opState = OperatingState::SHOW_RESULTS;
      }
      break;

    case OperatingState::SHOW_RESULTS:
      xMin = (xMin1 + xMin2) / 2;
      xMax = (xMax1 + xMax2) / 2;
      yMin = (yMin1 + yMin2) / 2;
      yMax = (yMax1 + yMax2) / 2;
      tft.fillScreen(BLACK);
      cursorX = tft.width() / 4;
      cursorY = (8 + 2) * textSize;     // start one line down from top
      tft.setCursor(cursorX, cursorY);
      tft.print(F("xMin = "));
      tft.print(xMin);
      cursorY += (8 + 2) * textSize;    // 8 is basis height of character cell, 2 is for spacing between lines
      tft.setCursor(cursorX, cursorY);
      tft.print(F("xMax = "));
      tft.print(xMax);
      cursorY += (8 + 2) * textSize;
      tft.setCursor(cursorX, cursorY);
      tft.print(F("yMin = "));
      tft.print(yMin);
      cursorY += (8 + 2) * textSize;
      tft.setCursor(cursorX, cursorY);
      tft.print(F("yMax = "));
      tft.print(yMax);
      cursorY += (8 + 2) * textSize * 2;
      tft.setCursor(0, cursorY);
      tft.print(F("Verify by drawing."));
      opState = OperatingState::TEST;
      break;

    case OperatingState::TEST:
      p = getPoint();
      if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
//        // press the bottom of the screen to erase 
//        if (p.y < (TS_MINY + ERASE_ZONE_HEIGHT)) {
//          Serial.println("erase");
//          tft.fillRect(0, BOXSIZE, tft.width(), tft.height()-BOXSIZE, BLACK);
//        }
        if (lcdRotation == rotationPortrait || lcdRotation == rotationPortraitUpsideDown) {
          drawX = map(p.x, xMin, xMax, 0, tft.width());
          drawY = map(p.y, yMin, yMax, 0, tft.height());
        }
        else {
          drawX = map(p.y, yMin, yMax, 0, tft.width());
          drawY = map(p.x, xMin, xMax, 0, tft.height());
        }
        tft.fillCircle(drawX, drawY, drawRadius, drawColor);
      }
      break;

    case OperatingState::DONE:
      break;
  }

}

