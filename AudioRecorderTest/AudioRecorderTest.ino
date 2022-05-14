// Record sound as raw data to a SD card, and play it back.
//
// Requires the audio shield:
//   http://www.pjrc.com/store/teensy3_audio.html

#include <Bounce.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// Use these with the Teensy Audio Shield
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

#define RECORD_BUTTON_PIN 2
#define STOP_BUTTON_PIN 1
#define PLAY_BUTTON_PIN 0

#define PLAY_LED_PIN 3
#define STOP_LED_PIN 4
#define RECORD_LED_PIN 5
#define ERROR_LED_PIN 16

static constexpr int inputSource = AUDIO_INPUT_MIC;   // AUDIO_INPUT_LINEIN or AUDIO_INPUT_MIC
static constexpr float lineOutputGain = 0.02;
static constexpr uint8_t lineOutputLevel = 29; // 29 (default) = 1.29 V p-p; range is 13 (3.16 V) to 31 (1.16 V)
static constexpr float headphoneVolume = 0.5;

// GUItool: begin automatically generated code
AudioInputI2S            i2s2;           //xy=221,154
AudioAnalyzePeak         peak1;          //xy=394,199
AudioRecordQueue         queue1;         //xy=397,154
AudioPlaySdRaw           playRaw1;       //xy=418,248
AudioMixer4              mixer1;         //xy=637,244
AudioOutputI2S           i2s1;           //xy=812,250
AudioConnection          patchCord1(i2s2, 0, queue1, 0);
AudioConnection          patchCord2(i2s2, 0, peak1, 0);
AudioConnection          patchCord3(playRaw1, 0, mixer1, 0);
AudioConnection          patchCord4(mixer1, 0, i2s1, 0);
AudioConnection          patchCord5(mixer1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=381,303
// GUItool: end automatically generated code

// Bounce objects to easily and reliably read the buttons
Bounce buttonRecord = Bounce(RECORD_BUTTON_PIN, 8);
Bounce buttonStop =   Bounce(STOP_BUTTON_PIN, 8);  // 8 = 8 ms debounce time
Bounce buttonPlay =   Bounce(PLAY_BUTTON_PIN, 8);

// Remember which mode we're doing
static int mode = 0;  // 0=stopped, 1=recording, 2=playing

// The file where data is recorded
static File frec;


void setup()
{
  pinMode(RECORD_BUTTON_PIN, INPUT_PULLUP);
  pinMode(STOP_BUTTON_PIN, INPUT_PULLUP);
  pinMode(PLAY_BUTTON_PIN, INPUT_PULLUP);

  pinMode(PLAY_LED_PIN, OUTPUT);
  pinMode(STOP_LED_PIN, OUTPUT);
  pinMode(RECORD_LED_PIN, OUTPUT);
  pinMode(ERROR_LED_PIN, OUTPUT);

  digitalWrite(PLAY_LED_PIN, LOW);
  digitalWrite(STOP_LED_PIN, HIGH);
  digitalWrite(RECORD_LED_PIN, LOW);
  digitalWrite(ERROR_LED_PIN, LOW);

  // Audio connections require memory, and the record queue
  // uses this memory to buffer incoming audio.
  AudioMemory(60);

  // Enable the audio shield, select input, and enable output
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(inputSource);
  sgtl5000_1.volume(headphoneVolume);
  sgtl5000_1.lineOutLevel(lineOutputLevel);
  mixer1.gain(0, lineOutputGain);

  // Initialize the SD card
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here if no SD card, but print a message
    digitalWrite(ERROR_LED_PIN, HIGH);
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
}


void loop()
{
  // First, read the buttons
  buttonRecord.update();
  buttonStop.update();
  buttonPlay.update();

  // Respond to button presses
  if (buttonRecord.fallingEdge()) {
    Serial.println("Record Button Press");
    if (mode == 2) stopPlaying();
    if (mode == 0) startRecording();
  }
  if (buttonStop.fallingEdge()) {
    Serial.println("Stop Button Press");
    if (mode == 1) stopRecording();
    if (mode == 2) stopPlaying();
  }
  if (buttonPlay.fallingEdge()) {
    Serial.println("Play Button Press");
    if (mode == 1) stopRecording();
    if (mode == 0) startPlaying();
  }

  // If we're playing or recording, carry on...
  if (mode == 1) {
    continueRecording();
  }
  if (mode == 2) {
    continuePlaying();
  }

  // when using a microphone, continuously adjust gain
  if (inputSource == AUDIO_INPUT_MIC) adjustMicLevel();
}


void startRecording() {
  Serial.println("startRecording");
  if (SD.exists("RECORD.RAW")) {
    // The SD library writes new data to the end of the
    // file, so to start a new recording, the old file
    // must be deleted before new data is written.
    SD.remove("RECORD.RAW");
  }
  frec = SD.open("RECORD.RAW", FILE_WRITE);
  if (frec) {
    queue1.begin();
    mode = 1;
    digitalWrite(STOP_LED_PIN, LOW);
    digitalWrite(RECORD_LED_PIN, HIGH);
    digitalWrite(ERROR_LED_PIN, LOW);
  }
  else {
    digitalWrite(ERROR_LED_PIN, HIGH);
  }
}


void continueRecording() {
  // TODO:  turn on error led if queue1 is full
  if (queue1.available() >= 2) {
    byte buffer[512];
    // Fetch 2 blocks from the audio library and copy
    // into a 512 byte buffer.  The Arduino SD library
    // is most efficient when full 512 byte sector size
    // writes are used.
    memcpy(buffer, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    memcpy(buffer+256, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    // write all 512 bytes to the SD card
    elapsedMicros usec = 0;
    frec.write(buffer, 512);
    // Uncomment these lines to see how long SD writes
    // are taking.  A pair of audio blocks arrives every
    // 5802 microseconds, so hopefully most of the writes
    // take well under 5802 us.  Some will take more, as
    // the SD library also must write to the FAT tables
    // and the SD card controller manages media erase and
    // wear leveling.  The queue1 object can buffer
    // approximately 301700 us of audio, to allow time
    // for occasional high SD card latency, as long as
    // the average write time is under 5802 us.
    //Serial.print("SD write, us=");
    //Serial.println(usec);
  }
}


void stopRecording() {
  Serial.println("stopRecording");
  queue1.end();
  if (mode == 1) {
    while (queue1.available() > 0) {
      frec.write((byte*)queue1.readBuffer(), 256);
      queue1.freeBuffer();
    }
    frec.close();
  }
  mode = 0;
  digitalWrite(RECORD_LED_PIN, LOW);
  digitalWrite(STOP_LED_PIN, HIGH);
}


void startPlaying() {
  Serial.println("startPlaying");
  playRaw1.play("RECORD.RAW");
  mode = 2;
  digitalWrite(PLAY_LED_PIN, HIGH);
  digitalWrite(STOP_LED_PIN, LOW);
  digitalWrite(ERROR_LED_PIN, LOW);
}


void continuePlaying() {
  if (!playRaw1.isPlaying()) {
    playRaw1.stop();
    mode = 0;
    digitalWrite(PLAY_LED_PIN, LOW);
    digitalWrite(STOP_LED_PIN, HIGH);
  }
}


void stopPlaying() {
  Serial.println("stopPlaying");
  if (mode == 2) playRaw1.stop();
  mode = 0;
  digitalWrite(PLAY_LED_PIN, LOW);
  digitalWrite(STOP_LED_PIN, HIGH);
}


void adjustMicLevel() {
  // TODO: read the peak1 object and adjust sgtl5000_1.micGain()
  // if anyone gets this working, please submit a github pull request :-)
}

