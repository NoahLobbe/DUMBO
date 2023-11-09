/*
 * 
 */

#ifndef MotorSystem_h
#define MotorSystem_h

#include "Arduino.h"

class MotorSystem { //make a class for handling the RC car motors
  private:
    uint8_t _DRIVING_MOTOR_A_PIN;
    uint8_t _DRIVING_MOTOR_B_PIN;
    uint8_t _STEERING_MOTOR_A_PIN;
    uint8_t _STEERING_MOTOR_B_PIN;

    bool _PWM_SPEED_ENABLED;
    uint8_t _STEERING_PWM_PIN;
    uint8_t _DRIVING_PWM_PIN;
    
    
  public:
    char driving_direction;
    char turning_direction;
    MotorSystem();
    void attach(uint8_t steering_motor_a, uint8_t steering_motor_b, uint8_t driving_motor_a, uint8_t driving_motor_b);
    void enablePWM(uint8_t steering_pwm_pin, uint8_t driving_pwm_pin);
    void _writeSpeed(uint8_t pwm_pin, uint8_t speed_val);
    void drive(char driving_direction_char, uint8_t speed_val);
    void turn(char turning_direction_char, uint8_t speed_val);
};

#endif
