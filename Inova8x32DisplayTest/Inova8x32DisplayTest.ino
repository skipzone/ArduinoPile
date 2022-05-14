#define DS_RED 2
#define DS_GREEN 4
#define STCP_nLE 5
#define SHCP 3
#define nMR 6
#define nE1 7
#define ROWA0 8
#define ROWA1 9
#define ROWA2 10

// NUM_ROWS must always be 8 (multiple panels
// must be joined horizontally).
#define NUM_ROWS 8
#define NUM_COLS 32



void clearAllRegisters(void)
{
  digitalWrite(nMR, 0);
  digitalWrite(STCP_nLE, 1);
  digitalWrite(STCP_nLE, 0);
  digitalWrite(nMR, 1);
}


void clearAllShiftRegisters(void)
{
  digitalWrite(nMR, 0);
  digitalWrite(nMR, 1);
}


bool walkingLed(void)
{
  // Returns true when the pattern is complete.

  static uint8_t curRow = 0;
  static uint8_t curCol = 0;
  static uint8_t curColor = 1;  // 1 = red, 2 = green, 3 = yellow
  static bool setRow = true;

  if (setRow) {
    setRow = false;
    clearAllShiftRegisters();
    digitalWrite(ROWA0, curRow & 0x01);
    digitalWrite(ROWA1, curRow & 0x02);
    digitalWrite(ROWA2, curRow & 0x04);
  }
  
  // If we're at the first column, we need to stuff a 1 into the first stage of the shift register.  
  if (0 == curCol) {
    digitalWrite(DS_RED, curColor & 0x01);
    digitalWrite(DS_GREEN, curColor & 0x02);
  }
  // Strobe SHCP to to set shift register stage 0 to DS and shift previous values to next stage.
  digitalWrite(SHCP, 1);
  digitalWrite(SHCP, 0);
  digitalWrite(DS_RED, 0);
  digitalWrite(DS_GREEN, 0);

  // Strobe STCP to transfer the shift register to the storage register.
  digitalWrite(nE1, 1);
  digitalWrite(STCP_nLE, 1);
  digitalWrite(STCP_nLE, 0);
  digitalWrite(nE1, 0);

  if (++curCol >= NUM_COLS) {
    curCol = 0;
    if (++curRow >= NUM_ROWS) {
      curRow = 0;
      if (++curColor > 3) {
        curColor = 1;
      }
    }
    setRow = true;
  }

  delay(5);
  
  return curRow == 0 && curCol == 0 && curColor == 1;
}


bool walkingRow(void)
{
  // Returns true when the pattern is complete.

  static uint8_t curRow = 0;
  static uint8_t curColor = 1;  // 1 = red, 2 = green, 3 = yellow
  static bool setRow = true;

  digitalWrite(ROWA0, curRow & 0x01);
  digitalWrite(ROWA1, curRow & 0x02);
  digitalWrite(ROWA2, curRow & 0x04);
  
  digitalWrite(DS_RED, curColor & 0x01);
  digitalWrite(DS_GREEN, curColor & 0x02);
  for (uint8_t i = 0; i < NUM_COLS; ++i) {
    // Strobe SHCP to to set shift register stage 0 to DS and shift previous values to next stage.
    digitalWrite(SHCP, 1);
    digitalWrite(SHCP, 0);
  }
  digitalWrite(DS_RED, 0);
  digitalWrite(DS_GREEN, 0);

  // Strobe STCP to transfer the shift register to the storage register.
  digitalWrite(nE1, 1);
  digitalWrite(STCP_nLE, 1);
  digitalWrite(STCP_nLE, 0);
  digitalWrite(nE1, 0);

  clearAllShiftRegisters();

  if (++curRow >= NUM_ROWS) {
    curRow = 0;
    if (++curColor > 3) {
      curColor = 1;
    }
  }

  delay(50);
  
  return curRow == 0 && curColor == 1;
}


bool walkingColumn(bool fill)
{
  // Returns true when the pattern is complete.

  static uint8_t curRow = 255;
  static uint8_t curCol = 255;
  static uint8_t curColor = 1;  // 1 = red, 2 = green, 3 = yellow
  static unsigned long nextColChangeMs = 0;
 
  unsigned long now = millis();
  bool patternDone = false;

  if (now < nextColChangeMs) {
    if (++curRow >= NUM_ROWS) {
      curRow = 0;
    }
    digitalWrite(ROWA0, curRow & 0x01);
    digitalWrite(ROWA1, curRow & 0x02);
    digitalWrite(ROWA2, curRow & 0x04);
    
    return false;
  }  

  nextColChangeMs = fill && curCol == NUM_COLS - 2 ? now + 1000 : now + 15;

  if (++curCol >= NUM_COLS) {
    curCol = 0;
    clearAllShiftRegisters();
    if (++curColor > 3) {
      // Pattern is done.  Prepare for next time.
      curRow = 255;
      curCol = 255;
      curColor = 1;
      nextColChangeMs = 0;
      return true;
    }
  }

  // If we're at the first column, we need to stuff a 1 into the first stage of the shift register.  
  if (0 == curCol || fill) {
    digitalWrite(DS_RED, curColor & 0x01);
    digitalWrite(DS_GREEN, curColor & 0x02);
  }
  // Strobe SHCP to to set shift register stage 0 to DS and shift previous values to next stage.
  digitalWrite(SHCP, 1);
  digitalWrite(SHCP, 0);
  digitalWrite(DS_RED, 0);
  digitalWrite(DS_GREEN, 0);
  // Strobe STCP to transfer the shift register to the storage register.
  digitalWrite(STCP_nLE, 1);
  digitalWrite(STCP_nLE, 0);

  return false;
}



void setup() {
  delay(100);
  
  pinMode(DS_RED, OUTPUT);
  pinMode(DS_GREEN, OUTPUT);
  pinMode(STCP_nLE, OUTPUT);
  pinMode(SHCP, OUTPUT);
  pinMode(nMR, OUTPUT);
  pinMode(nE1, OUTPUT);
  pinMode(ROWA0, OUTPUT);
  pinMode(ROWA1, OUTPUT);
  pinMode(ROWA2, OUTPUT);

  digitalWrite(DS_RED, 0);
  digitalWrite(DS_GREEN, 0);
  digitalWrite(STCP_nLE, 0);
  digitalWrite(SHCP, 0);
  digitalWrite(nMR, 1);
  digitalWrite(nE1, 1);
  digitalWrite(ROWA0, 0);
  digitalWrite(ROWA1, 0);
  digitalWrite(ROWA2, 0);

  clearAllRegisters();
}



void loop() {
  static uint8_t patternNum = 0;

  bool changePattern;
  
  switch(patternNum) {
    case 0:
      changePattern = walkingLed();
      break;
    case 1:
      changePattern = walkingRow();
      break;
    case 2:
      changePattern = walkingColumn(false);
      break;
    case 3:
      changePattern = walkingColumn(true);
      break;
  }

  if (changePattern) {
    if (++patternNum > 3) {
      patternNum = 0;
    }
    clearAllRegisters();
  }
}
