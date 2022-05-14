// ---------------------------------------------------------------------------
// Example NewPing library sketch that does a ping about 20 times per second.
// ---------------------------------------------------------------------------

#include <NewPing.h>
#include <LiquidCrystal.h>
#include "FastLED.h"

#define NUM_SENSORS 2
#define TRIGGER_PIN_0  7  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN_0     8  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE_0 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define TRIGGER_PIN_1  A2  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN_1     A3 // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE_1 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#define PRINT_TO_SERIAL

NewPing sonar[NUM_SENSORS] = {
  NewPing(TRIGGER_PIN_0, ECHO_PIN_0, MAX_DISTANCE_0),
  NewPing(TRIGGER_PIN_1, ECHO_PIN_1, MAX_DISTANCE_1)
};

// RS, E, D4, D5, D6, D7
LiquidCrystal lcd(A1, A0, 3, 4, 5, 6);

#define NUM_LEDS 8
#define LED_DATA_PIN 2
CRGB leds[NUM_LEDS];

#define GOOD_MEASUREMENT_SUSTAIN_MS 500L
int lastGoodMeasurement[NUM_SENSORS] = {0, 0};
unsigned long lastGoodMeasurementMs[NUM_SENSORS];

void setup() {

#ifdef PRINT_TO_SERIAL
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
#endif

  // set up the LCD's number of columns and rows: 
  lcd.begin(20, 4);
  lcd.setCursor(2, 0);
  lcd.print("HCSR04 Test For");
  lcd.setCursor(0, 1);
  lcd.print("Kelli's & Cowboys's");
  lcd.setCursor(0, 2);
  lcd.print("== W I D G E T S ==");

  FastLED.addLeds<WS2812B, LED_DATA_PIN, GRB>(leds, NUM_LEDS); // 60 pixels/m 4m 5 V white strips (devel. strip, lampshade hat); Joule strips
}

void loop() {

  delay(500);                     // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.

#ifdef PRINT_TO_SERIAL
  Serial.print("Ping: ");
#endif

  unsigned long now = millis();

  int distance[NUM_SENSORS];
  for (int i = 0; i < NUM_SENSORS; ++i) {
    distance[i] = sonar[i].ping_cm();
    if (distance[i] != 0) {
      lastGoodMeasurement[i] = distance[i];
      lastGoodMeasurementMs[i] = now;
    }
    else {
      if (now - lastGoodMeasurementMs[i] <= GOOD_MEASUREMENT_SUSTAIN_MS) {
        distance[i] = lastGoodMeasurement[i];
      }
    }
  }

#ifdef PRINT_TO_SERIAL  
  Serial.print(distance[0]);
  Serial.print(", ");
  Serial.print(distance[1]);
  Serial.println("cm");
#endif

  lcd.setCursor(0, 3);  // zero-based col, line
  lcd.print("                    ");
  lcd.setCursor(0, 3);  // zero-based col, line
  for (int i = 0; i < NUM_SENSORS; ++i) {
    lcd.print(distance[i]);
    lcd.print(" ");
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

