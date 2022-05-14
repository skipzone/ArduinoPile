/*
  LiquidCrystal Library - Hello World
 
 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the 
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.
 
 This sketch prints "Hello World!" to the LCD
 and shows the time.
 
  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 
 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe
 
 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

//#define LCD_20x4
#define LCD_16x2

// include the library code:
#include <LiquidCrystal.h>

// RS, E, D4, D5, D6, D7
//LiquidCrystal lcd(8, 9, 10, 11, 12, 13);
LiquidCrystal lcd(A1 , A0, 5, 6, 7, 8);

void setup()
{
#if defined(LCD_20x4)

  // set up the LCD's number of columns and rows: 
  lcd.begin(20, 4);
  lcd.setCursor(3, 0);
  lcd.print("Hello, world!");
  lcd.setCursor(0,1);
  lcd.print("This is the 2nd line");
  lcd.setCursor(0,2);
  lcd.print("Line 3 is here-----X");

#elif defined(LCD_16x2)

  // set up the LCD's number of columns and rows: 
  lcd.begin(20, 2);

  lcd.setCursor(0, 0);
  lcd.print("Hi, world!");
  
  lcd.setCursor(0,1);
  lcd.print("--2nd line here-");

#endif
}

void loop()
{
#if defined(LCD_20x4)
  lcd.setCursor(10, 3);  // zero-based col, line
#elif defined(LCD_16x2)
  lcd.setCursor(10, 0);  // zero-based col, line
#endif
  lcd.print(millis());
}

