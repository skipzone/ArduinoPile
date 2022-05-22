/**************************************************
 *                                                *
 * OctoFlashy Interactive Lamps Pattern Generator *
 * Using Time-of-Flight Sensor Data               *
 *                                                *
 * Ross Butler  December 2019                     *
 *                                                *
 **************************************************/


/***********
 * Options *
 ***********/

#define ENABLE_WATCHDOG
//#define ENABLE_DEBUG_PRINT


/************
 * Includes *
 ************/

#ifdef ENABLE_WATCHDOG
#include <avr/wdt.h>
#endif

#include <Wire.h>
#include <VL53L1X.h>
//#include <SPI.h>
//#include "RF24.h"

#ifndef ENABLE_DEBUG_PRINT
#include "DmxSimple.h"
#else
#include "printf.h"
#endif


/*********************************************
 * Implementation and Behavior Configuration *
 *********************************************/

#define DMX_OUTPUT_PIN 3
#define LAMP_TEST_PIN 8

#define LAMP_TEST_ACTIVE LOW
#define LAMP_TEST_INTENSITY 255

#define SENSOR_POLL_INTERVAL_MS 100L
#define DMX_TX_INTERVAL_MS 33L

// Restrict color and lamp selection angles to avoid gimbal lock.
constexpr float maxColorAngleDegrees = 180;
constexpr float maxLampAngleDegrees = 180;

constexpr int16_t minPpSoundForStrobe = 300;
constexpr int16_t maxPpSoundForStrobe = 500;
constexpr uint8_t minStrobeValue = 225;
constexpr uint8_t maxStrobeValue = 250;

#define NUM_LAMPS 9

#define LAMP_MIN_INTENSITY 64

// With RGB lamps, enable red-green-blue-red wraparound as widget value
// varies from one extreme to another.  (Without wraparound, the lamp
// color would be red at one extreme and blue at the other, with no way
// to go from blue through violet and magenta back to red.)
//#define ENABLE_COLOR_WRAPAROUND

// The cheap-o RGB "PAR" lamps have a fourth DMX channel for strobe and intensity.
#define LAMP_HAS_CONTROL_CHANNEL

// With four-channel dimmers, it can be convenient to leave the fourth channel
// unused so that each dimmer controls one tri-color lamp arrangement.
//#define SKIP_DIMMER_FOURTH_CHANNEL


/*************************
 * Don't mess with these *
 *************************/

#ifdef ENABLE_COLOR_WRAPAROUND
  // With RGB lamps, we want to fade from red to green to blue then back to red as
  // the corresponding value from the widget goes from one extreme to the other.
  // To facilitate that, we track four color intensities, with both the first and
  // last representing the red intensity.
  #define NUM_COLORS_PER_LAMP 4
#else
  #define NUM_COLORS_PER_LAMP 3
#endif

#if defined(LAMP_HAS_CONTROL_CHANNEL) || defined(SKIP_DIMMER_FOURTH_CHANNEL)
#define DMX_NUM_CHANNELS_PER_LAMP 4
#else
#define DMX_NUM_CHANNELS_PER_LAMP 3
#endif
#define DMX_NUM_CHANNELS (NUM_LAMPS * DMX_NUM_CHANNELS_PER_LAMP)

// Let the scale8 function stolen from the FastLED library use assembly code if we're on an AVR chip.
#if defined(__AVR__)
#define SCALE8_AVRASM 1
#else
#define SCALE8_C 1
#endif
typedef uint8_t fract8;   ///< ANSI: unsigned short _Fract
#define LIB8STATIC_ALWAYS_INLINE __attribute__ ((always_inline)) static inline


/***********
 * Globals *
 ***********/

VL53L1X sensor;

static float currentColorAngle;
static float currentLampAngle;
static int16_t currentPpSound;

static int colorChannelIntensities[NUM_LAMPS][NUM_COLORS_PER_LAMP];


/******************
 * Implementation *
 ******************/

