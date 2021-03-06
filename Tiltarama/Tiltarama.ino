/***********************************************
 *                                             *
 * MPU-6050 Triple-Axis Accelerometer and Gyro *
 * Evaluation Program                          *
 *                                             *
 * Ross Butler  July 2018                      *
 *                                             *
 ***********************************************/


/***********
 * Options *
 ***********/

#define ENABLE_WATCHDOG
//#define ENABLE_DEBUG_PRINT
#define ENABLE_LCD_16x2
//#define ENABLE_LCD_20x4   // TODO:  not supported yet
//#define PLOT_YAW_PITCH_ROLL
//#define PLOT_REAL_ACCELERATION
//#define PLOT_GYRO
//#define ENABLE_DMX

#if defined(ENABLE_LCD_16x2) || defined(ENABLE_LCD_20x4)
  #define ENABLE_LCD
#endif

#if defined(PLOT_YAW_PITCH_ROLL) || defined(PLOT_REAL_ACCELERATION) || defined(PLOT_GYRO)
  #define ENABLE_PLOTTING
#endif

// To view the plots, run arduino-plotter/listener/listener.pde in Processing.
// The serial plotter tool built into the Arduino IDE will show only one value.


/************
 * Includes *
 ************/

#ifdef ENABLE_WATCHDOG
#include <avr/wdt.h>
#endif

#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

#ifdef ENABLE_LCD
#include <LiquidCrystal.h>
#endif

#ifdef ENABLE_PLOTTING
#include "Plotter.h"
#endif

#ifdef ENABLE_DMX
#include "DmxSimple.h"
#endif


/****************************
 * Constants and Data Types *
 ****************************/

enum class MpuMode {
  init,
  cycle,
  normal
};


/*****************
 * Configuration *
 *****************/

#define STANDBY_DISPLAY_TX_INTERVAL_MS 2000L
#define ACTIVE_DISPLAY_TX_INTERVAL_MS 100L
#define TEMPERATURE_SAMPLE_INTERVAL_MS 500L
#define DMX_TX_INTERVAL_MS 33L

// There must be at least YPR_MOTION_CHANGE_THRESHOLD tenths of a degree
// motion in yaw, pitch, or roll every MOTION_TIMEOUT_MS ms to keep us
// out of standby mode.
#define MOTION_TIMEOUT_MS 5000L
#define YPR_MOTION_CHANGE_THRESHOLD 1

#define IMU_INTERRUPT_PIN 2

#define NUM_MPU_VALUES_TO_SEND 9
#define MA_LENGTH 8
#define NUM_MA_SETS (NUM_MPU_VALUES_TO_SEND)

// When we haven't retrieved packets from the MPU6050's DMP's FIFO fast enough,
// data corruption becomes likely even before the FIFO overflows.  We'll clear
// the FIFO when more than maxPacketsInFifoBeforeForcedClear packets are in it.
constexpr uint8_t maxPacketsInFifoBeforeForcedClear = 2;

#define PLOT_NUM_POINTS 500
#define PLOT_Z_COLOR "orange"
#define PLOT_Y_COLOR "pink"
#define PLOT_X_COLOR "cyan"

constexpr double countsPerG = 8192.0;

#define LCD_RS A1
#define LCD_E  A0
#define LCD_D4  5
#define LCD_D5  6
#define LCD_D6  7
#define LCD_D7  8

#define DMX_OUTPUT_PIN 3

#define DMX_NUM_CHANNELS 8


/***********
 * Globals *
 ***********/

// standby/active control
static MpuMode mpuMode = MpuMode::init;
static uint32_t lastMotionDetectedMs;

// moving average variables
static int16_t maValues[NUM_MA_SETS][MA_LENGTH];
static int32_t maSums[NUM_MA_SETS];
static uint8_t maNextSlotIdx[NUM_MA_SETS];
static bool maSetFull[NUM_MA_SETS];

static MPU6050 mpu;               // using default I2C address 0x68

