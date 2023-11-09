/*
 * Simple header for a class for the biss0001 PIR motion sensor
 * 
 */

#ifndef biss0001_PIR_h
#define biss0001_PIR_h

#include "Arduino.h"

class biss0001_PIR {
  private:
    uint8_t _PIN;
    bool _PRINT_RESULTS;

  public:
    bool pir_state = false;
  
    biss0001_PIR(bool print_results);
    void attach(uint8_t pin);
    bool checkSensor(); //returns if the
};

#endif
