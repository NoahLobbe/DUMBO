/*
 * 
 */

#include "Arduino.h"
#include "MotorSystem.h"


MotorSystem::MotorSystem() {
  _PWM_SPEED_ENABLED = false; //default
}


void MotorSystem::attach(uint8_t steering_motor_a, uint8_t steering_motor_b, uint8_t driving_motor_a, uint8_t driving_motor_b) {
  _STEERING_MOTOR_A_PIN = steering_motor_a;
  _STEERING_MOTOR_B_PIN = steering_motor_b;
  _DRIVING_MOTOR_A_PIN = driving_motor_a;
  _DRIVING_MOTOR_B_PIN = driving_motor_b;

  pinMode(_STEERING_MOTOR_A_PIN, OUTPUT);
  pinMode(_STEERING_MOTOR_B_PIN, OUTPUT);
  pinMode(_DRIVING_MOTOR_A_PIN, OUTPUT);
  pinMode(_DRIVING_MOTOR_B_PIN, OUTPUT);
}


void MotorSystem::enablePWM(uint8_t steering_pwm_pin, uint8_t driving_pwm_pin) {
  _PWM_SPEED_ENABLED = true; 
  _STEERING_PWM_PIN = steering_pwm_pin;
  _DRIVING_PWM_PIN = driving_pwm_pin;

  pinMode(_STEERING_PWM_PIN, OUTPUT);
  pinMode(_DRIVING_PWM_PIN, OUTPUT);
}

void MotorSystem::_writeSpeed(uint8_t pwm_pin, uint8_t speed_val) {
  Serial.println("_PWM_SPEED_ENABLED: " + String(_PWM_SPEED_ENABLED));
  if (_PWM_SPEED_ENABLED) {
    Serial.println("analog writing!!!");
    analogWrite(pwm_pin, speed_val);
  } else {
    analogWrite(pwm_pin, 255); //default to HIGH
  }
}


//this method handles everything for driving
void MotorSystem::drive(char driving_direction_char, uint8_t speed_val) {
  _writeSpeed(_DRIVING_PWM_PIN, speed_val); 
  if (driving_direction_char == 'F') {
    Serial.println("...forward...");
    //forward
    digitalWrite(_DRIVING_MOTOR_A_PIN, HIGH);
    digitalWrite(_DRIVING_MOTOR_B_PIN, LOW);
    
  } else if (driving_direction_char == 'B') {
    Serial.println("...backward...");
    //backward
    digitalWrite(_DRIVING_MOTOR_A_PIN, LOW);
    digitalWrite(_DRIVING_MOTOR_B_PIN, HIGH);
    
  } else {
    Serial.println("...stop...");
    //stop - stop powering the drive motor
    digitalWrite(_DRIVING_MOTOR_A_PIN, LOW);
    digitalWrite(_DRIVING_MOTOR_B_PIN, LOW);
  }
  
  driving_direction = driving_direction_char; //update for future use
}


//this method handles everything for steering
void MotorSystem::turn(char turning_direction_char, uint8_t speed_val) {
  _writeSpeed(_STEERING_PWM_PIN, speed_val);
  if (turning_direction_char == 'L') {
    Serial.println("...left...");
    //forward
    digitalWrite(_STEERING_MOTOR_A_PIN, HIGH);
    digitalWrite(_STEERING_MOTOR_B_PIN, LOW);
    
  } else if (turning_direction_char == 'R') {
    Serial.println("...right...");
    //backward
    digitalWrite(_STEERING_MOTOR_A_PIN, LOW);
    digitalWrite(_STEERING_MOTOR_B_PIN, HIGH);
    
  } else { // 'C'
    Serial.println("...centre...");
    //centre - spring system should centre the steering
    digitalWrite(_STEERING_MOTOR_A_PIN, LOW);
    digitalWrite(_STEERING_MOTOR_B_PIN, LOW);
  }
  
  turning_direction = turning_direction_char; //update for future use
}
