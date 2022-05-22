
#include <SoftwareSerial.h>


#define HC12_TX_PIN 2
#define HC12_RX_PIN 4
#define TX_LED_PIN A2
#define TX_LED_ON HIGH
#define TX_LED_OFF LOW


String rxStr;

SoftwareSerial HC12(HC12_TX_PIN, HC12_RX_PIN);  // Arduino-side RX, TX


void setup()
{
 Serial.begin(9600);
 HC12.begin(9600);

 pinMode(TX_LED_PIN, OUTPUT);
 digitalWrite(TX_LED_PIN, TX_LED_OFF);
}


void loop()
{
  constexpr uint32_t txIntervalMs = 1000;
  static uint32_t lastTxMs;

  uint32_t now = millis();
  
  while (HC12.available()) {
    byte rxb = HC12.read();
    Serial.print("rx 0x");
    Serial.print((int) rxb, HEX);
    Serial.print(" ");
    if (rxb >= 0x20 && rxb <= 0x7e) {   // is a printable character?
      Serial.println(rxb);
    }
    else {
      Serial.println("--");
    }
  }

  if (now - lastTxMs > txIntervalMs) {
    lastTxMs = now;
    digitalWrite(TX_LED_PIN, TX_LED_ON);
    HC12.println("Hello radioland!");
    digitalWrite(TX_LED_PIN, TX_LED_OFF);
  }
}

