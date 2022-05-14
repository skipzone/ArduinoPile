/***********
 * Options *
 ***********/

//#define ENABLE_PRINT_TO_SERIAL


/************
 * Includes *
 ************/

#include <ADCTouch.h>
#include <avr/pgmspace.h>
#include <Button.h>
#include <LiquidCrystal.h>


/*****************
 * Configuration *
 *****************/

constexpr uint32_t capSenseUpdateIntervalMs = 333;

constexpr uint32_t ledSlowUpdateIntervalMs = 30;
constexpr uint32_t ledFastUpdateIntervalMs = 3;
constexpr uint8_t ledLowMaxIntensity = 96;
constexpr uint8_t ledHighMaxIntensity = 254;

constexpr uint8_t numCapSensePins = 5;
constexpr uint8_t capSensePins[numCapSensePins] = {A0, A1, A2, A3, A4};
constexpr int capSenseNumSamplesForRef = 500;
constexpr int capSenseNumSamples = 10;
constexpr int capSenseThresholds[numCapSensePins] = {30, 30, 30, 30, 25};

constexpr uint8_t lcd_numRows = 4;
constexpr uint8_t lcd_numCols = 16;
constexpr uint8_t lcd_pin_rs = 8;
constexpr uint8_t lcd_pin_e = 9;
constexpr uint8_t lcd_pin_d4 = 10;
constexpr uint8_t lcd_pin_d5 = 11;
constexpr uint8_t lcd_pin_d6 = 12;
constexpr uint8_t lcd_pin_d7 = 13;

constexpr uint8_t led_pin_red = 3;
constexpr uint8_t led_pin_green = 5;
constexpr uint8_t led_pin_blue = 6;

constexpr uint8_t switch_pin_s1 = 2;
constexpr uint8_t switch_pin_s2 = 4;


/*************
 * Constants *
 *************/

const char diagnosis_0[] PROGMEM = "Inconclusive";
const char diagnosis_1[] PROGMEM = "Purrrrfect!";
const char diagnosis_2[] PROGMEM = "Dehydrated";
const char diagnosis_3[] PROGMEM = "Ballsy tom";
const char diagnosis_4[] PROGMEM = "Hairball";
const char diagnosis_5[] PROGMEM = "Pretty kitty";
const char diagnosis_6[] PROGMEM = "In heat";
const char diagnosis_7[] PROGMEM = "Not a pussy";

const char* const diagnoses[8] PROGMEM = {
  diagnosis_0, diagnosis_1, diagnosis_2, diagnosis_3,
  diagnosis_4, diagnosis_5, diagnosis_6, diagnosis_7
};

const char treatment_0[] PROGMEM = "sRescan pussy";
const char treatment_1[] PROGMEM = "hGo play!";
const char treatment_2[] PROGMEM = "sGet a drink!";
const char treatment_3[] PROGMEM = "sSnip snip!";
const char treatment_4[] PROGMEM = "sShave pussy!";
const char treatment_5[] PROGMEM = "hPet kitty!";
const char treatment_6[] PROGMEM = "hRawr!";
const char treatment_7[] PROGMEM = "sHiss!";

const char* const treatments[8] PROGMEM = {
  treatment_0, treatment_1, treatment_2, treatment_3, 
  treatment_4, treatment_5, treatment_6, treatment_7
};

enum class OperatingState {
  INIT_START,
  INIT_FINISH,
  IDLE,
  IDLE_RESUME,
  RECALIBRATE_START,
  RECALIBRATE,
  RECALIBRATE_FINISH,
  PHS_START,
  PHS,
  PHS_FINISH,
  PLR_START,
  PLR,
  PLR_FINISH,
  SHOW_RESULTS_TEXT_START,
  SHOW_RESULTS_TEXT
};

enum class LedMode {
  UNKNOWN,
  STOP,
  SLOW_THROB,
  FAST_THROB,
  UNISON_THROB,
  SLOW_RAMP,
  FAST_RAMP,
};

