
//pin definitions
#define SIGNAL_PIN 6

//variables
#define CENTRE_US 1550
#define LEFT_US 2200
#define RIGHT_US 900
const int POSITIONS[] = {CENTRE_US, LEFT_US, CENTRE_US, RIGHT_US};
const int NUM_POSITIONS = sizeof(POSITIONS)/sizeof(int);

void setup() {
  pinMode(SIGNAL_PIN, OUTPUT);
  
}

void writeServo(int high_time) {
  digitalWrite(SIGNAL_PIN, HIGH);
  delayMicroseconds(high_time);
  digitalWrite(SIGNAL_PIN, LOW);
  delayMicroseconds(20000); //just easier to delay 20ms than calculate remainder
}

void loop() {
  /*
  if (Serial.available() > 0) {
    int us = Serial.parseInt();
    Serial.println(us);

    if (us < 0) {
      for (int i=500; i<2500; i++) {
        writeServo(i);
      }
    } else {
      writeServo(us);
    }
  }
  */
  for (int i=0; i<NUM_POSITIONS; i++) {
    writeServo(POSITIONS[i]);
    delay(1000);
  }
  

  

  

}
