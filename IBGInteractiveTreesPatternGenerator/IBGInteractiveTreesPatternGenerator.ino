/***********************************************
 *                                             *
 *   Idaho Botanical Garden Interactive Trees  *
 *               Pattern Generator             *
 *                                             *
 * Ross Butler  August 2018                    *
 *                                             *
 ***********************************************/


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

#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

#include "DmxSimple.h"


/*****************
 * Configuration *
 *****************/

#define DISPLAY_TX_INTERVAL_MS 250L
#define TEMPERATURE_SAMPLE_INTERVAL_MS 500L
#define DMX_TX_INTERVAL_MS 33L

#define IMU_INTERRUPT_PIN 2

#define NUM_MA_SETS 9
#define MA_LENGTH 8

// When we haven't retrieved packets from the MPU6050's DMP's FIFO fast enough,
// data corruption becomes likely even before the FIFO overflows.  We'll clear
// the FIFO when more than maxPacketsInFifoBeforeReset packets are in it.
constexpr uint8_t maxPacketsInFifoBeforeReset = 2;

constexpr double countsPerG = 8192.0;

#define DMX_OUTPUT_PIN 3
#define DMX_NUM_CHANNELS 8

#define NUM_COLORS_PER_TREE 3
#define NUM_TREES 3
#define TREE_MIN_INTENSITY 96

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

// moving average variables
static int16_t maValues[NUM_MA_SETS][MA_LENGTH];
static int32_t maSums[NUM_MA_SETS];
static uint8_t nextSlotIdx[NUM_MA_SETS];
static bool maSetFull[NUM_MA_SETS];

static MPU6050 mpu;               // using default I2C address 0x68

// MPU control/status vars
static bool dmpReady = false;     // set true if DMP init was successful
static uint8_t devStatus;         // return status after each device operation (0 = success, !0 = error)
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

static volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high

static double avgYaw;
static double avgPitch;
static double avgRoll;
static double avgRealAccelX;
static double avgRealAccelY;
static double avgRealAccelZ;
static double avgGyroX;
static double avgGyroY;
static double avgGyroZ;



/******************
 * Implementation *
 ******************/

