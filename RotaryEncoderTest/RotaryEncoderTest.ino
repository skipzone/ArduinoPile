//From bildr article: http://bildr.org/2012/08/rotary-encoder-arduino/

//these pins can not be changed 2/3 are special pins
int ENCODER_0_A_PIN = A0; //2;
int ENCODER_0_B_PIN = A1; //3;
int ENCODER_0_SW_PIN = A2; //4; //push button switch

// External interrupts only work with pins 2 and 3.
//#define USE_EXTERNAL_INTERRUPTS
//#define HANDLE_PIN_CHANGE_INT_FOR_D8_TO_D13
#define HANDLE_PIN_CHANGE_INT_FOR_A0_TO_A5
//#define HANDLE_PIN_CHANGE_INT_FOR_D0_TO_D7

volatile int lastEncoded = 0;
volatile long encoderValue = 0;

int lastMSB = 0;
int lastLSB = 0;


#ifdef USE_EXTERNAL_INTERRUPTS

void updateEncoder()
{
  int MSB = digitalRead(ENCODER_0_A_PIN); //MSB = most significant bit
  int LSB = digitalRead(ENCODER_0_B_PIN); //LSB = least significant bit

  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;

  lastEncoded = encoded; //store this value for next time
}

#else

void pciSetup(byte pin) 
{
  *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
  PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
  PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group 
}

#endif


#ifdef HANDLE_PIN_CHANGE_INT_FOR_D8_TO_D13
ISR (PCINT0_vect) // handle pin change interrupt for D8 to D13 here
{
  int MSB = digitalRead(ENCODER_0_A_PIN); //MSB = most significant bit
  int LSB = digitalRead(ENCODER_0_B_PIN); //LSB = least significant bit
  int encoded = (MSB << 1) | LSB; //converting the 2 pin value to single number
  int sum = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;

  lastEncoded = encoded; //store this value for next time
}
#endif

#ifdef HANDLE_PIN_CHANGE_INT_FOR_A0_TO_A5
ISR (PCINT1_vect) // handle pin change interrupt for A0 to A5 here
{
  int MSB = digitalRead(ENCODER_0_A_PIN); //MSB = most significant bit
  int LSB = digitalRead(ENCODER_0_B_PIN); //LSB = least significant bit
  int encoded = (MSB << 1) | LSB; //converting the 2 pin value to single number
  int sum = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;

  lastEncoded = encoded; //store this value for next time
}
#endif

#ifdef HANDLE_PIN_CHANGE_INT_FOR_D0_TO_D7
ISR (PCINT2_vect) // handle pin change interrupt for D0 to D7 here
{
  int MSB = digitalRead(ENCODER_0_A_PIN); //MSB = most significant bit
  int LSB = digitalRead(ENCODER_0_B_PIN); //LSB = least significant bit
  int encoded = (MSB << 1) | LSB; //converting the 2 pin value to single number
  int sum = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;

  lastEncoded = encoded; //store this value for next time
}  
#endif


void setup()
{
  Serial.begin (9600);

  pinMode(ENCODER_0_A_PIN, INPUT); 
  pinMode(ENCODER_0_B_PIN, INPUT);
  pinMode(ENCODER_0_SW_PIN, INPUT);
  //pinMode(MODE_PUSHBUTTON_PIN, INPUT_PULLUP);

  digitalWrite(ENCODER_0_A_PIN, HIGH); //turn pullup resistor on
  digitalWrite(ENCODER_0_B_PIN, HIGH); //turn pullup resistor on
  digitalWrite(ENCODER_0_SW_PIN, HIGH); //turn pullup resistor on

#ifdef USE_EXTERNAL_INTERRUPTS
  // call updateEncoder() when any high/low changed seen
  // on interrupt 0 (pin 2), or interrupt 1 (pin 3) 
  attachInterrupt(0, updateEncoder, CHANGE); 
  attachInterrupt(1, updateEncoder, CHANGE);
#else
  pciSetup(ENCODER_0_A_PIN);
  pciSetup(ENCODER_0_B_PIN);
#endif
}


void loop()
{
  static long lastEncoderValue = 0;

  long thisEncoderValue = encoderValue;
  if (thisEncoderValue != lastEncoderValue) {
    lastEncoderValue = thisEncoderValue;
    Serial.println(encoderValue);
  }

  if (digitalRead(ENCODER_0_SW_PIN)) {
    digitalWrite(LED_BUILTIN, LOW);     // turn the LED off
//    PORTB &= ~bit(5);  // turn off pin 13
  }
  else {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on
//    PORTB |= bit(5);  // turn on pin 13
  }
}



