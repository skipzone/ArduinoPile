
#define TPIC6B595_SERIN 4
#define TPIC6B595_SRCK  8
#define TPIC6B595_RCK   7


void writeShiftReg(uint8_t val) {

  // We drive the eye LEDS by writing 8 bits to a parallel-out serial
  // shift register, such as the 74164 or TPIC6B595.  The register's parallel
  // output drives the LEDs.  The bits are sent in least-to-most-significant
  // order.

  uint8_t i;

  // Shift out the LED bits.
  for (i = 0; i < 8; ++i) {
    // Without any explicit delays, the period between writing the data bit
    // and the leading edge of the clock pulse ends up being much longer
    // than the required 15 ns (74164) OR 20 ns (TPIC6B595) setup+hold time.
    // The clock pulse width is also much greater than the minimum of 20 ns
    // (74164) or 40 ns (TPIC6B595).
    digitalWrite(TPIC6B595_SERIN, val & 0x0001);
    val >>= 1;
    digitalWrite(TPIC6B595_SRCK, 1);
    digitalWrite(TPIC6B595_SRCK, 0);
  }
  digitalWrite(TPIC6B595_SERIN, 0);

  // Clock the shift register into the output register.
  digitalWrite(TPIC6B595_RCK, 1);
  digitalWrite(TPIC6B595_RCK, 0);
}


void setup() {
  pinMode(TPIC6B595_SERIN, OUTPUT);
  pinMode(TPIC6B595_SRCK, OUTPUT);
  pinMode(TPIC6B595_RCK, OUTPUT);
  
  // power-on lamp test
  uint8_t i;
  for (i = 0; i < 8; ++i) {
    writeShiftReg(1 << i);
    delay(500);
  }  

  // Analog input 0 should be disconnected, making it a good source of
  // random noise with which we can seed the random number generator.
  randomSeed(analogRead(0));  
}

void loop() {
  
  static uint8_t displayCount = 0;
  static uint8_t color;

  uint8_t i;  
  uint8_t leds;
  uint8_t newColor;
  
  if (displayCount == 0) {
    displayCount = 3;
    do {
      newColor = random(8);
    } while (newColor == 0 || newColor == color);
    color = newColor;
  }
  --displayCount;
  
  writeShiftReg(0);
  delay(1000);
  leds = color << 2;
  writeShiftReg(leds);
  delay(1000);
  leds |= color << 5;
  writeShiftReg(leds);
  delay(3000);
 
  if (displayCount == 0) {
    for (i = 0; i <  10; ++i) {
      leds = color << 2;
      writeShiftReg(leds);
      delay(random(5,25));
      leds |= color << 5;
      writeShiftReg(leds);
      delay(random(20,100));
    }
    delay(random(300,500));
    for (i = 0; i <  10; ++i) {
      leds = color << 2;
      writeShiftReg(leds);
      delay(random(5,50));
      leds |= color << 5;
      writeShiftReg(leds);
      delay(random(20,100));
    }
    delay(1000);
    writeShiftReg(0);
    delay(1000);
 }
}