void initTofSensor()
{
  sensor.setTimeout(500);
  if (!sensor.init())
  {
#ifdef ENABLE_DEBUG_PRINT
    Serial.println("Failed to detect and initialize sensor.");
#endif
  }

  // Use long distance mode and allow up to 50000 us (50 ms) for a measurement.
  // You can change these settings to adjust the performance of the sensor, but
  // the minimum timing budget is 20 ms for short distance mode and 33 ms for
  // medium and long distance modes. See the VL53L1X datasheet for more
  // information on range and timing limits.
  sensor.setDistanceMode(VL53L1X::Long);
  sensor.setMeasurementTimingBudget(50000);

  // Start continuous readings at a rate of one measurement every 50 ms (the
  // inter-measurement period). This period should be at least as long as the
  // timing budget.
  sensor.startContinuous(50);
}


void initDmx()
{
#ifndef ENABLE_DEBUG_PRINT
  DmxSimple.usePin(DMX_OUTPUT_PIN);
  DmxSimple.maxChannel(DMX_NUM_CHANNELS);
#endif
}


void setup()
{
#ifdef ENABLE_DEBUG_PRINT
  Serial.begin(115200);
  printf_begin();
  Serial.println(F("Debug print enabled."));    
#endif

  Wire.begin();
  Wire.setClock(400000); // use 400 kHz I2C

  initDmx();
  initTofSensor();

#ifdef LAMP_TEST_PIN
#if LAMP_TEST_ACTIVE == LOW
  pinMode(LAMP_TEST_PIN, INPUT_PULLUP);
#else
  pinMode(LAMP_TEST_PIN, INPUT);
#endif
#endif

#ifdef ENABLE_WATCHDOG
  wdt_enable(WDTO_1S);     // enable the watchdog
#endif
}


// scale8 function shamelessly stolen from FastLED library.
//
///  scale one byte by a second one, which is treated as
///  the numerator of a fraction whose denominator is 256
///  In other words, it computes i * (scale / 256)
///  4 clocks AVR with MUL, 2 clocks ARM
LIB8STATIC_ALWAYS_INLINE uint8_t scale8(uint8_t i, fract8 scale)
{
#if SCALE8_C == 1
    return (((uint16_t)i) * (1+(uint16_t)(scale))) >> 8;
#elif SCALE8_AVRASM == 1
#if defined(LIB8_ATTINY)
    uint8_t work=i;
    uint8_t cnt=0x80;
    asm volatile(
        "  inc %[scale]                 \n\t"
        "  breq DONE_%=                 \n\t"
        "  clr %[work]                  \n\t"
        "LOOP_%=:                       \n\t"
        /*"  sbrc %[scale], 0             \n\t"
        "  add %[work], %[i]            \n\t"
        "  ror %[work]                  \n\t"
        "  lsr %[scale]                 \n\t"
        "  clc                          \n\t"*/
        "  sbrc %[scale], 0             \n\t"
        "  add %[work], %[i]            \n\t"
        "  ror %[work]                  \n\t"
        "  lsr %[scale]                 \n\t"
        "  lsr %[cnt]                   \n\t"
        "brcc LOOP_%=                   \n\t"
        "DONE_%=:                       \n\t"
        : [work] "+r" (work), [cnt] "+r" (cnt)
        : [scale] "r" (scale), [i] "r" (i)
        :
      );
    return work;
#else
    asm volatile(
        // Multiply 8-bit i * 8-bit scale, giving 16-bit r1,r0
        "mul %0, %1          \n\t"
        // Add i to r0, possibly setting the carry flag
        "add r0, %0         \n\t"
        // load the immediate 0 into i (note, this does _not_ touch any flags)
        "ldi %0, 0x00       \n\t"
        // walk and chew gum at the same time
        "adc %0, r1          \n\t"
         "clr __zero_reg__    \n\t"

         : "+a" (i)      /* writes to i */
         : "a"  (scale)  /* uses scale */
         : "r0", "r1"    /* clobbers r0, r1 */ );

    /* Return the result */
    return i;
#endif
#else
#error "No implementation for scale8 available."
#endif
}


