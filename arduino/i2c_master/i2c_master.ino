#include <Wire.h>

void setup() {
  Serial.begin(115200);
  Wire.begin();
}

void loop() {
  uint8_t num;

  num = 'A';
  
  if (Serial.available()) {
    num = Serial.read();
    
    Wire.beginTransmission(1);
    Wire.write(num);
    Wire.endTransmission();

    Wire.requestFrom(1, 42);
    if (Wire.available()) {
      num = Wire.read();
      Serial.print(num);
    }
  }
}