// MPU control/status vars
static bool dmpReady = false;     // set true if DMP init was successful
static uint16_t packetSize;       // expected DMP packet size (default is 42 bytes)
static uint8_t packetBuffer[42];  // must be at least as large as packet size returned by dmpGetFIFOPacketSize

// orientation/motion vars
static Quaternion quat;           // [w, x, y, z] quaternion container
static VectorFloat gravity;       // [x, y, z] gravity vector
static float ypr[3];              // [yaw, pitch, roll] yaw/pitch/roll container
static VectorInt16 aa;            // [x, y, z] accel sensor measurements
static VectorInt16 aaReal;        // [x, y, z] gravity-free accel sensor measurements
static int16_t rawAccel[3];
static int16_t gyro[3];
static float mpuTemperatureC;
static float mpuTemperatureF;

static volatile bool gotMpuInterrupt;

// stuff for Plotter
#ifdef ENABLE_PLOTTING
static Plotter p;
#endif
static double plotYaw;
static double plotPitch;
static double plotRoll;
static double plotRealAccelX;
static double plotRealAccelY;
static double plotRealAccelZ;
static double plotGyroX;
static double plotGyroY;
static double plotGyroZ;

#ifdef ENABLE_LCD
static LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
#endif


/******************
 * Implementation *
 ******************/

// top half of the MPU ISR (bottom half is processMpuInterrupt)
void handleMpuInterrupt() {
  gotMpuInterrupt = true;
}


void setMpuMode(MpuMode newMode, uint32_t now)
{
  switch (newMode)
  {
    case MpuMode::cycle:
#ifdef ENABLE_DEBUG_PRINT
      Serial.println(F("Setting mpuMode to cycle..."));
#endif
      mpu.setDMPEnabled(false);
      // TODO:  motion detection should be configurable
      // Set up motion detection.
      mpu.setMotionDetectionThreshold(1);         // unit is 2mg
      mpu.setMotionDetectionCounterDecrement(1);
      mpu.setMotionDetectionDuration(1);          // unit is ms
      // Put MPU-6050 in cycle mode.
      mpuMode = MpuMode::cycle;
      mpu.setWakeFrequency(0);                    // 0 = 1.25 Hz, 1 = 2.5 Hz, 2 - 5 Hz, 3 = 10 Hz
      mpu.setWakeCycleEnabled(true);
      mpu.setIntMotionEnabled(true);
      break;

    case MpuMode::normal:
#ifdef ENABLE_DEBUG_PRINT
      Serial.println(F("Setting mpuMode to normal..."));
#endif
      mpu.setIntMotionEnabled(false);
      mpu.setWakeCycleEnabled(false);
      clearMovingAverages();
      mpuMode = MpuMode::normal;
      mpu.setDMPEnabled(true);
      lastMotionDetectedMs = now;
      break;

    default:
#ifdef ENABLE_DEBUG_PRINT
      Serial.println(F("*** Invalid newMode in setMpuMode"));
#endif
      break;
  }
}


void initI2c()
{
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#ifdef ENABLE_DEBUG_PRINT
  Serial.println(F("initI2c:  I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE"));
#endif
  Wire.begin();
  TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
#ifdef ENABLE_DEBUG_PRINT
  Serial.println(F("initI2c:  I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE"));
#endif
  Fastwire::setup(400, true);
#endif
}