#ifdef NO_COMPILE
//bool handleMeasurementVectorPayload(const MeasurementVectorPayload* payload, uint8_t payloadSize)
bool handleMeasurementVectorPayload(MeasurementVectorPayload* payload, uint8_t payloadSize)
{
  if (payload->widgetHeader.id == 12) payload->widgetHeader.id = 3;  // Pretend that Baton is Rainstick
  
  if (payload->widgetHeader.id < 1 || payload->widgetHeader.id > 4) {
#ifdef ENABLE_DEBUG_PRINT
    Serial.print(F("got MeasurementVectorPayload payload from widget "));
    Serial.print(payload->widgetHeader.id);
    Serial.println(F(" but expected one from widgets 1 (Tilt-1), 2 (Tilt-2), 3 (Tilt-Test), or 4 (Rainstick)."));
#endif
    return false;
  }

// TODO:  Enable the next two lines and remove the third when Rainstick is running current firmware.
  // Rainstick sends everything the tilt widgets send plus a peak-to-peak sound value.
  uint8_t numExpectedValues = payload->widgetHeader.id <= 3 ? 13 : 14;
//  uint8_t numExpectedValues = 13;
  uint16_t expectedPayloadSize = sizeof(WidgetHeader) + sizeof(int16_t) * numExpectedValues;
  if (payloadSize != expectedPayloadSize) {
#ifdef ENABLE_DEBUG_PRINT
    Serial.print(F("got MeasurementVectorPayload from widget "));
    Serial.print(payload->widgetHeader.id);
    Serial.print(F(" with "));
    Serial.print(payloadSize);
    Serial.print(F(" bytes but expected "));
    Serial.print(expectedPayloadSize);
    Serial.println(F(" bytes."));    
#endif
    return false;
  }

  // Ignore payloads with all-zero data because the packet is probably
  // just a heartbeat while the widget is in standby mode.
  bool gotAllZeroData = true;
  for (uint8_t i = 0; gotAllZeroData && i < numExpectedValues; ++i) {
    gotAllZeroData = payload->measurements[0] == 0;
  }
  if (gotAllZeroData) {
    return false;
  }
  
  switch (payload->widgetHeader.id) {

    case 1:
      // Tilt-1's pitch is the lamp selection angle.
      currentLampAngle = payload->measurements[1] / 10.0;
#ifdef ENABLE_DEBUG_PRINT
      Serial.print(F("got pitch "));
      Serial.print(currentLampAngle);
      Serial.println(F(" for lamp angle from widget 1"));
#endif
      break;

    case 2:
      // Tilt-2's pitch is the color selection angle.
      currentColorAngle = payload->measurements[1] / 10.0;
#ifdef ENABLE_DEBUG_PRINT
      Serial.print(F("got pitch "));
      Serial.print(currentColorAngle);
      Serial.println(F(" for color angle from widget 2"));
#endif
      break;

    case 3:
      // Tilt-Test's pitch is the lamp selection angle, and its roll is the color selection angle.
      currentLampAngle = payload->measurements[1] / 10.0;
      currentColorAngle = payload->measurements[2] / 10.0;
#ifdef ENABLE_DEBUG_PRINT
      Serial.print(F("got pitch "));
      Serial.print(currentLampAngle);
      Serial.print(F(" for lamp angle and roll "));
      Serial.print(currentColorAngle);
      Serial.println(F(" for color angle from widget 2"));
#endif
      break;

    case 4:
    // TODO:  fix the damn comment below
      // Rainstick's pitch is the color selection angle, and its pitch is the color selection angle.
      currentColorAngle = payload->measurements[1] / 10.0;
      currentLampAngle = payload->measurements[2] / 10.0;
      currentPpSound = payload->measurements[13];
#ifdef ENABLE_DEBUG_PRINT
      Serial.print(F("got pitch "));
      Serial.print(currentColorAngle);
      Serial.print(F(" for color angle, roll "));
      Serial.print(currentLampAngle);
      Serial.print(F(" for lamp angle, and p-p sound "));
      Serial.println(currentPpSound);
#endif
    break;
  }
  
  return true;
}
#endif


