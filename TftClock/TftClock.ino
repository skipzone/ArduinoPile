// Date and time functions using a DS1307 RTC connected via I2C and Wire lib

#include <Elegoo_GFX.h>     // core graphics library
//#include <Adafruit_GFX.h>     // core graphics library
#include <Elegoo_TFTLCD.h>    // hardware-specific library
//#include <MCUFRIEND_kbv.h>    // hardware-specific library
#include <Wire.h>
#include "RTClib.h"


#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

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


static Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
//static MCUFRIEND_kbv tft;

static DS1307 rtc;


void setup ()
{
  Serial.begin(9600);

  Wire.begin();
  rtc.begin();
  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running!  Starting RTC and setting date and time to when this sketch was compiled.");
    rtc.adjust(DateTime(__DATE__, __TIME__));
  }

  Serial.print("TFT size is ");
  Serial.print(tft.width());
  Serial.print("x");
  Serial.println(tft.height());
  uint16_t identifier = tft.readID();
  Serial.print(F("LCD indentifier = 0x"));
  Serial.println(identifier, HEX);
//  if (identifier == 0x101) {
//    The identifier ends up being 0x101 if tft.reset() is called without a sufficient delay afterward.
//    Serial.println(F("Changing LCD identifier to 0x9341 for Elegoo UNO R3 2.8 Inches TFT Touch Screen."));
//    identifier = 0x9341;
//  }
  if (identifier == 0xD3D3) {
    Serial.println(F("Changing LCD identifier to 0x9481 for mcufriend.com write-only shield."));
    identifier = 0x9481;
  }
  tft.begin(identifier);
  tft.setRotation(rotationLandscape);
  tft.fillScreen(BLACK);
}


void loop ()
{
  DateTime now = rtc.now();

  char formattedDateTime[20];
  strcpy_P(formattedDateTime, PSTR("YYYY-MM-DD hh:mm:ss"));
  Serial.println(now.format(formattedDateTime));

  tft.setTextColor(WHITE, BLACK);     // Specifying foreground and background causes opaque write instead of transparent.
  tft.setTextSize(2);
  tft.setCursor(0, 0);
  tft.print(formattedDateTime);

  delay(1000);
}