void initMpu()
{
#ifdef ENABLE_DEBUG_PRINT
  Serial.println(F("Initializing MPU6050..."));
#endif
  mpu.initialize();

#ifdef ENABLE_DEBUG_PRINT
  Serial.println(F("Testing MPU6050 connection..."));
#endif
  if (mpu.testConnection()) {
#ifdef ENABLE_DEBUG_PRINT
    Serial.println(F("MPU6050 connection successful.  Initializing DMP..."));
#endif

    uint8_t devStatus = mpu.dmpInitialize();
    if (devStatus == 0) {
  
      // supply your own gyro offsets here, scaled for min sensitivity
      // TODO 2/28/2018 ross:  What do we do about this?  Every widget could be different.
      //mpu.setXGyroOffset(220);
      //mpu.setYGyroOffset(76);
      //mpu.setZGyroOffset(-85);
      //mpu.setZAccelOffset(1788); // 1688 factory default for my test chip
  
#ifdef ENABLE_DEBUG_PRINT
      Serial.println(F("Enabling interrupt..."));
#endif
      attachInterrupt(digitalPinToInterrupt(IMU_INTERRUPT_PIN), handleMpuInterrupt, RISING);

      // Get expected DMP packet size, and make sure packetBuffer is large enough.
      packetSize = mpu.dmpGetFIFOPacketSize();
      if (sizeof(packetBuffer) >= packetSize) {
#ifdef ENABLE_DEBUG_PRINT
        Serial.println(F("DMP ready."));
#endif
        dmpReady = true;

        setMpuMode(MpuMode::cycle, millis());
      }
      else {
        Serial.print(F("*** FIFO packet size "));
        Serial.print(packetSize);
        Serial.print(F(" is larger than packetBuffer size "));
        Serial.println(sizeof(packetBuffer));
      }
    }
    else {
      // Well, shit.
      // 1 = initial memory load failed (most likely)
      // 2 = DMP configuration updates failed
#ifdef ENABLE_DEBUG_PRINT
      Serial.print(F("*** DMP Initialization failed.  devStatus="));
      Serial.println(devStatus);
#endif
    }
  }
  else {
#ifdef ENABLE_DEBUG_PRINT
  Serial.println(F("MPU6050 connection failed."));
#endif
  }
}


void initLcd()
{
#if defined(ENABLE_LCD_16x2)
  lcd.begin(20, 2);
#elif defined(ENABLE_LCD_20x4)
  lcd.begin(20, 4);
#endif
}


void initPlotting()
{
#ifdef ENABLE_PLOTTING
  uint8_t plotIdx = 0;
#ifdef ENABLE_DEBUG_PRINT
  Serial.println(F("Creating plots..."));
#endif
  p.Begin();
#ifdef PLOT_YAW_PITCH_ROLL
  p.AddTimeGraph("Yaw/Pitch/Roll (deg)", PLOT_NUM_POINTS, "Yaw", plotYaw, "Pitch", plotPitch, "Roll", plotRoll);
  p.SetColor(plotIdx++, PLOT_Z_COLOR, PLOT_Y_COLOR, PLOT_X_COLOR);
#endif
#ifdef PLOT_REAL_ACCELERATION
  p.AddTimeGraph("Real Acceleration (g)", PLOT_NUM_POINTS, "X", plotRealAccelX, "Y", plotRealAccelY, "Z", plotRealAccelZ);
  p.SetColor(plotIdx++, PLOT_X_COLOR, PLOT_Y_COLOR, PLOT_Z_COLOR);
#endif
#ifdef PLOT_GYRO
  p.AddTimeGraph("Gyroscope Data (deg/s)", PLOT_NUM_POINTS, "X", plotGyroX, "Y", plotGyroY, "Z", plotGyroZ);
  p.SetColor(plotIdx++, PLOT_X_COLOR, PLOT_Y_COLOR, PLOT_Z_COLOR);
#endif
#ifdef ENABLE_DEBUG_PRINT
  Serial.print(F("Created "));
  Serial.print(plotIdx);
  Serial.println(F(" plots."));
#endif
#endif
}


void initDmx()
{
#ifdef ENABLE_DMX
  DmxSimple.usePin(DMX_OUTPUT_PIN);
  DmxSimple.maxChannel(DMX_NUM_CHANNELS);
#endif
}


void setup()
{
#if defined(ENABLE_DEBUG_PRINT) || defined(ENABLE_PLOTTING)
  Serial.begin(115200);
#endif

  initI2c();
  initMpu();
  initLcd();
  initPlotting();
  initDmx();

  // Communication with the MPU6050 has proven to be problematic.
  // If we don't hear from the unit periodically, or if we don't
  // get good data for a while, we need the watchdog to reset us.
#ifdef ENABLE_WATCHDOG
  wdt_enable(WDTO_1S);     // enable the watchdog
#endif
}


