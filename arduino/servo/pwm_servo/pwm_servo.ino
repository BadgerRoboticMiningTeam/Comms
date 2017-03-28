#define PWM_PIN 11

void setup() {
  Serial.begin(115200);
  pinMode(PWM_PIN, OUTPUT);
}

void loop() {
  if (Serial.available()) {
    analogWrite(PWM_PIN, Serial.read());
  }
}
