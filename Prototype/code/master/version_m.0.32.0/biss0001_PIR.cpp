/*
 * The .cpp file for the biss0001 PIR motion sensor class
 */

#include "Arduino.h"
#include "biss0001_PIR.h"


biss0001_PIR::biss0001_PIR(bool print_results) {
  _PRINT_RESULTS = print_results;
}


void biss0001_PIR::attach(uint8_t pin) {
  _PIN = pin;
  pinMode(_PIN, INPUT);
}


bool biss0001_PIR::checkSensor() {
  if (digitalRead(_PIN)) { //if motion is detected...

    if (pir_state == false) { //if motion hasn't been detected
      if (_PRINT_RESULTS) Serial.println("New Motion detected!");
      pir_state = true;
    }
    
  } else { //if motion not detected...
    
    if (pir_state) { //reset pir_state due to no motion currently, compared to previously
      if (_PRINT_RESULTS) Serial.println("Motion ended");
      pir_state = false;
    }
  }

  return pir_state; //just in case someone wants a return value instead of accessing the pir_state public member
}