void clearMovingAverages()
{
  for (uint8_t i = 0; i < NUM_MA_SETS; ++i) {
    for (uint8_t j = 0; j < MA_LENGTH; ++j) {
      maValues[i][j] = 0;
    }
    maSums[i] = 0;
    maNextSlotIdx[i] = 0;
    maSetFull[i] = false;
  }
}


void updateMovingAverage(uint8_t setIdx, int16_t newValue)
{
  maSums[setIdx] -= maValues[setIdx][maNextSlotIdx[setIdx]];
  maSums[setIdx] += newValue;
  maValues[setIdx][maNextSlotIdx[setIdx]] = newValue;

  ++maNextSlotIdx[setIdx];
  if (maNextSlotIdx[setIdx] >= MA_LENGTH) {
     maSetFull[setIdx] = true;
     maNextSlotIdx[setIdx] = 0;
  }
}


int16_t getMovingAverage(uint8_t setIdx)
{
  int32_t avg;
  if (maSetFull[setIdx]) {
    avg = maSums[setIdx] / (int32_t) MA_LENGTH;
  }
  else {
    avg = maNextSlotIdx[setIdx] > 0 ? (int32_t) maSums[setIdx] / (int32_t) maNextSlotIdx[setIdx] : 0;
  }

//#ifdef ENABLE_DEBUG_PRINT
//  Serial.print("getMovingAverage(");
//  Serial.print(setIdx);
//  Serial.print("):  ");
//  for (uint8_t i = 0; i < (maSetFull[setIdx] ? MA_LENGTH : maNextSlotIdx[setIdx]); ++i) {
//    Serial.print(i);
//    Serial.print(":");
//    Serial.print(maValues[setIdx][i]);
//    Serial.print("  ");
//  }
//  Serial.print(maSums[setIdx]);
//  Serial.print("  ");
//  Serial.println(avg);
//#endif

  return avg;
}


bool detectMovingAverageChange(uint8_t setIdx, int16_t threshold)
{
  uint8_t latestSlotIdx;
  if (maNextSlotIdx[setIdx] == 0) {
    if (!maSetFull[setIdx]) {
      return false;
    }
    latestSlotIdx = MA_LENGTH - 1;
  }
  else {
    latestSlotIdx = maNextSlotIdx[setIdx] - 1;
  }
  int16_t diff = maValues[setIdx][maNextSlotIdx[setIdx]] - maValues[setIdx][latestSlotIdx];
  return abs(diff) > threshold ? true : false;
}


void clearMpuFifo()
{
#ifdef ENABLE_DEBUG_PRINT
  Serial.println(F("Clearing FIFO..."));
#endif

  uint16_t fifoCount = mpu.getFIFOCount();
  while (fifoCount != 0) {
    uint8_t readLength = fifoCount >= packetSize ? packetSize : fifoCount;
    mpu.getFIFOBytes(packetBuffer, readLength);
    //fifoCount = mpu.getFIFOCount();
    fifoCount -= readLength;
  }

#ifdef ENABLE_DEBUG_PRINT
    Serial.println(F("Cleared FIFO."));
#endif
}