const uint8_t PROGMEM lcdCustomChars[8][8] = {
  // happy cat, upper left
  { 0b01000,
    0b01100,
    0b10010,
    0b10001,
    0b10100,
    0b11000,
    0b10010 },
  // happy cat, upper right
  { 0b00010,
    0b00110,
    0b01001,
    0b10001,
    0b00101,
    0b00011,
    0b01001 },
  // happy cat, lower left
  { 0b10100,
    0b10010,
    0b00001,
    0b01000,
    0b00100,
    0b00011,
    0b00000 },
  // happy cat, lower right
  { 0b00101,
    0b01001,
    0b10000,
    0b00010,
    0b00100,
    0b11000,
    0b00000 },
  // sad cat, upper left
  { 0b01000,
    0b01100,
    0b10010,
    0b10001,
    0b10100,
    0b11000,
    0b10010 },
  // sad cat, upper right
  { 0b00010,
    0b00110,
    0b01001,
    0b10001,
    0b00101,
    0b00011,
    0b01001 },
  // sad cat, lower left
  { 0b10001,
    0b10010,
    0b00000,
    0b01000,
    0b00100,
    0b00011,
    0b00000 },
  // sad cat, lower right
  { 0b10001,
    0b01001,
    0b00000,
    0b00010,
    0b00100,
    0b11000,
    0b00000 },
};

// gamma correction for human-eye perception of WS2812 RGB LED brightness
// (from http://rgb-123.com/ws2812-color-output/ on 3 April 2014),
// modified for PWM-driven LEDs using open drain by changing elements
// 1 to 21 from 0 to 1 and elements 22 to 28 from 1 to 2.
const uint8_t PROGMEM gamma[256] = {
      0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
      1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
      2,  2,  2,  3,  3,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,  5,
      6,  6,  6,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 11, 11,
     11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18,
     19, 19, 20, 21, 21, 22, 22, 23, 23, 24, 25, 25, 26, 27, 27, 28,
     29, 29, 30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 37, 38, 39, 40,
     40, 41, 42, 43, 44, 45, 46, 46, 47, 48, 49, 50, 51, 52, 53, 54,
     55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
     71, 72, 73, 74, 76, 77, 78, 79, 80, 81, 83, 84, 85, 86, 88, 89,
     90, 91, 93, 94, 95, 96, 98, 99,100,102,103,104,106,107,109,110,
    111,113,114,116,117,119,120,121,123,124,126,128,129,131,132,134,
    135,137,138,140,142,143,145,146,148,150,151,153,155,157,158,160,
    162,163,165,167,169,170,172,174,176,178,179,181,183,185,187,189,
    191,193,194,196,198,200,202,204,206,208,210,212,214,216,218,220,
    222,224,227,229,231,233,235,237,239,241,244,246,248,250,252,255,
};


/***********
 * Globals *
 ***********/

static bool plrMode;

static OperatingState opState;

static LiquidCrystal lcd(lcd_pin_rs, lcd_pin_e, lcd_pin_d4, lcd_pin_d5, lcd_pin_d6, lcd_pin_d7);

static int capSenseRef[numCapSensePins];

static Button s1 = Button(switch_pin_s1);
static Button s2 = Button(switch_pin_s2);


/***********
 * Helpers *
 ***********/


void writeLeds(uint8_t r, uint8_t g, uint8_t b) {

  analogWrite(led_pin_red, 255 - pgm_read_byte_near(gamma + r));
  analogWrite(led_pin_green, 255 - pgm_read_byte_near(gamma + g));
  analogWrite(led_pin_blue, 255 - pgm_read_byte_near(gamma + b));
}


void doIntensityStep(uint8_t& intensity, bool& up, uint8_t increment, uint8_t maxIntensity, bool throb) {
  uint8_t prev = intensity;
  if (up) {
    intensity += increment;
    if (intensity > maxIntensity || intensity < prev) {
      if (throb) {
        intensity = maxIntensity;
        up = false;
      }
      else {
        intensity = 1;
      }
    }
  }
  else {
    intensity -= increment;
    if (intensity == 0 || intensity > prev) {
      if (throb) {
        intensity = 1;
        up = true;
      }
      else {
        intensity = maxIntensity;
      }
    }
  }
}


void lcdClearScreen() {

  lcd.clear();
  delay(1);
  lcd.setCursor(0, 0);  // zero-based col, line; (0, 0) is upper left corner
  lcd.print(F("CAT SCANner 9000"));
}


void lcdClearLine(uint8_t line) {

    lcd.setCursor(0, line);
    for (uint8_t i = 0; i < lcd_numCols; ++i) {
      lcd.write((uint8_t) ' ');
    }
}


void lcdWriteLine(uint8_t line, const char* text) {

  lcd.setCursor(0, line);
  lcd.print(text);
  uint8_t i = lcd_numCols - strlen(text);
  while (i != 0) {
    lcd.write(' ');
    --i;
  }
}


void drawCat(uint8_t col, uint8_t line, bool isHappy) {

  uint8_t i = isHappy ? 0 : 4;
  lcd.setCursor(col, line);
  lcd.write(i++);
  lcd.write(i++);
  lcd.setCursor(col, line + 1);
  lcd.write(i++);
  lcd.write(i);
}


