/*
 * 
 */


#include "MotorSystemReg.h"


MotorSystemReg::MotorSystemReg() {
  _PWM_SPEED_ENABLED = false; //default
}


void MotorSystemReg::init(uint8_t steering_motor_a, uint8_t steering_motor_b, uint8_t driving_motor_a, uint8_t driving_motor_b,
                          uint8_t data_pin, uint8_t latch_pin, uint8_t clock_pin, uint8_t reset_pin) {
  _STEERING_MOTOR_A_PIN = steering_motor_a;
  _STEERING_MOTOR_B_PIN = steering_motor_b;
  _DRIVING_MOTOR_A_PIN = driving_motor_a;
  _DRIVING_MOTOR_B_PIN = driving_motor_b;

  Registers.init(data_pin, latch_pin, clock_pin, reset_pin);
  Registers.setPin(_STEERING_MOTOR_A_PIN,  LOW);
  Registers.setPin(_STEERING_MOTOR_B_PIN,  LOW);
  Registers.setPin(_DRIVING_MOTOR_A_PIN,  LOW);
  Registers.setPin(_DRIVING_MOTOR_B_PIN,  LOW);

}


void MotorSystemReg::enablePWM(uint8_t steering_pwm_pin, uint8_t driving_pwm_pin) {
  _PWM_SPEED_ENABLED = true; 
  _STEERING_PWM_PIN = steering_pwm_pin;
  _DRIVING_PWM_PIN = driving_pwm_pin;

  //PWM not enabled in the shift registers
  pinMode(_STEERING_PWM_PIN, OUTPUT);
  pinMode(_DRIVING_PWM_PIN, OUTPUT);
}

void MotorSystemReg::_writeSpeed(uint8_t pwm_pin, uint8_t speed_val) {
  if (_PWM_SPEED_ENABLED) {
    analogWrite(pwm_pin, speed_val);
  } else {
    analogWrite(pwm_pin, 255); //default to HIGH
  }
}


//this method handles everything for driving
void MotorSystemReg::drive(char driving_direction_char, uint8_t speed_val) {
  _writeSpeed(_DRIVING_PWM_PIN, speed_val); 
  if (driving_direction_char == 'F') {
    //forward
    Registers.setPin(_DRIVING_MOTOR_A_PIN, HIGH);
    Registers.setPin(_DRIVING_MOTOR_B_PIN, LOW);
    
  } else if (driving_direction_char == 'R') { //was 'B' (backward) till brake was introduced
    //backward
    Registers.setPin(_DRIVING_MOTOR_A_PIN, LOW);
    Registers.setPin(_DRIVING_MOTOR_B_PIN, HIGH);
    
  } else if (driving_direction_char == 'B') {
    Registers.setPin(_DRIVING_MOTOR_A_PIN, HIGH);
    Registers.setPin(_DRIVING_MOTOR_B_PIN, HIGH);
    
  } else {
    //stop powering the drive motor
    Registers.setPin(_DRIVING_MOTOR_A_PIN, LOW);
    Registers.setPin(_DRIVING_MOTOR_B_PIN, LOW);
  }
  
  driving_direction = driving_direction_char; //update for future use
}


//this method handles everything for steering
void MotorSystemReg::turn(char turning_direction_char, uint8_t speed_val) {
  _writeSpeed(_STEERING_PWM_PIN, speed_val);
  if (turning_direction_char == 'L') {
    //forward
    Registers.setPin(_STEERING_MOTOR_A_PIN, HIGH);
    Registers.setPin(_STEERING_MOTOR_B_PIN, LOW);
    
  } else if (turning_direction_char == 'R') {
    //backward
    Registers.setPin(_STEERING_MOTOR_A_PIN, LOW);
    Registers.setPin(_STEERING_MOTOR_B_PIN, HIGH);
    
  } else {
    //spring system should centre the steering
    Registers.setPin(_STEERING_MOTOR_A_PIN, LOW);
    Registers.setPin(_STEERING_MOTOR_B_PIN, LOW);
  }

  
  turning_direction = turning_direction_char; //update for future use
}