void gatherMotionMeasurements(uint32_t now)
{
  uint16_t fifoCount = mpu.getFIFOCount();
  while (fifoCount >= packetSize) {
    mpu.getFIFOBytes(packetBuffer, packetSize);
    fifoCount -= packetSize;

//#ifdef ENABLE_DEBUG_PRINT
//    // Careful:  We might not be able to keep up if this debug print is enabled.
//    Serial.print(F("Got packet from fifo.  fifoCount now "));
//    Serial.println(fifoCount);
//#endif

    mpu.dmpGetGyro(gyro, packetBuffer);
    mpu.dmpGetQuaternion(&quat, packetBuffer);
    mpu.dmpGetGravity(&gravity, &quat);
    mpu.dmpGetYawPitchRoll(ypr, &quat, &gravity);
    mpu.dmpGetAccel(&aa, packetBuffer);
    mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);

    updateMovingAverage(0, ypr[0] * (float) 1800 / M_PI);
    updateMovingAverage(1, ypr[1] * (float) 1800 / M_PI);
    updateMovingAverage(2, ypr[2] * (float) 1800 / M_PI);
    updateMovingAverage(3, aaReal.x);
    updateMovingAverage(4, aaReal.y);
    updateMovingAverage(5, aaReal.z);
//    updateMovingAverage(3, aa.x);
//    updateMovingAverage(4, aa.y);
//    updateMovingAverage(5, aa.z);
    updateMovingAverage(6, gyro[0]);
    updateMovingAverage(7, gyro[1]);
    updateMovingAverage(8, gyro[2]);

#ifdef ENABLE_WATCHDOG
    // If we're here and we got non-zero data, communication
    // with the MPU6050 is probably working, so kick the dog.
    bool gotNonzeroData = false;
    for (uint8_t i = 0; i < 3; ++i) {
      if (ypr[i] > 0.001 || gyro[i] != 0) {
        gotNonzeroData = true;
        break;
      }
    }
    if (gotNonzeroData) {
      wdt_reset();
    }
#endif

  // If there was sufficient motion, keep us out of standby mode for now.
  for (uint8_t i = 0; i <= 2; ++i) {
    if (detectMovingAverageChange(i, YPR_MOTION_CHANGE_THRESHOLD)) {
      lastMotionDetectedMs = now;
      break;
    }
  }

//#ifdef ENABLE_DEBUG_PRINT
//      // Careful:  We might not be able to keep up if this debug print is enabled.
//    Serial.print("ypr:  ");
//    Serial.print(ypr[0]);
//    Serial.print(", ");
//    Serial.print(ypr[1]);
//    Serial.print(", ");
//    Serial.print(ypr[2]);
//    Serial.print("    aaReal:  ");
//    Serial.print(aaReal.x);
//    Serial.print(", ");
//    Serial.print(aaReal.y);
//    Serial.print(", ");
//    Serial.print(aaReal.z);
//    Serial.print("    gyro:  ");
//    Serial.print(gyro[0]);
//    Serial.print(", ");
//    Serial.print(gyro[1]);
//    Serial.print(", ");
//    Serial.println(gyro[2]);
//#endif
  }
}


void gatherTemperatureMeasurement()
{
  int16_t rawTemperature = mpu.getTemperature();
  mpuTemperatureC = (float) rawTemperature / 340.0 + 36.53;
  mpuTemperatureF = mpuTemperatureC * 9.0/5.0 + 32.0;
}