void lcdShowCatProgress(int8_t stepNum, bool isHappy) {

  if (stepNum < 0) {
    uint8_t numSteps = -stepNum;
    // Clear the lines and draw the boundary bars.
    lcdClearLine(2);
    lcdClearLine(3);
    lcd.setCursor(0, 2);
    lcd.write(0xff);
    lcd.setCursor(0, 3);
    lcd.write(0xff);
    lcd.setCursor(1 + (numSteps << 1), 2);
    lcd.write(0xff);
    lcd.setCursor(1 + (numSteps << 1), 3);
    lcd.write(0xff);
  }
  else {
    drawCat(1 + (stepNum << 1), 2, isHappy);
  }
}


void lcdTest() {

  lcd.clear();
  delay(1);
  lcd.setCursor(0, 0);  // zero-based col, line; (0, 0) is upper left corner
  lcd.print(F("0123456789ABCDEF"));
  lcd.setCursor(0, 1);
  lcd.print(F("-----Line 2-----"));
  lcd.setCursor(0, 2);
  lcd.print(F("-----Line 3-----"));
  lcd.setCursor(0, 3);
  lcd.print(F("-----Line 4-----"));
  lcd.setCursor(15, 3);
  lcd.write((uint8_t) 0xfc);  // ROM code A00:  little table    ROM code A02:  u with umlaut
  lcd.setCursor(15, 2);
  lcd.write((uint8_t) 0xfd);  // ROM code A00:  division sign   ROM code A02:  y with accent
  lcd.setCursor(15, 1);
  lcd.write((uint8_t) 0xfe);  // ROM code A00:  blank           ROM code A02:  l with titty
  lcd.setCursor(15, 0);
  lcd.write((uint8_t) 0xff);  // ROM code A00:  block           ROM code A02:  y with umlaut
  lcd.setCursor(0, 3);
  lcd.print(F("z"));
  lcd.setCursor(0, 2);
  lcd.print(F("y"));
  lcd.setCursor(0, 1);
  lcd.print(F("x"));
  lcd.setCursor(0, 0);
  lcd.print(F("w"));
  lcd.setCursor(9, 3);
  lcd.print(F("|"));
  lcd.setCursor(9, 2);
  lcd.print(F("|"));
  lcd.setCursor(9, 1);
  lcd.print(F("|"));
  lcd.setCursor(9, 0);
  lcd.print(F("|"));
}


void readCapSense(bool (&isTouched)[numCapSensePins]) {

  for (uint8_t i = 0; i < numCapSensePins; ++i) {
    int value = ADCTouch.read(capSensePins[i], capSenseNumSamples) - capSenseRef[i];
#ifdef ENABLE_PRINT_TO_SERIAL
    Serial.print(i);
    Serial.print(":");
    Serial.println(value);
#endif
    isTouched[i] = value > capSenseThresholds[i];
  }
}


void displayCapSense(bool (&isTouched)[numCapSensePins], bool isLockedIn) {

  lcd.setCursor(16 - numCapSensePins, 1);
  for (uint8_t i = 0; i < numCapSensePins; ++i) {
    lcd.print(isTouched[i] ? isLockedIn ? ':' : '.' : ' ');
#ifdef ENABLE_PRINT_TO_SERIAL
    Serial.print(isTouched[i]);
    Serial.print(" ");
#endif
  }
  Serial.println(millis());
  Serial.println(F("----------"));
}


void displayDiagnosis(bool (&isTouched)[numCapSensePins]) {

  uint8_t diagnosisIdx = isTouched[2] << 2 | isTouched[1] << 1 | isTouched[0];

  char buf[18];
  
  strcpy_P(buf, (char*) pgm_read_word(&(diagnoses[diagnosisIdx])));
  lcdWriteLine(2, buf);

  strcpy_P(buf, (char*) pgm_read_word(&(treatments[diagnosisIdx])));
  bool isHappy = buf[0] == 'h';
  buf[0] = 0x7e;                  // arrow pointing right
  lcdWriteLine(3, buf);
  
  drawCat(14, 2, isHappy);
}


/******************
 * Initialization *
 ******************/

void initPins() {
  pinMode(led_pin_red, OUTPUT);
  pinMode(led_pin_green, OUTPUT);
  pinMode(led_pin_blue, OUTPUT);
  pinMode(switch_pin_s1, INPUT_PULLUP);
  pinMode(switch_pin_s2, INPUT_PULLUP);
}


