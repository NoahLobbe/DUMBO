#include "DoubleRegisters.h"

//pin definitions
#define DATA_PIN 2
#define LATCH_PIN 3
#define CLOCK_PIN 4
#define RESET_PIN 7

//variables

//objects
DoubleRegisters Registers;

void setup() {
  Registers.init(DATA_PIN, LATCH_PIN, CLOCK_PIN, RESET_PIN);  
  Serial.begin(115200);
}

void loop() {
  if (Serial.available() > 0 ) {
    int input = Serial.parseInt();

    if (input > 16) {
      Registers.reset();
    } else {
      Registers.setPin(input, HIGH);
      Registers.writeOutputsMSB();
    }
  }
}