void dmpDataReady() {
    mpuInterrupt = true;
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
    devStatus = mpu.dmpInitialize();
    if (devStatus == 0) {
  
      // supply your own gyro offsets here, scaled for min sensitivity
      // TODO 2/28/2018 ross:  What do we do about this?  Every widget could be different.
      //mpu.setXGyroOffset(220);
      //mpu.setYGyroOffset(76);
      //mpu.setZGyroOffset(-85);
      //mpu.setZAccelOffset(1788); // 1688 factory default for my test chip
  
      // turn on the DMP, now that it's ready
#ifdef ENABLE_DEBUG_PRINT
      Serial.println(F("Enabling DMP..."));
#endif
      mpu.setDMPEnabled(true);

#ifdef ENABLE_DEBUG_PRINT
      Serial.println(F("Enabling interrupt..."));
#endif
      attachInterrupt(digitalPinToInterrupt(IMU_INTERRUPT_PIN), dmpDataReady, RISING);

      // Get expected DMP packet size, and make sure packetBuffer is large enough.
      packetSize = mpu.dmpGetFIFOPacketSize();
      if (sizeof(packetBuffer) >= packetSize) {
#ifdef ENABLE_DEBUG_PRINT
        Serial.println(F("DMP ready."));
#endif
        dmpReady = true;
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


void initDmx()
{
  DmxSimple.usePin(DMX_OUTPUT_PIN);
  DmxSimple.maxChannel(DMX_NUM_CHANNELS);
}


void setup()
{
#ifdef ENABLE_DEBUG_PRINT
  Serial.begin(115200);
#endif

  initI2c();
  initMpu();
  initDmx();

  // Communication with the MPU6050 has proven to be problematic.
  // If we don't hear from the unit periodically, or if we don't
  // get good data for a while, we need the watchdog to reset us.
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


void updateMovingAverage(uint8_t setIdx, int16_t newValue)
{
  maSums[setIdx] -= maValues[setIdx][nextSlotIdx[setIdx]];
  maSums[setIdx] += newValue;
  maValues[setIdx][nextSlotIdx[setIdx]] = newValue;

  ++nextSlotIdx[setIdx];
  if (nextSlotIdx[setIdx] >= MA_LENGTH) {
     maSetFull[setIdx] = true;
     nextSlotIdx[setIdx] = 0;
  }
}


int16_t getMovingAverage(uint8_t setIdx)
{
  int32_t avg;
  if (maSetFull[setIdx]) {
    avg = maSums[setIdx] / (int32_t) MA_LENGTH;
  }
  else {
    avg = nextSlotIdx[setIdx] > 0 ? (int32_t) maSums[setIdx] / (int32_t) nextSlotIdx[setIdx] : 0;
  }

//#ifdef ENABLE_DEBUG_PRINT
//  Serial.print("getMovingAverage(");
//  Serial.print(setIdx);
//  Serial.print("):  ");
//  for (uint8_t i = 0; i < (maSetFull[setIdx] ? MA_LENGTH : nextSlotIdx[setIdx]); ++i) {
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


void resetDmpFifo()
{
#ifdef ENABLE_DEBUG_PRINT
    Serial.println(F("Resetting DMP FIFO..."));
#endif
    mpu.resetFIFO();
    while (true) {
      if (mpuInterrupt) {
        mpuInterrupt = false;
        uint8_t mpuIntStatus = mpu.getIntStatus();
        if (mpuIntStatus & 0x01) {
          mpu.resetFIFO();
          break;
        }
      }
    }
#ifdef ENABLE_DEBUG_PRINT
    Serial.println(F("Cleared FIFO and re-sync'd."));
#endif
}


void gatherMotionMeasurements()
{
//#ifdef ENABLE_DEBUG_PRINT
//    Serial.println(F("gather motion"));
//#endif

  mpuInterrupt = false;
  uint8_t mpuIntStatus = mpu.getIntStatus();
  uint16_t fifoCount = mpu.getFIFOCount();

  // Check if FIFO overflowed.  If it did, clear it, then wait for another
  // data-ready interrupt and clear the FIFO again so that we are back in
  // sync (i.e., any partial packets have been cleared out).  Based on some
  // brief testing, it appears that bits 0 and 1 always appear set together.
  if (mpuIntStatus & 0x10) {
#ifdef ENABLE_DEBUG_PRINT
    Serial.print(F("*** FIFO overflow!  fifoCount is "));
    Serial.println(fifoCount);
#endif
    resetDmpFifo();
    return;
  }

  // The MPU6050 register map document says that bit 0 indicates data ready and bit 1
  // is reserved.  However, the I2C data analyzer dump from Jeff Rowberg found at
  // https://www.i2cdevlib.com/tools/analyzer/1 shows that bit 0 indicates raw data
  // ready and bit 1 indicates DMP data ready.
  if (!(mpuIntStatus & 0x02)) {
#ifdef ENABLE_DEBUG_PRINT
    Serial.print(F("Got interrupt but not for data ready.  mpuIntStatus="));
    Serial.print((int) mpuIntStatus);
    Serial.print(F(", fifoCount="));
    Serial.println(fifoCount);
#endif
    return;
  }

  // If we've missed retrieving more than a few packets in time, the FIFO has
  // probably already overflowed (even though we haven't gotten that interrupt)
  // or otherwise become corrupted.  We need to reset it to avoid getting bad data.
  if (fifoCount > packetSize * maxPacketsInFifoBeforeReset) {
#ifdef ENABLE_DEBUG_PRINT
    Serial.print(F("*** Missed too many packets.  fifoCount="));
    Serial.println(fifoCount);
#endif
    resetDmpFifo();
    return;
  }

  // If the FIFO length is not a multiple of the packet size, there is a partial
  // packet in the FIFO, either due to the FIFO being filled right now or due to
  // some sort of FIFO corruption.  We need to reset it to avoid getting bad data.
  if (fifoCount % packetSize != 0) {
#ifdef ENABLE_DEBUG_PRINT
    Serial.print(F("////////// *** Partial packet in FIFO.  fifoCount="));
    Serial.println(fifoCount);
#endif
    resetDmpFifo();
    return;
  }

  while (fifoCount >= packetSize) {
    fifoCount -= packetSize;
    mpu.getFIFOBytes(packetBuffer, packetSize);
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

#ifdef ENABLE_DEBUG_PRINT
      // Careful:  We might not be able to keep up if this debug print is enabled.
    Serial.print("ypr:  ");
    Serial.print(ypr[0]);
    Serial.print(", ");
    Serial.print(ypr[1]);
    Serial.print(", ");
    Serial.print(ypr[2]);
    Serial.print("    aaReal:  ");
    Serial.print(aaReal.x);
    Serial.print(", ");
    Serial.print(aaReal.y);
    Serial.print(", ");
    Serial.print(aaReal.z);
    Serial.print("    gyro:  ");
    Serial.print(gyro[0]);
    Serial.print(", ");
    Serial.print(gyro[1]);
    Serial.print(", ");
    Serial.println(gyro[2]);
#endif
  }
}


void gatherTemperatureMeasurement()
{
  int16_t rawTemperature = mpu.getTemperature();
  mpuTemperatureC = (float) rawTemperature / 340.0 + 36.53;
  mpuTemperatureF = mpuTemperatureC * 9.0/5.0 + 32.0;
}


void getAverageMeasurements()
{
  avgYaw = getMovingAverage(0) / 10.0;
  avgPitch = getMovingAverage(1) / 10.0;
  avgRoll = getMovingAverage(2) / 10.0;
  avgRealAccelX = getMovingAverage(3) / countsPerG;
  avgRealAccelY = getMovingAverage(4) / countsPerG;
  avgRealAccelZ = getMovingAverage(5) / countsPerG;
  avgGyroX = getMovingAverage(6);
  avgGyroY = getMovingAverage(7);
  avgGyroZ = getMovingAverage(8);

#ifdef ENABLE_DEBUG_PRINT
  Serial.print(F("avgYaw="));
  Serial.print(avgYaw);
  Serial.print(F(" avgPitch="));
  Serial.print(avgPitch);
  Serial.print(F(" avgRoll="));
  Serial.print(avgRoll);
  Serial.print(F(" avgRealAccelX="));
  Serial.print(avgRealAccelX);
  Serial.print(F(" avgRealAccelY="));
  Serial.print(avgRealAccelY);
  Serial.print(F(" avgRealAccelZ="));
  Serial.print(avgRealAccelZ);
  Serial.print(F(" avgGyroX="));
  Serial.print(avgGyroX);
  Serial.print(F(" avgGyroY="));
  Serial.print(avgGyroY);
  Serial.print(F(" avgGyroZ="));
  Serial.println(avgGyroZ);
#endif
}


void sendDmx()
{
  const float maxTiltDegrees = 30;   // restrict tilt to avoid gimbal lock
  const float colorAngleStep = maxTiltDegrees * 2 / (NUM_COLORS_PER_TREE - 1);
  const float treeAngleStep = maxTiltDegrees * 2 / (NUM_TREES - 1);

  int colorIntensities[NUM_COLORS_PER_TREE];
  for (uint8_t i = 0; i < NUM_COLORS_PER_TREE; ++i) {
    colorIntensities[i] = 0;
  }

  int treeIntensities[DMX_NUM_CHANNELS];
  for (uint8_t i = 0; i < DMX_NUM_CHANNELS; ++i) {
    treeIntensities[i] = TREE_MIN_INTENSITY;
  }

  int channelIntensities[NUM_TREES];
  for (uint8_t i = 0; i < NUM_TREES; ++i) {
    channelIntensities[i] = 0;
  }

  getAverageMeasurements();

  // Normalize and restrict pitch and roll to [0, maxTiltDegrees*2] degrees.
  float normalizedPitch;
  if (avgPitch <= - maxTiltDegrees) {
    normalizedPitch = 0;
  }
  else if (avgPitch >= maxTiltDegrees) {
    normalizedPitch = maxTiltDegrees * 2.0;
  }
  else {
    normalizedPitch = avgPitch + maxTiltDegrees;
  }
  float normalizedRoll;
  if (avgRoll <= - maxTiltDegrees) {
    normalizedRoll = 0;
  }
  else if (avgRoll >= maxTiltDegrees) {
    normalizedRoll = maxTiltDegrees * 2.0;
  }
  else {
    normalizedRoll = avgRoll + maxTiltDegrees;
  }

  // Fade across the colors based on pitch angle.
  int colorSection = floor(normalizedPitch / colorAngleStep);
  // Fix up colorSection if measurement is maxTiltDegrees degrees or more.
  if (colorSection >= NUM_COLORS_PER_TREE - 1) {
      colorSection = NUM_COLORS_PER_TREE - 2;
  }
  colorIntensities[colorSection] =
    map(normalizedPitch, colorAngleStep * colorSection, colorAngleStep * (colorSection + 1), 255, 0);
  colorIntensities[colorSection + 1] =
    map(normalizedPitch, colorAngleStep * colorSection, colorAngleStep * (colorSection + 1), 0, 255);

  // Fade across the trees based on roll angle.
  int treeSection = floor(normalizedRoll / treeAngleStep);
  // Fix up treeSection if measurement is maxTiltDegrees degrees or more.
  if (treeSection >= NUM_TREES - 1) {
      treeSection = NUM_TREES - 2;
  }
  treeIntensities[treeSection] =
    map(normalizedRoll, treeAngleStep * treeSection, treeAngleStep * (treeSection + 1), 255, TREE_MIN_INTENSITY);
  treeIntensities[treeSection + 1] =
    map(normalizedRoll, treeAngleStep * treeSection, treeAngleStep * (treeSection + 1), TREE_MIN_INTENSITY, 255);

  // For each tree, scale the intensity of each channel assigned
  // to that tree by the corresponding color intensity.
  for (uint8_t treeIdx = 0; treeIdx < NUM_TREES; ++treeIdx) {
    for (uint8_t colorIdx = 0; colorIdx < NUM_COLORS_PER_TREE; ++colorIdx) {
      uint8_t channelIdx = treeIdx * NUM_COLORS_PER_TREE + colorIdx;
      if (channelIdx < DMX_NUM_CHANNELS) {
        channelIntensities[channelIdx] = scale8(treeIntensities[treeIdx], colorIntensities[colorIdx]);
      }
    }
  }

  // Transmit the intensities via DMX.
  for (uint8_t i = 0; i < DMX_NUM_CHANNELS; ++i) {
    DmxSimple.write(i+1, channelIntensities[i]);
//#ifdef ENABLE_DEBUG_PRINT
//    Serial.print(F("sent DMX ch "));
//    Serial.print(i+1);
//    Serial.print(F(": "));
//    Serial.println(channelIntensities[i]);    
//#endif
  }
}


void loop()
{
  static int32_t lastDisplayTxMs;
  static int32_t lastTemperatureSampleMs;
  static int32_t lastDmxTxMs;

  uint32_t now = millis();

  if (dmpReady && mpuInterrupt) {
    gatherMotionMeasurements();
  }

  if (now - lastTemperatureSampleMs >= TEMPERATURE_SAMPLE_INTERVAL_MS) {
    lastTemperatureSampleMs = now;
    gatherTemperatureMeasurement();
  }

  if (now - lastDisplayTxMs >= DISPLAY_TX_INTERVAL_MS) {
    lastDisplayTxMs = now;
    getAverageMeasurements();
  }

  if (now - lastDmxTxMs >= DMX_TX_INTERVAL_MS) {
    lastDmxTxMs = now;
    sendDmx();
  }
}

