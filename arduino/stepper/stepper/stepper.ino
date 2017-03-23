#define step1 2
#define step2 3
#define step3 4
#define step4 5

uint8_t currMode;

void spinPos(void) {
  switch (currMode) {
    case 0x3 :
      digitalWrite(step1, LOW);
      digitalWrite(step2, HIGH);
      digitalWrite(step3, HIGH);
      digitalWrite(step4, LOW);
      currMode = 0x6;
      break;
    case 0x6 :
      digitalWrite(step1, LOW);
      digitalWrite(step2, LOW);
      digitalWrite(step3, HIGH);
      digitalWrite(step4, HIGH);
      currMode = 0xC;
      break;
    case 0xC :
      digitalWrite(step1, HIGH);
      digitalWrite(step2, LOW);
      digitalWrite(step3, LOW);
      digitalWrite(step4, HIGH);
      currMode = 0x9;
      break;
    case 0x9 :
      digitalWrite(step1, HIGH);
      digitalWrite(step2, HIGH);
      digitalWrite(step3, LOW);
      digitalWrite(step4, LOW);
      currMode = 0x3;
      break;
  }
}

void spinNeg(void) {
  switch (currMode) {
    case 0x3 :
      digitalWrite(step1, HIGH);
      digitalWrite(step2, LOW);
      digitalWrite(step3, LOW);
      digitalWrite(step4, HIGH);
      currMode = 0x9;
      break;
    case 0x6 :
      digitalWrite(step1, HIGH);
      digitalWrite(step2, HIGH);
      digitalWrite(step3, LOW);
      digitalWrite(step4, LOW);
      currMode = 0x3;
      break;
    case 0xC :
      digitalWrite(step1, LOW);
      digitalWrite(step2, HIGH);
      digitalWrite(step3, HIGH);
      digitalWrite(step4, LOW);
      currMode = 0x6;
      break;
    case 0x9 :
      digitalWrite(step1, LOW);
      digitalWrite(step2, LOW);
      digitalWrite(step3, HIGH);
      digitalWrite(step4, HIGH);
      currMode = 0xC;
      break;
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(step1, OUTPUT);
  pinMode(step2, OUTPUT);
  pinMode(step3, OUTPUT);
  pinMode(step4, OUTPUT);
  
  digitalWrite(step1, HIGH);
  digitalWrite(step2, HIGH);
  digitalWrite(step3, LOW);
  digitalWrite(step4, LOW);

  currMode = 0x3;
}

void loop() {
  uint8_t num;
  uint16_t bigNum;

  spinPos();
  delay(500);
/*  
  if (Serial.available()) {
    num = Serial.read();

    bigNum = 64*((uint16_t)num);

    while (bigNum != 0) {
      if (bigNum > 0) {
        spinPos();
        delay(5);
        bigNum--;
      } else {
        spinNeg();
        delay(5);
        bigNum++;
      }
    }
  }*/
}