void pollTofSensor()
{
  sensor.read();
  
  bool gotMeasurements = false;
  if (sensor.ranging_data.range_status == VL53L1X::RangeValid) {
    gotMeasurements = true;

#ifdef ENABLE_DEBUG_PRINT
    Serial.print("range: ");
    Serial.print(sensor.ranging_data.range_mm);
    Serial.print("\tstatus: ");
    Serial.print(VL53L1X::rangeStatusToString(sensor.ranging_data.range_status));
    Serial.print("\tpeak signal: ");
    Serial.print(sensor.ranging_data.peak_signal_count_rate_MCPS);
    Serial.print("\tambient: ");
    Serial.print(sensor.ranging_data.ambient_count_rate_MCPS);
    Serial.println();
#endif

    //currentLampAngle = map(sensor.ranging_data.range_mm, 0, 2500, -179.9, 179.9);
    currentColorAngle = map(constrain((float) sensor.ranging_data.range_mm, 200.0, 3000.0), 200.0, 3000.0, (float) -179.9, (float) 179.9);
  }

#ifdef ENABLE_DEBUG_PRINT
  if (gotMeasurements) {
    Serial.print(F(" currentColorAngle="));
    Serial.print(currentColorAngle);
    Serial.print(F(" currentLampAngle="));
    Serial.print(currentLampAngle);
    Serial.print(F(" currentPpSound="));
    Serial.println(currentPpSound);
  }
#endif
}


void sendDmx()
{
  uint8_t dmxChannelValues[DMX_NUM_CHANNELS + 1];

  uint16_t dmxChannelNum = 1;
  for (uint8_t lampIdx = 0; lampIdx < NUM_LAMPS; ++lampIdx) {

#if defined(LAMP_HAS_CONTROL_CHANNEL)
    // If the sound is sufficiently loud, flash all the lamps instead of setting their intensities.
    if (currentPpSound >= minPpSoundForStrobe) {
      uint8_t strobeValue = map(constrain(currentPpSound, minPpSoundForStrobe, minPpSoundForStrobe),
                                minPpSoundForStrobe, maxPpSoundForStrobe, minStrobeValue, maxStrobeValue);
      dmxChannelValues[dmxChannelNum++] = strobeValue;
#ifdef ENABLE_DEBUG_PRINT
      Serial.print(F("strobeValue="));
      Serial.print(strobeValue);
      Serial.print(F(" currentPpSound="));
      Serial.print(currentPpSound);
      Serial.print(F(" minPpSoundForStrobe="));
      Serial.print(minPpSoundForStrobe);
      Serial.print(F(" maxPpSoundForStrobe="));
      Serial.print(maxPpSoundForStrobe);
      Serial.print(F(" minStrobeValue="));
      Serial.print(minStrobeValue);
      Serial.print(F(" maxStrobeValue="));
      Serial.println(maxStrobeValue);
#endif
    }
    else {
      // Set the lamp to full brightness because we control its
      // overall brightness by way of the individual colors.
      dmxChannelValues[dmxChannelNum++] = 127;
    }
#endif

#if (NUM_COLORS_PER_LAMP == 4)
    dmxChannelValues[dmxChannelNum++] = colorChannelIntensities[lampIdx][0] + colorChannelIntensities[lampIdx][3];
#else
    dmxChannelValues[dmxChannelNum++] = colorChannelIntensities[lampIdx][0];
#endif
    dmxChannelValues[dmxChannelNum++] = colorChannelIntensities[lampIdx][1];
    dmxChannelValues[dmxChannelNum++] = colorChannelIntensities[lampIdx][2];      

#ifdef SKIP_DIMMER_FOURTH_CHANNEL
    dmxChannelValues[dmxChannelNum++] = 0;
#endif
  }

  // Send zeros to any unused channels.
  while (dmxChannelNum <= DMX_NUM_CHANNELS) {
    dmxChannelValues[dmxChannelNum++] = 0;
  }

  // Transmit the DMX channel values.
#ifndef ENABLE_DEBUG_PRINT
  for (dmxChannelNum = 1; dmxChannelNum <= DMX_NUM_CHANNELS; ++dmxChannelNum) {
    DmxSimple.write(dmxChannelNum, dmxChannelValues[dmxChannelNum]);
//#ifdef ENABLE_DEBUG_PRINT
//    Serial.print(F("sent DMX ch "));
//    Serial.print(dmxChannelNum);
//    Serial.print(F(": "));
//    Serial.println(dmxChannelValues[dmxChannelNum]);    
//#endif
  }
#endif
}