void processMpuInterrupt(uint32_t now)
{
  static bool needClearMpuFifo;
  uint16_t fifoCount;

  uint8_t mpuIntStatus = mpu.getIntStatus();
//#ifdef ENABLE_DEBUG_PRINT
//  Serial.print("0x");
//  Serial.print((int) mpuIntStatus, HEX);
//  Serial.print(" ");
//#endif

  // Frequently, the interrupt status is zero by the time we're processing
  // the interrupt.  Why that happens has not yet been determined.  If it
  // is zero, there's nothing we can do because we don't know what the
  // interrupt was for.
  if (mpuIntStatus == 0) {
    return;
  }

  switch (mpuMode)
  {
    case MpuMode::init:
      // Don't do anything because the MPU isn't ready yet.
      break;

    case MpuMode::cycle:
      if (mpuIntStatus & 0x40) {
        setMpuMode(MpuMode::normal, now);
        clearMpuFifo();
      }
      break;

    case MpuMode::normal:

      fifoCount = mpu.getFIFOCount();

      // Check if FIFO overflowed.  If it did, clear it, then wait for another
      // data-ready interrupt and clear the FIFO again so that we are back in
      // sync (i.e., any partial packets have been cleared out).  Based on some
      // brief testing, it appears that bits 0 and 1 always appear set together.
      if (mpuIntStatus & 0x10) {
#ifdef ENABLE_DEBUG_PRINT
        Serial.print(F("*** FIFO overflow!  fifoCount is "));
        Serial.println(fifoCount);
#endif
        clearMpuFifo();
        needClearMpuFifo = true;    // clear it again after next interrupt to get in sync
        return;
      }

      // The MPU6050 register map document says that bit 0 indicates data ready and bit 1
      // is reserved.  However, the I2C data analyzer dump from Jeff Rowberg found at
      // https://www.i2cdevlib.com/tools/analyzer/1 shows that bit 0 indicates raw data
      // ready and bit 1 indicates DMP data ready.
      if (!(mpuIntStatus & 0x02)) {
#ifdef ENABLE_DEBUG_PRINT
        Serial.print(F("Got interrupt but not for data ready.  mpuIntStatus=0x"));
        Serial.print((int) mpuIntStatus, HEX);
        Serial.print(F(", fifoCount="));
        Serial.println(fifoCount);
#endif
        return;
      }

      if (needClearMpuFifo) {
        needClearMpuFifo = false;
        clearMpuFifo();
        return;
      }

      // If we've missed retrieving more than a few packets in time, the FIFO has
      // probably already overflowed (even though we haven't gotten that interrupt)
      // or otherwise become corrupted.  We need to reset it to avoid getting bad data.
      if (fifoCount > packetSize * maxPacketsInFifoBeforeForcedClear) {
#ifdef ENABLE_DEBUG_PRINT
        Serial.print(F("*** Missed too many packets.  fifoCount="));
        Serial.println(fifoCount);
#endif
        clearMpuFifo();
        return;
      }

      // If the FIFO length is not a multiple of the packet size, there is a partial
      // packet in the FIFO, either due to the FIFO being filled right now or due to some
      // sort of FIFO corruption.  We need to clear the FIFO to avoid getting bad data.
      if (fifoCount % packetSize != 0) {
#ifdef ENABLE_DEBUG_PRINT
        Serial.print(F("////////// *** Partial packet in FIFO.  fifoCount="));
        Serial.println(fifoCount);
#endif
        clearMpuFifo();
        needClearMpuFifo = true;    // clear it again after next interrupt to get in sync
        return;
      }

//#ifdef ENABLE_DEBUG_PRINT
//      Serial.print(F("Got data ready interrupt 0x"));
//      Serial.print((int) mpuIntStatus, HEX);
//      Serial.print(F(", fifoCount="));
//      Serial.println(fifoCount);
//#endif
      gatherMotionMeasurements(now);
      break;
  }
}


void sendMeasurements()
{
  plotYaw = getMovingAverage(0) / 10.0;
  plotPitch = getMovingAverage(1) / 10.0;
  plotRoll = getMovingAverage(2) / 10.0;
  plotRealAccelX = getMovingAverage(3) / countsPerG;
  plotRealAccelY = getMovingAverage(4) / countsPerG;
  plotRealAccelZ = getMovingAverage(5) / countsPerG;
  plotGyroX = getMovingAverage(6);
  plotGyroY = getMovingAverage(7);
  plotGyroZ = getMovingAverage(8);

#ifdef ENABLE_PLOTTING
  p.Plot();
#endif

#ifdef ENABLE_LCD_16x2
  // 0123456789012345
  // -ddd.d-dd.d-dd.d
  // -dddd-dddd-dddd
  // ddd.dxC  ddd.dxF
  char buf[17];
  lcd.setCursor(0, 0);
  lcd.print(dtostrf(plotYaw, 6, 1, buf));
  lcd.print(dtostrf(plotPitch, 5, 1, buf));
  lcd.print(dtostrf(plotRoll, 5, 1, buf));
  lcd.setCursor(0, 1);
//  // Display real acceleration as raw count values / 100.
//  sprintf(buf, "% 4d% 4d% 4d", getMovingAverage(3) / 100, getMovingAverage(4) / 100, getMovingAverage(5) / 100);
//  lcd.print(buf);
  lcd.print(dtostrf(mpuTemperatureC, 5, 1, buf));
  lcd.print((char) 223);    // degree symbol
  lcd.print("C  ");
  lcd.print(dtostrf(mpuTemperatureF, 5, 1, buf));
  lcd.print((char) 223);    // degree symbol
  lcd.print("F");
#endif

#ifdef ENABLE_DEBUG_PRINT
  Serial.print(F("yaw="));
  Serial.print(plotYaw);
  Serial.print(F(" pitch="));
  Serial.print(plotPitch);
  Serial.print(F(" roll="));
  Serial.print(plotRoll);
  Serial.print(F("   realAccelX="));
  Serial.print(plotRealAccelX);
  Serial.print(F(" realAccelY="));
  Serial.print(plotRealAccelY);
  Serial.print(F(" realAccelZ="));
  Serial.print(plotRealAccelZ);
  Serial.print(F("   gyroX="));
  Serial.print(plotGyroX);
  Serial.print(F(" gyroY="));
  Serial.print(plotGyroY);
  Serial.print(F(" gyroZ="));
  Serial.print(plotGyroZ);
  Serial.print(F("   tempC="));
  Serial.print(mpuTemperatureC);
  Serial.print(F(" tempF="));
  Serial.println(mpuTemperatureF);
#endif
}


