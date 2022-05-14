#include <Wire.h>

void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
}

void loop() {
  static uint8_t countdown = 10;

  if (--countdown == 0) {
    countdown = 10;

    Wire.requestFrom(42, 3);    // request 3 bytes from slave device #42
    for (uint8_t retry = 0; retry < 10; ++retry) {
      if (Wire.available() >= 3) {
        break;
      }
      Serial.println("waiting...");
      delay(100);      
    }

    uint8_t rxByteCount = Wire.available();
    if (rxByteCount == 0) {
      Serial.println("Request failed.");
    }
    else if (rxByteCount > 3) {
      Serial.println("Too many bytes received.  Discarding all.");
      while (Wire.available() != 0) {
        Wire.read();
      }
    }
    else {
      uint8_t selectedLegPattern = Wire.read();
      uint8_t selectedBodyPattern = Wire.read();
      uint8_t selectedEyePattern = Wire.read();
  
      Serial.print("leg=");
      Serial.print(selectedLegPattern);
      Serial.print(" body=");
      Serial.print(selectedBodyPattern);
      Serial.print(" eye=");
      Serial.println(selectedEyePattern);
    }
  }
    
  delay(100);
}

