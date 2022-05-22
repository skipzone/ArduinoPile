// ---------------------------------------------------------------------------
// Example NewPing library sketch that does a ping about 20 times per second.
// ---------------------------------------------------------------------------

#define PRINT_TO_SERIAL
//#define ENABLE_LCD

#ifdef ENABLE_LCD
#include <LiquidCrystal.h>
#endif

//#include "FastLED.h"

#define MIC_PIN A3

#define SAMPLE_PERIOD_MS 200L

#ifdef ENABLE_LCD
// RS, E, D4, D5, D6, D7
LiquidCrystal lcd(A1, A0, 3, 4, 5, 6);
#endif

//#define NUM_LEDS 8
//#define LED_DATA_PIN 2
//CRGB leds[NUM_LEDS];

#define GOOD_MEASUREMENT_SUSTAIN_MS 500L


void setup() {

#ifdef PRINT_TO_SERIAL
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
#endif

#ifdef ENABLE_LCD
  // set up the LCD's number of columns and rows: 
  lcd.begin(20, 4);
  lcd.setCursor(3, 0);
  lcd.print("Mic Test For");
  lcd.setCursor(1, 1);
  lcd.print("Bells and Plunger");
//  lcd.setCursor(0, 2);
//  lcd.print("== W I D G E T S ==");
#endif

//  FastLED.addLeds<WS2812B, LED_DATA_PIN, GRB>(leds, NUM_LEDS); // 60 pixels/m 4m 5 V white strips (devel. strip, lampshade hat); Joule strips
}


void loop() {

  static uint16_t minSoundSample = UINT16_MAX;
  static uint16_t maxSoundSample;
  static uint16_t numSamples;
  static unsigned long samplePeriodStartMs;

  unsigned long now = millis();

  if (now - samplePeriodStartMs > SAMPLE_PERIOD_MS) {

    uint16_t pp = maxSoundSample - minSoundSample;

#ifdef PRINT_TO_SERIAL
    Serial.print(numSamples);
    Serial.print(" ");
    Serial.print(maxSoundSample);
    Serial.print(" ");
    Serial.print(minSoundSample);
    Serial.print(" ");
    Serial.println(pp);
#endif

#ifdef ENABLE_LCD
    uint16_t numBars = pp / 51 + 1;
    if (numBars > 20) numBars = 20;
    char bars[21];
    memset(bars, ' ', 20);
    memset(bars, '*', numBars);
    bars[numBars] = 0;
    lcd.setCursor(0, 2);  // zero-based col, line
    lcd.print("                    ");
    lcd.setCursor(0, 2);  // zero-based col, line
    lcd.print(bars);

    lcd.setCursor(0, 3);  // zero-based col, line
    lcd.print("                    ");
    lcd.setCursor(0, 3);  // zero-based col, line
    lcd.print(numSamples);
    lcd.print(" ");
    lcd.print(maxSoundSample);
    lcd.print(" ");
    lcd.print(minSoundSample);
    lcd.print(" ");
    lcd.print(pp);
#endif
    
    minSoundSample = UINT16_MAX;
    maxSoundSample = 0;
    numSamples = 0;
    samplePeriodStartMs = now;
  }

  ++numSamples;
  unsigned int soundSample = analogRead(MIC_PIN);
  if (soundSample < minSoundSample) {
    minSoundSample = soundSample;
  }
  if (soundSample > maxSoundSample) {
    maxSoundSample = soundSample;
  }

/*
  if (distance[0] == 0) {
    leds[0] = CRGB::Black;
  }
  else if (distance[0] < 10) {
    leds[0] = CRGB::Blue;
  }
  else if (distance[0] < 20) {
    leds[0] = CRGB::Cyan;
  }
  else if (distance[0] < 30) {
    leds[0] = CRGB::Green;
  }
  else if (distance[0] < 40) {
    leds[0] = CRGB::Yellow;
  }
  else if (distance[0] < 50) {
    leds[0] = CRGB::Red;
  }
  else if (distance[0] < 60) {
    leds[0] = CRGB::Magenta;
  }
  else  {
    leds[0] = CRGB::White;
  }
*/

/*
  CHSV hsvColor;
  hsvColor.h = (HSVHue) distance[0] * 5;
  hsvColor.s = 255;
  hsvColor.v = distance[1] * 2;
  hsv2rgb_rainbow(hsvColor, leds[0]);
  for (uint8_t i = 1; i < NUM_LEDS; leds[i++] = leds[0]);
  FastLED.show();
*/

}

