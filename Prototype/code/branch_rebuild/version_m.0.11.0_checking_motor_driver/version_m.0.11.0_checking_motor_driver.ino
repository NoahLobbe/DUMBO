/*
 * Much better implementation of the motor driver, 
 * including putting it in a class
 * 
 * Version m.0.11.0
 */


#include "MotorSystem.h"

//pin definitions
#define PWM_SPEED_ENABLED true 

#define EN_STEERING_SPEED_PIN 10
#define STEERING_MOTOR_A_PIN 4
#define STEERING_MOTOR_B_PIN 5

#define DRIVING_MOTOR_A_PIN 6
#define DRIVING_MOTOR_B_PIN 7
#define EN_DRIVING_SPEED_PIN 11




//variables


//objects
MotorSystem RC_Car; //object to control the RC car motors


void setup() {
  
  
  RC_Car.attach(STEERING_MOTOR_A_PIN, 
                STEERING_MOTOR_B_PIN, 
                DRIVING_MOTOR_A_PIN, 
                DRIVING_MOTOR_B_PIN);

  
  if (PWM_SPEED_ENABLED) {
    RC_Car.enablePWM(EN_STEERING_SPEED_PIN, EN_DRIVING_SPEED_PIN);
    
  } else {
    pinMode(EN_STEERING_SPEED_PIN, OUTPUT);
    pinMode(EN_DRIVING_SPEED_PIN, OUTPUT);
    digitalWrite(EN_STEERING_SPEED_PIN, HIGH); //default to max
    digitalWrite(EN_DRIVING_SPEED_PIN, HIGH); //default to max
  }

                
  Serial.begin(9600);
  Serial.println("...Enter a two char command...");
  Serial.println("Drive motor \n\tMotor Select: 'D' \n\tcommands: F = forward, B = backward, S = stop"
                 "\nSteering motor \n\tMotor Select: 'S' \n\tcommands: L = left, R = right, C = centre");
}

void loop() {

  if (Serial.available() > 0) {
    String input = Serial.readString();
    char motor_select = input[0];
    char command = input[1];
    int speed_value = (input.substring(2).toInt()); //converting
    String text = "\nYou inputted..."
                  "\n\tmotor select: " + String(motor_select) +
                  "\n\tcommand: " + String(command) +
                  "\n\tspeed value: " + String(speed_value);
    Serial.println(text);
    Serial.println(speed_value * 2);

    if (motor_select == 'D') {
      Serial.println("Command given to drive motor...");
      RC_Car.drive(command, speed_value);
      
    } else if (motor_select == 'S') {
      Serial.println("Command given to steering motor...");
      RC_Car.turn(command, speed_value);
      
    } else {
      Serial.println("...unrecognized motor_select...");
    }
    
  }

  //RC_Car.drive('F', 255);
  

}
