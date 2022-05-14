#include <Wire.h>


// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int numRxBytes) {

  if (numRxBytes == 3) {
    uint8_t selectedLegPatternIdx = Wire.read();
    uint8_t selectedHeadAndBodyPatternIdx = Wire.read();
    uint8_t selectedEyePatternIdx = Wire.read();

    Serial.print(selectedLegPatternIdx);
    Serial.print(",");
    Serial.print(selectedHeadAndBodyPatternIdx);
    Serial.print(",");
    Serial.println(selectedEyePatternIdx);

    return;
  }
  
  Serial.print("Expected 3 bytes but received ");
  Serial.println(numRxBytes);
  for (uint8_t i = 0; i < numRxBytes; ++i) {
    uint8_t rxByte = Wire.read();
    Serial.println(rxByte);
  }
}


void setup() {
  Wire.begin(42);                     // join i2c bus with address 42
  Wire.onReceive(receiveEvent);       // register event
  Serial.begin(9600);                 // start serial for output
}


void loop() {
  delay(100);
}

