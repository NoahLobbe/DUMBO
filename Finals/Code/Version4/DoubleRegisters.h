/*
 * Header file for class for using 2 74HC595N shift registers
 */

#ifndef DoubleRegisters_h
#define DoubleRegisters_h

#include "Arduino.h"


class DoubleRegisters {
  private:
    uint8_t _data_pin;
    uint8_t _output_latch_pin;
    uint8_t _data_clock_pin;
    uint8_t _reset_pin;

    uint16_t _registers_data = 0x00000000;

   public:
    const uint8_t NUM_PINS = 16;
   
    DoubleRegisters();
    void init(uint8_t data_pin, uint8_t latch_pin, uint8_t clock_pin, uint8_t clear_pin);
    void outputLatchPulse();
    void dataClockPulse();
    
    void setPin(uint8_t pin, bool state);
    void setAllPins(uint16_t new_register_data);
    void setAllBool(bool state);
    void togglePin(uint8_t pin);
    
    void writeOutputsLSB();
    void writeOutputsMSB();
    
    void reset();
    
    uint16_t getRegisterData();
  
};



#endif