void updateLamps()
{
#ifdef LAMP_TEST_PIN
  if (digitalRead(LAMP_TEST_PIN) == LAMP_TEST_ACTIVE) {
    for (uint8_t lampIdx = 0; lampIdx < NUM_LAMPS; ++lampIdx) {
      for (uint8_t colorIdx = 0; colorIdx < 3; ++colorIdx) {
        colorChannelIntensities[lampIdx][colorIdx] = LAMP_TEST_INTENSITY;
      }
    }
#ifndef ENABLE_DEBUG_PRINT
    sendDmx();
#endif
    return;
  }
#endif

  constexpr float colorAngleStep = maxColorAngleDegrees * 2 / (NUM_COLORS_PER_LAMP - 1);
  constexpr float lampAngleStep = maxLampAngleDegrees * 2 / (NUM_LAMPS - 1);

  int colorIntensities[NUM_COLORS_PER_LAMP];
  for (uint8_t i = 0; i < NUM_COLORS_PER_LAMP; ++i) {
    colorIntensities[i] = 0;
  }

  int lampIntensities[NUM_LAMPS];
  for (uint8_t i = 0; i < NUM_LAMPS; ++i) {
    lampIntensities[i] = LAMP_MIN_INTENSITY;
  }

  // Normalize and restrict color and lamp selection angles to [0, max___Degrees*2] degrees.
  float normalizedColorAngle;
  if (currentColorAngle <= - maxColorAngleDegrees) {
    normalizedColorAngle = 0;
  }
  else if (currentColorAngle >= maxColorAngleDegrees) {
    normalizedColorAngle = maxColorAngleDegrees * 2.0;
  }
  else {
    normalizedColorAngle = currentColorAngle + maxColorAngleDegrees;
  }
  float normalizedLampAngle;
  if (currentLampAngle <= - maxLampAngleDegrees) {
    normalizedLampAngle = 0;
  }
  else if (currentLampAngle >= maxLampAngleDegrees) {
    normalizedLampAngle = maxLampAngleDegrees * 2.0;
  }
  else {
    normalizedLampAngle = currentLampAngle + maxLampAngleDegrees;
  }

  // Fade across the colors based on color selection angle.
  int colorSection = floor(normalizedColorAngle / colorAngleStep);
  // Fix up colorSection if measurement is maxColorAngleDegrees degrees or more.
  if (colorSection >= NUM_COLORS_PER_LAMP - 1) {
      colorSection = NUM_COLORS_PER_LAMP - 2;
  }
  colorIntensities[colorSection] =
    map(normalizedColorAngle, colorAngleStep * colorSection, colorAngleStep * (colorSection + 1), 255, 0);
  colorIntensities[colorSection + 1] =
    map(normalizedColorAngle, colorAngleStep * colorSection, colorAngleStep * (colorSection + 1), 0, 255);

  // Fade across the lamps based on lamp selection angle.
  int lampSection = floor(normalizedLampAngle / lampAngleStep);
  // Fix up lampSection if measurement is maxLampAngleDegrees degrees or more.
  if (lampSection >= NUM_LAMPS - 1) {
      lampSection = NUM_LAMPS - 2;
  }
  lampIntensities[lampSection] =
    map(normalizedLampAngle, lampAngleStep * lampSection, lampAngleStep * (lampSection + 1), 255, LAMP_MIN_INTENSITY);
  lampIntensities[lampSection + 1] =
    map(normalizedLampAngle, lampAngleStep * lampSection, lampAngleStep * (lampSection + 1), LAMP_MIN_INTENSITY, 255);

  // For each lamp, scale the intensity of its colors by the corresponding color intensity.
  for (uint8_t lampIdx = 0; lampIdx < NUM_LAMPS; ++lampIdx) {
    for (uint8_t colorIdx = 0; colorIdx < NUM_COLORS_PER_LAMP; ++colorIdx) {
      colorChannelIntensities[lampIdx][colorIdx] = scale8(lampIntensities[lampIdx], colorIntensities[colorIdx]);
    }
  }

  sendDmx();
}


void loop()
{
  static int32_t lastSensorPollMs;
  static int32_t lastDmxTxMs;

  uint32_t now = millis();

  if (now - lastSensorPollMs >= SENSOR_POLL_INTERVAL_MS) {
    pollTofSensor();
  }

  if (now - lastDmxTxMs >= DMX_TX_INTERVAL_MS) {
    lastDmxTxMs = now;
    updateLamps();
  }

#ifdef ENABLE_WATCHDOG
  wdt_reset();
#endif
}
