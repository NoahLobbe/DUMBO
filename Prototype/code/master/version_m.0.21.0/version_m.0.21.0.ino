/*
 * This version makes the PIR motion into a class.
 * 
 * Also added bool to classes to disable/enable printing
 * 
 * Version m.0.21.0
 * 
 * Last updated 24 August 2023, Noah Lobbe
 */


#include "MotorSystem.h"
#include "biss0001_PIR.h"

////pin definitions

//motor driver
#define PWM_SPEED_ENABLED true 

#define EN_STEERING_SPEED_PIN 10
#define STEERING_MOTOR_A_PIN 4
#define STEERING_MOTOR_B_PIN 5

#define DRIVING_MOTOR_A_PIN 6
#define DRIVING_MOTOR_B_PIN 7
#define EN_DRIVING_SPEED_PIN 11

//PIR motion sensor
#define PIR_PIN 9
#define MOTION_LED_PIN 8 


////variables
#define PRINT_RESULTS false
#define GET_USER_INPUT true

////objects
MotorSystem RC_Car(PRINT_RESULTS); //object to control the RC car motors
biss0001_PIR PIR_Sensor(PRINT_RESULTS); //pir motion sensor object


void setup() {
  ////setup pinModes and pin allocations
  
  //Motor driver stuff
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

  //PIR motion sensor
  PIR_Sensor.attach(PIR_PIN);

  if (PRINT_RESULTS or GET_USER_INPUT) {       
    Serial.begin(9600);
    Serial.println("...Enter a two char command...");
    Serial.println("Drive motor \n\tMotor Select: 'D' \n\tcommands: F = forward, B = backward, S = stop"
                   "\nSteering motor \n\tMotor Select: 'S' \n\tcommands: L = left, R = right, C = centre");
  }
}


void userMotorControl() {
  ////Function to handle the user controling the motor system
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
}


void loop() {
  ////inputs
  if (GET_USER_INPUT) {
    userMotorControl();
  }

  //PIR motion sensor
  PIR_Sensor.checkSensor();


  ////Calculations
  

  ////outputs
  
  //probably should enact motor states here, NOT in userMotorControl()
  if (PIR_Sensor.pir_state) {
    digitalWrite(MOTION_LED_PIN, HIGH);
    RC_Car.drive('F', 128);//forward half speed
    
  } else {
    digitalWrite(MOTION_LED_PIN, LOW);
    if (RC_Car.driving_direction != 'S'){ //no need to keep driving so stop. This if statement also prevents it from running needlessly
      RC_Car.drive('S', 128);//stop
    } 
  }

}
