#include <Wire.h>

char c;
int numChars;

void setup() {
  Serial.begin(115200);
  Wire.begin(1);
  Wire.onReceive(receiveHandler);
  Wire.onRequest(reqHandler);
}

void loop() {
  if (numChars > 0) {
    c = Wire.read();
    Serial.write(c);
    numChars--;
  }
}

void receiveHandler(int numBytes) {
  numChars = Wire.available();
}

void reqHandler() {
  Wire.write(c);
}

