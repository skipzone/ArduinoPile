#include <RBD_Button.h>


constexpr uint8_t pinSw0 = 2;
constexpr uint8_t pinSw1 = 4;
constexpr uint8_t pinSw2 = 7;
constexpr uint8_t pinSw3 = 8;
constexpr uint8_t pinSw4 = A5;
constexpr uint8_t pinSw5 = A4;
constexpr uint8_t pinSw6 = A0;
constexpr uint8_t pinSw7 = A1;
constexpr uint8_t pinButton = A3;

constexpr uint8_t pinPot1 = A2;
constexpr uint8_t pinPot2 = A7;
constexpr uint8_t pinPot3 = A6;

constexpr uint8_t pinLedRed = 3;
constexpr uint8_t pinLedGreen = 5;
constexpr uint8_t pinLedBlue = 6;

constexpr uint8_t swOn = LOW;
constexpr uint8_t swOff = HIGH;


static RBD::Button sw0(pinSw0, true);  // true -> enable internal pullup
static RBD::Button sw1(pinSw1, true);  // true -> enable internal pullup
static RBD::Button sw2(pinSw2, true);  // true -> enable internal pullup
static RBD::Button sw3(pinSw3, true);  // true -> enable internal pullup
static RBD::Button sw4(pinSw4, true);  // true -> enable internal pullup
static RBD::Button sw5(pinSw5, true);  // true -> enable internal pullup
static RBD::Button sw6(pinSw6, true);  // true -> enable internal pullup
static RBD::Button sw7(pinSw7, true);  // true -> enable internal pullup
static RBD::Button button(pinButton, true);  // true -> enable internal pullup


void setup()
{
  Serial.begin(9600);

  pinMode(pinPot1, INPUT);
  pinMode(pinPot2, INPUT);
  pinMode(pinPot3, INPUT);

  pinMode(pinLedRed, OUTPUT);
  pinMode(pinLedGreen, OUTPUT);
  pinMode(pinLedBlue, OUTPUT);

  analogWrite(pinLedRed, 0);
  analogWrite(pinLedGreen, 0);
  analogWrite(pinLedBlue, 0);
}


void loop()
{
  analogWrite(pinLedRed, analogRead(pinPot1) >> 2);
  analogWrite(pinLedGreen, analogRead(pinPot2) >> 2);
  analogWrite(pinLedBlue, analogRead(pinPot3) >> 2);

  if (sw0.onPressed()) Serial.println("switch 0 on");
  if (sw1.onPressed()) Serial.println("switch 1 on");
  if (sw2.onPressed()) Serial.println("switch 2 on");
  if (sw3.onPressed()) Serial.println("switch 3 on");
  if (sw4.onPressed()) Serial.println("switch 4 on");
  if (sw5.onPressed()) Serial.println("switch 5 on");
  if (sw6.onPressed()) Serial.println("switch 6 on");
  if (sw7.onPressed()) Serial.println("switch 7 on");
  if (button.onPressed()) Serial.println("button pressed");
  
  if (sw0.onReleased()) Serial.println("switch 0 off");
  if (sw1.onReleased()) Serial.println("switch 1 off");
  if (sw2.onReleased()) Serial.println("switch 2 off");
  if (sw3.onReleased()) Serial.println("switch 3 off");
  if (sw4.onReleased()) Serial.println("switch 4 off");
  if (sw5.onReleased()) Serial.println("switch 5 off");
  if (sw6.onReleased()) Serial.println("switch 6 off");
  if (sw7.onReleased()) Serial.println("switch 7 off");
  if (button.onReleased()) Serial.println("button released");
}
