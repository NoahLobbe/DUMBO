/*
 * CPP file for class for using 2 74HC595N shift registers
 */

#include "DoubleRegisters.h"


DoubleRegisters::DoubleRegisters() {
  
}

void DoubleRegisters::init(uint8_t data_pin, uint8_t latch_pin, uint8_t clock_pin, uint8_t reset_pin) {
  _data_pin = data_pin;
  _output_latch_pin = latch_pin;
  _data_clock_pin = clock_pin;
  _reset_pin = reset_pin;

  pinMode(_data_pin, OUTPUT);
  pinMode(_output_latch_pin, OUTPUT);
  pinMode(_data_clock_pin, OUTPUT);
  pinMode(_reset_pin, OUTPUT);

  digitalWrite(_data_pin, LOW);
  digitalWrite(_output_latch_pin, LOW);
  digitalWrite(_data_clock_pin, LOW);
  digitalWrite(_reset_pin, HIGH); //inactive when HIGH
}

void DoubleRegisters::outputLatchPulse() {
  digitalWrite(_output_latch_pin, HIGH);
  digitalWrite(_output_latch_pin, LOW);
}

void DoubleRegisters::dataClockPulse() {
  digitalWrite(_data_clock_pin, HIGH);
  digitalWrite(_data_clock_pin, LOW);
}

void DoubleRegisters::setPin(uint8_t pin, bool state) {
  bitWrite(_registers_data, pin, state);
}

void DoubleRegisters::togglePin(uint8_t pin) {
  bool current_state = bitRead(_registers_data, pin);
  bitWrite(_registers_data, pin, !current_state);
}

void DoubleRegisters::setAllPins(uint16_t new_registers_data) {
  _registers_data = new_registers_data;
}

void DoubleRegisters::setAllBool(bool state) {
  _registers_data = 0xFFFF * state;
}

void DoubleRegisters::writeOutputsLSB() {
  for (uint8_t pin=0; pin<NUM_PINS; pin++) {
    bool state = bitRead(_registers_data, pin);
    digitalWrite(_data_pin, state);
    dataClockPulse(); //send into registers
  }

  outputLatchPulse(); //make the registers outputs go to their states
}

void DoubleRegisters::writeOutputsMSB() {
  for (int8_t pin=15; pin>=0; pin--) {
    bool state = bitRead(_registers_data, pin);
    digitalWrite(_data_pin, state);
    dataClockPulse(); 
  }
  outputLatchPulse(); 
}


void DoubleRegisters::reset() {
  digitalWrite(_reset_pin, LOW); //LOW to be held for ~5ns
  digitalWrite(_reset_pin, HIGH); //return to inactive state
}


uint16_t DoubleRegisters::getRegisterData() {
  return _registers_data;
}