void sendDmx()
{
#ifdef ENABLE_DMX

  const float channelAngleStep = 180.0 / (DMX_NUM_CHANNELS - 1);

  int intensities[DMX_NUM_CHANNELS];
  for (uint8_t i = 0; i < DMX_NUM_CHANNELS; ++i) {
    intensities[i] = 0;
  }

  // Normalize roll to 0-180 degrees.
  float roll = getMovingAverage(2) / 10.0 + 90.0;

  int section = floor(roll / channelAngleStep);
  // Fix up section if measurement is 180 degrees or more.
  if (section >= DMX_NUM_CHANNELS - 1) {
    section = DMX_NUM_CHANNELS - 2;
  }

  intensities[section] = map(roll, channelAngleStep * section, channelAngleStep * (section + 1), 255, 0);
  intensities[section + 1] = map(roll, channelAngleStep * section, channelAngleStep * (section + 1), 0, 255);

  for (uint8_t i = 0; i < DMX_NUM_CHANNELS; ++i) {
    DmxSimple.write(i+1, intensities[i]);
//#ifdef ENABLE_DEBUG_PRINT
//    Serial.print(F("sent DMX ch "));
//    Serial.print(i+1);
//    Serial.print(F(": "));
//    Serial.println(intensities[i]);    
//#endif
  }

#endif
}


void loop()
{
  static int32_t lastDisplayTxMs;
  static int32_t lastTemperatureSampleMs;
  static int32_t lastDmxTxMs;

  uint32_t now = millis();

  if (gotMpuInterrupt) {
    gotMpuInterrupt = false;
    processMpuInterrupt(now);
  }

#ifdef ENABLE_WATCHDOG
  // We don't get data periodically from the MPU when it
  // is in cycle mode, so we need to kick the dog here.
  if (mpuMode == MpuMode::cycle) {
    wdt_reset();
  }
#endif

  if (now - lastTemperatureSampleMs >= TEMPERATURE_SAMPLE_INTERVAL_MS) {
    lastTemperatureSampleMs = now;
    gatherTemperatureMeasurement();
  }

  if (now - lastDisplayTxMs >= (mpuMode == MpuMode::cycle ? STANDBY_DISPLAY_TX_INTERVAL_MS : ACTIVE_DISPLAY_TX_INTERVAL_MS)) {
    lastDisplayTxMs = now;
    sendMeasurements();
  }

  if (now - lastDmxTxMs >= DMX_TX_INTERVAL_MS) {
    lastDmxTxMs = now;
    sendDmx();
  }

  if (mpuMode == MpuMode::normal && now - lastMotionDetectedMs >= MOTION_TIMEOUT_MS) {
#ifdef ENABLE_DEBUG_PRINT
    Serial.print(F("Going standby because no motion from "));
    Serial.print(lastMotionDetectedMs);
    Serial.print(F(" to "));
    Serial.println(now);
#endif
    setMpuMode(MpuMode::cycle, now);
  }
}