void initLcd() {

  lcd.begin(lcd_numCols, lcd_numRows);

  // Download cat characters to the LCD controller.
  for (uint8_t i = 0; i < 8; ++i) {
    uint8_t buf[8];
    memcpy_P(buf, lcdCustomChars[i], 8);
    delay(1);
    lcd.createChar(i, buf);
  }

  // We never want scrolling.
  lcd.noAutoscroll();
}


void setup() {

#ifdef ENABLE_PRINT_TO_SERIAL
  Serial.begin(115200);
#endif

  initPins();
  
  initLcd();
//  lcdTest();
//  drawCat(12, 2, true);
//  drawCat(3, 2, false);

  s1.begin();
  s2.begin();
  
  opState = OperatingState::INIT_START;
}


/************
 * Run Loop *
 ************/

void loop() {

  static OperatingState returnState;
  static uint8_t stepNum;
  static uint32_t nextStepMs;
  static uint32_t capSenseNextUpdateMs;
  static bool padIsTouched[numCapSensePins];
  static bool selectionIsLocked;
  static LedMode ledMode;
  static LedMode lastLedMode;
  static uint32_t ledNextUpdateMs;
  static uint32_t ledUpdateIntervalMs; 
  static uint8_t ledMaxIntensity;
  static uint8_t r, g, b;
  static bool rUp, gUp, bUp;
  
  uint32_t now = millis();

  switch(opState) {

    case OperatingState::INIT_START:
      ledMode = LedMode::STOP;
      lastLedMode = LedMode::UNKNOWN;
      ledNextUpdateMs = now;
      returnState = OperatingState::INIT_FINISH;
      opState = OperatingState::RECALIBRATE_START;
      break;
      
    case OperatingState::INIT_FINISH:
      lcdClearScreen();
      opState = OperatingState::IDLE_RESUME;
      break;

    case OperatingState::IDLE_RESUME:
      lcdWriteLine(1, "Here pussy pussy");
      opState = OperatingState::IDLE;
      break;

    case OperatingState::IDLE:
      ledMode = LedMode::SLOW_THROB;
      // plrMode
      if (s1.pressed()) {
        returnState = OperatingState::IDLE_RESUME;
        opState = OperatingState::RECALIBRATE_START;
      }
      else if (s2.pressed()) {
        returnState = OperatingState::IDLE;
        opState = OperatingState::PHS_START;
      }
      break;
      
    case OperatingState::RECALIBRATE_START:
      ledMode = LedMode::UNISON_THROB;
      lcdClearScreen();
      lcdWriteLine(1, "Calibrating...");
      lcdShowCatProgress(-numCapSensePins, false);
      stepNum = 0;
      nextStepMs = now + 500;
      opState = OperatingState::RECALIBRATE;
      break;
      
    case OperatingState::RECALIBRATE:
      if ((int32_t) (now - nextStepMs) >= 0) {
        if (stepNum < numCapSensePins) {
          // Create reference value to account for stray
          // capacitance and capacitance of the pad.
          capSenseRef[stepNum] = ADCTouch.read(capSensePins[stepNum], capSenseNumSamplesForRef);
          lcdShowCatProgress(stepNum, true);
          ++stepNum;
          nextStepMs += 250;
        }
        else {
          lcdWriteLine(1, "Calibrated!");
          nextStepMs += 1000;
          opState = OperatingState::RECALIBRATE_FINISH;
        }
      }
      break;
      
    case OperatingState::RECALIBRATE_FINISH:
      if ((int32_t) (now - nextStepMs) >= 0) {
        lcdClearScreen();
        opState = s1.read() == Button::RELEASED
          ? returnState
          : OperatingState::SHOW_RESULTS_TEXT_START;
      }
      break;
      
    case OperatingState::PHS_START:
      ledMode = LedMode::FAST_THROB;
      lcdClearScreen();
      lcdWriteLine(1, "Scanning...");
      lcdShowCatProgress(-7, false);
      capSenseNextUpdateMs = now;
      selectionIsLocked = false;
      stepNum = 0;
      nextStepMs = now + 1000;
      opState = OperatingState::PHS;
      break;
      
    case OperatingState::PHS:
      if ((int32_t) (now - capSenseNextUpdateMs) >= 0) {
        capSenseNextUpdateMs += capSenseUpdateIntervalMs;
        if (!selectionIsLocked) {
          readCapSense(padIsTouched);
          selectionIsLocked |= padIsTouched[4];
          displayCapSense(padIsTouched, selectionIsLocked);
        }
        else {
          // If pad 3 is touched, unlock the selection.
          bool tempPads[numCapSensePins];
          readCapSense(tempPads);
          if (tempPads[3]) {
            selectionIsLocked = false;
          }
        }
      }
      if ((int32_t) (now - nextStepMs) >= 0) {
        nextStepMs += 1000;
        if (stepNum < 6) {
          lcdShowCatProgress(stepNum, false);
          ++stepNum;
        }
        else {
          // If a selection wasn't explicitly locked in, clear all the pad
          // flags so that appears that nothing at all was selected.
          if (!selectionIsLocked) {
            for (uint8_t i = 0; i < numCapSensePins; padIsTouched[i++] = false);
          }
          lcdWriteLine(1, "Done!");
          lcdShowCatProgress(stepNum, true);
          opState = OperatingState::PHS_FINISH;
        }
      }
      break;
      
    case OperatingState::PHS_FINISH:
      if ((int32_t) (now - nextStepMs) >= 0) {
          lcdWriteLine(1, "----Diagnosis---");
          displayDiagnosis(padIsTouched);
          opState = returnState;
      }
      break;
      
    case OperatingState::PLR_START:
      ledMode = LedMode::FAST_RAMP;
      break;
      
    case OperatingState::PLR:
      break;
      
    case OperatingState::PLR_FINISH:
      break;
      
    case OperatingState::SHOW_RESULTS_TEXT_START:
      ledMode = LedMode::SLOW_RAMP;
      capSenseNextUpdateMs = now;
      opState = OperatingState::SHOW_RESULTS_TEXT;
      break;
      
    case OperatingState::SHOW_RESULTS_TEXT:
      if (s1.pressed()) {
        lcdClearScreen();
        opState = returnState;
      }
      else {
        if ((int32_t) (now - capSenseNextUpdateMs) >= 0) {
          capSenseNextUpdateMs += capSenseUpdateIntervalMs;
          readCapSense(padIsTouched);
          displayCapSense(padIsTouched, false);
          displayDiagnosis(padIsTouched);
        }
      }
      break;
  }

  if (ledMode != lastLedMode) {
    lastLedMode = ledMode;
    r = g = b = 1;
    rUp = gUp = bUp = true;
    ledNextUpdateMs = now - 1;
    switch (ledMode) {
      case LedMode::STOP:
        break;
      case LedMode::SLOW_THROB:
        ledUpdateIntervalMs = ledSlowUpdateIntervalMs;
        ledMaxIntensity = ledLowMaxIntensity;
        break;
      case LedMode::FAST_THROB:
        ledUpdateIntervalMs = ledFastUpdateIntervalMs;
        ledMaxIntensity = ledHighMaxIntensity;
        break;
      case LedMode::UNISON_THROB:
        ledUpdateIntervalMs = ledSlowUpdateIntervalMs;
        ledMaxIntensity = ledLowMaxIntensity;
        break;
      case LedMode::SLOW_RAMP:
        ledUpdateIntervalMs = ledSlowUpdateIntervalMs;
        ledMaxIntensity = ledLowMaxIntensity;
        break;
      case LedMode::FAST_RAMP:
        ledUpdateIntervalMs = ledFastUpdateIntervalMs;
        ledMaxIntensity = ledHighMaxIntensity;
        break;
    }
  }

  if ((int32_t) (now - ledNextUpdateMs) >= 0) {
    ledNextUpdateMs += ledUpdateIntervalMs; 
    switch (ledMode) {

      case LedMode::UNKNOWN:
      case LedMode::STOP:
        r = g = b = 16;
        ledNextUpdateMs = UINT32_MAX;
        break;

      case LedMode::SLOW_THROB:
      case LedMode::FAST_THROB:
        doIntensityStep(r, rUp, 2, ledMaxIntensity, true);
        doIntensityStep(g, gUp, 3, ledMaxIntensity, true);
        doIntensityStep(b, bUp, 5, ledMaxIntensity, true);
        break;
      
      case LedMode::UNISON_THROB:
        doIntensityStep(r, rUp, 1, ledMaxIntensity, true);
        doIntensityStep(g, gUp, 1, ledMaxIntensity, true);
        doIntensityStep(b, bUp, 1, ledMaxIntensity, true);
        break;

      case LedMode::SLOW_RAMP:
      case LedMode::FAST_RAMP:
        doIntensityStep(r, rUp, 2, ledMaxIntensity, false);
        doIntensityStep(g, gUp, 3, ledMaxIntensity, false);
        doIntensityStep(b, bUp, 5, ledMaxIntensity, false);
        break;
    }
    writeLeds(r, g, b);
  }

}

