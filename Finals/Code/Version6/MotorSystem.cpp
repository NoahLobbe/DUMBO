/*
 * 
 */


#include "MotorSystem.h"


MotorSystem::MotorSystem(bool print_results) {
  _PRINT_RESULTS = print_results;
  _PWM_SPEED_ENABLED = false; //default
}


void MotorSystem::attach(uint8_t steering_servo, uint8_t driving_motor_a, uint8_t driving_motor_b) {
  _STEERING_SERVO_PIN = steering_servo;
  _DRIVING_MOTOR_A_PIN = driving_motor_a;
  _DRIVING_MOTOR_B_PIN = driving_motor_b;

  pinMode(_DRIVING_MOTOR_A_PIN, OUTPUT);
  pinMode(_DRIVING_MOTOR_B_PIN, OUTPUT);
  pinMode(_STEERING_SERVO_PIN, OUTPUT);
  digitalWrite(_STEERING_SERVO_PIN, LOW);
}


void MotorSystem::enablePWM(uint8_t driving_pwm_pin) {
  _PWM_SPEED_ENABLED = true; 
  _DRIVING_PWM_PIN = driving_pwm_pin;
  pinMode(_DRIVING_PWM_PIN, OUTPUT);
}

void MotorSystem::_writeSpeed(uint8_t pwm_pin, uint8_t speed_val) {
  
  if (_PWM_SPEED_ENABLED) {
    if (_PRINT_RESULTS) Serial.print("analog writing!!!");
    analogWrite(pwm_pin, speed_val);
  } else {
    analogWrite(pwm_pin, 255); //default to HIGH
  }
}


//this method handles everything for driving
void MotorSystem::drive(char driving_direction_char, uint8_t speed_val) {
  _writeSpeed(_DRIVING_PWM_PIN, speed_val); 
  if (driving_direction_char == 'F') {
    if (_PRINT_RESULTS) Serial.print("...forward...");
    //forward
    digitalWrite(_DRIVING_MOTOR_A_PIN, HIGH);
    digitalWrite(_DRIVING_MOTOR_B_PIN, LOW);
    
  } else if (driving_direction_char == 'R') {
    if (_PRINT_RESULTS) Serial.print("...reverse..."); //was backward till brake was introduced
    //backward
    digitalWrite(_DRIVING_MOTOR_A_PIN, LOW);
    digitalWrite(_DRIVING_MOTOR_B_PIN, HIGH);
    
  } else if (driving_direction_char == 'B') {
    if (_PRINT_RESULTS) Serial.print("...BRAKE...");
    digitalWrite(_DRIVING_MOTOR_A_PIN, HIGH);
    digitalWrite(_DRIVING_MOTOR_B_PIN, HIGH);
    
  } else {
    if (_PRINT_RESULTS) Serial.print("...stop...");
    //stop - stop powering the drive motor
    digitalWrite(_DRIVING_MOTOR_A_PIN, LOW);
    digitalWrite(_DRIVING_MOTOR_B_PIN, LOW);
  }
  
  driving_direction = driving_direction_char; //update for future use
}

void MotorSystem::brake(uint8_t speed_val) {
  _writeSpeed(_DRIVING_PWM_PIN, speed_val); 
  digitalWrite(_DRIVING_MOTOR_A_PIN, HIGH);
  digitalWrite(_DRIVING_MOTOR_B_PIN, HIGH);
  driving_direction = 'B';
}


//this method handles everything for steering
void MotorSystem::turn(char turning_direction_char, bool use_delay=true) {
  digitalWrite(_STEERING_SERVO_PIN, HIGH);
  uint16_t high_time;
  if (turning_direction_char == 'L') {
    if (_PRINT_RESULTS) Serial.print("...left...");
    high_time = 2200;
    
  } else if (turning_direction_char == 'R') {
    if (_PRINT_RESULTS) Serial.print("...right...");
    high_time = 900;
    
  } else { // 'C'
    if (_PRINT_RESULTS) Serial.print("...centre/stop...");
    high_time = 1550;
  }
  delayMicroseconds(high_time);
  digitalWrite(_STEERING_SERVO_PIN, LOW);
  if (use_delay) delayMicroseconds(20000 - high_time);
  
  turning_direction = turning_direction_char; //update for future use
}
