#define TPIC6B595_SERIN 5
#define TPIC6B595_SRCK  6
#define TPIC6B595_RCK   7

#define NUM_SSDISPLAY_DIGITS 3
#define NUM_SEVEN_SEG_DIGITS 3


typedef struct ssDisplay_struct {
  char digitChar[NUM_SSDISPLAY_DIGITS];
  uint8_t digitBuf[NUM_SSDISPLAY_DIGITS];
  uint8_t digitSegToSRSegMap[NUM_SSDISPLAY_DIGITS][8];
} ssDisplay_t;

typedef ssDisplay_t* ssDisplay_p;

ssDisplay_t ssDisplay;

uint8_t ssDispBuf[NUM_SEVEN_SEG_DIGITS];



void writeSSDisplay(void* p)
{
  ssDisplay_p ssDisplay = (ssDisplay_p) p;
  uint8_t srSegs[NUM_SSDISPLAY_DIGITS * 8];

  for (uint8_t digitIdx = 0; digitIdx < NUM_SSDISPLAY_DIGITS; ++digitIdx) {
    for (uint8_t segIdx = 0, segMask = 0x01; segIdx < 8; ++segIdx, segMask <<= 1) {
      srSegs[ssDisplay->digitSegToSRSegMap[digitIdx][segIdx]] =
        ssDisplay->digitBuf[digitIdx] & segMask ? 1 : 0;
    }
  }

  // We have to output the bits in reverse order:  D7 of the last
  // shift register first, D0 of the first shift register last.
  for (uint8_t i = 0, bitIdx = NUM_SSDISPLAY_DIGITS * 8 - 1; i < NUM_SSDISPLAY_DIGITS * 8; ++i, --bitIdx) {
    digitalWrite(TPIC6B595_SERIN, srSegs[bitIdx]);
    digitalWrite(TPIC6B595_SRCK, 1);
    digitalWrite(TPIC6B595_SRCK, 0);
  }
  digitalWrite(TPIC6B595_SERIN, 0);

  // Clock the shift register into the output register.
  digitalWrite(TPIC6B595_RCK, 1);
  digitalWrite(TPIC6B595_RCK, 0);
}



void writeSSChars(void* p)
{
  ssDisplay_p ssDisplay = (ssDisplay_p) p;

  for (uint8_t i = 0; i < NUM_SSDISPLAY_DIGITS; ++i) {
    uint8_t digitSegs;
    switch(ssDisplay->digitChar[i]) {
      case '0':
        digitSegs = 0b00111111;
        break;
      case '1':
        digitSegs = 0b00110000;
        break;
      case '2':
        digitSegs = 0b01011011;
        break;
      case '3':
        digitSegs = 0b01001111;
        break;
      case '4':
        digitSegs = 0b01100110;
        break;
      case '5':
        digitSegs = 0b01101101;
        break;
      case '6':
        digitSegs = 0b01111101;
        break;
      case '7':
        digitSegs = 0b00000111;
        break;
      case '8':
        digitSegs = 0b01111111;
        break;
      case '9':
        digitSegs = 0b01101111;
        break;
      default:
        digitSegs = 0b01000000;
        break;
    }
    ssDisplay->digitBuf[i] = digitSegs;
  }  
  
  writeSSDisplay(ssDisplay);
}


void writeShiftReg(uint8_t* pDigits, uint8_t numDigits) {

  // We drive the segment LEDs by writing bits to a parallel-out serial
  // shift register, such as the 74164 or TPIC6B595.  The register's parallel
  // output drives the LEDs.

  for (uint8_t iDigit = 0; iDigit < numDigits; ++iDigit) {

    uint8_t digitBits = *pDigits++;
    
    // Shift out the LED bits.
    for (uint8_t i = 0; i < 8; ++i) {
      // Without any explicit delays, the period between writing the data bit
      // and the leading edge of the clock pulse ends up being much longer
      // than the required 15 ns (74164) OR 20 ns (TPIC6B595) setup+hold time.
      // The clock pulse width is also much greater than the minimum of 20 ns
      // (74164) or 40 ns (TPIC6B595).
      digitalWrite(TPIC6B595_SERIN, digitBits & 0x0001);
      digitBits >>= 1;
      digitalWrite(TPIC6B595_SRCK, 1);
      digitalWrite(TPIC6B595_SRCK, 0);
    }
    digitalWrite(TPIC6B595_SERIN, 0);
  }

  // Clock the shift register into the output register.
  digitalWrite(TPIC6B595_RCK, 1);
  digitalWrite(TPIC6B595_RCK, 0);
  
}



void setup() {

  pinMode(TPIC6B595_SERIN, OUTPUT);
  pinMode(TPIC6B595_SRCK, OUTPUT);
  pinMode(TPIC6B595_RCK, OUTPUT);  

  // Shift register index 0 is the D0 output of the first shift register in
  // the cascaded chain.  Index 8(n-1)+7, where n is the number of digits
  // in the display, is D7 of the last shift register.
  uint8_t digitSegToSRSegMap[NUM_SSDISPLAY_DIGITS][8] =
  { {16+5, 16+4, 16+2, 16+1, 16+0, 16+6, 16+7, 16+3},
    { 0+5,  0+4,  8+5,  8+6,  8+7,  0+6,  0+7,  8+4},
    { 0+2,  0+3,  8+2,  8+1,  8+0,  0+1,  0+0,  8+3} };
  for (uint8_t i = 0; i < NUM_SSDISPLAY_DIGITS; ++i) {
    for (uint8_t j = 0; j < 8; ++j) {
      ssDisplay.digitSegToSRSegMap[i][j] = digitSegToSRSegMap[i][j];
    }
  }

  // power-on lamp test
  for (uint8_t i = 0; i < NUM_SEVEN_SEG_DIGITS; ssDispBuf[i++] = 0xff);
  writeShiftReg(ssDispBuf, NUM_SEVEN_SEG_DIGITS);
  delay(500);
  for (uint8_t i = 0; i < NUM_SEVEN_SEG_DIGITS; ssDispBuf[i++] = 0);
  writeShiftReg(ssDispBuf, NUM_SEVEN_SEG_DIGITS);

}


void loop() {

  static uint8_t stepNum = 0;
  static uint8_t iDigit = 0;
  static uint8_t iSeg = 0;

  for (uint8_t i = 0; i < NUM_SSDISPLAY_DIGITS; ssDisplay.digitBuf[i++] = 0);

  switch(stepNum) {

    case 0:
      ssDisplay.digitBuf[iDigit] = 1 << iSeg;
      writeSSDisplay(&ssDisplay);
      delay(100);
      if (++iSeg > 7) {
        iSeg = 0;
        if (++iDigit >= NUM_SSDISPLAY_DIGITS) {
          iDigit = 0;
          ++stepNum;
        }
      }
      break;

    case 1:
      for (uint8_t i = 0; i < NUM_SSDISPLAY_DIGITS; ssDisplay.digitChar[i++] = '0' + iDigit);
      writeSSChars(&ssDisplay);
      delay(250);
      if (++iDigit > 10) {
        iDigit = 0;
        ++stepNum;
      }
      break;

    default:
      stepNum = 0;
  }
  
}
