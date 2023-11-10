/*
 * Header file for simple Ultrasonic Sensor Class
 */

#ifndef Ultrasonic_h
#define Ultrasonic_h

#include "Arduino.h"
#include "custom_datatypes.h"


#define SPEED_OF_SOUND_MM_PER_S 340000


class Ultrasonic { /////potentially get rid of the timeout_us and replace with #define to save 4bytes
  private:
    uint8_t _echo;
    uint8_t _trigger;
  public:
    char id_char;
    const uint8_t TRIGGER_LENGTH_US = 10;
    //const uint32_t SPEED_OF_SOUND_MM_PER_S = 340000;
    unsigned long duration_us; // so that it is accessable for later
    uint16_t prev_distance_mm;
    uint16_t curr_distance_mm;
    
    Ultrasonic(char char_name);
    void attach(uint8_t echo_pin, uint8_t trigger_pin);
    void sendPulse();
    uint16_t getDistanceMM(unsigned long timeout_us);
    void rememberDistance();
};


#endif
