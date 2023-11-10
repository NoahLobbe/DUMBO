/*
 * CPP file for simple Ultrasonic Sensor Class
 */

#include "ultrasonic.h"

Ultrasonic::Ultrasonic(){
}

void Ultrasonic::init(uint8_t echo_pin, uint8_t trigger_pin, char id_char) {
  _echo = echo_pin;
  _trigger = trigger_pin;
  id_char = id_char;
  pinMode(_echo, INPUT);
  pinMode(_trigger, OUTPUT); 
  digitalWrite(_trigger, LOW);
}

void Ultrasonic::sendPulse() {
  //Serial.println(String(id_char) + " sensor is sending pulse");
  digitalWrite(_trigger, HIGH);
  delayMicroseconds(TRIGGER_LENGTH_US);
  digitalWrite(_trigger, LOW);
}

uint16_t Ultrasonic::getDistanceMM(unsigned long timeout_us) {
  sendPulse();
  duration_us = pulseIn(_echo, HIGH, timeout_us); //update
  curr_distance_mm = (SPEED_OF_SOUND_MM_PER_S * duration_us * 1e-6) / 2;
  //Serial.print(String(id_char) + " sensor duration_us: ");
  //Serial.print(duration_us);
  //Serial.println(", distance_mm: " + String(curr_distance_mm));

  return curr_distance_mm; 
}

void Ultrasonic::rememberDistance() {
  prev_distance_mm = curr_distance_mm; //bookmark
}
