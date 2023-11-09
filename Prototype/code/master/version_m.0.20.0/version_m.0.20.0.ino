/*
 * This version integrates the PIR motion sensor 
 * along with motor driver code form previous versions.
 * 
 * Next version make the PIR motion sensor into a class????
 * 
 * Version m.0.20.0
 * 
 * Last updated 24 August 2023, Noah Lobbe
 */


#include "MotorSystem.h"

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
bool pir_state = false; //keep track of the PIR motion state

////objects
MotorSystem RC_Car; //object to control the RC car motors


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
  pinMode(PIR_PIN, INPUT);
  pinMode(MOTION_LED_PIN, OUTPUT);

                
  Serial.begin(9600);
  Serial.println("...Enter a two char command...");
  Serial.println("Drive motor \n\tMotor Select: 'D' \n\tcommands: F = forward, B = backward, S = stop"
                 "\nSteering motor \n\tMotor Select: 'S' \n\tcommands: L = left, R = right, C = centre");
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


void checkPIR() {
  ////just checks the PIR motion sensor
  if (digitalRead(PIR_PIN)) { //if motion is detected...
    digitalWrite(MOTION_LED_PIN, HIGH);
    //Serial.println(digitalRead(PIR_PIN));

    if (pir_state == false) { //if motion hasn't been detected
      Serial.println("New Motion detected!");
      pir_state = true;
    }
    
  } else { //if motion not detected...
    digitalWrite(MOTION_LED_PIN, LOW);

    if (pir_state) { //reset pir_state due to no motion currently, compared to previously
      Serial.println("Motion ended");
      pir_state = false;
    }
  }
}

void loop() {
  ////inputs
  
  //userMotorControl();

  //PIR motion sensor
  checkPIR();


  ////Calculations
  

  ////outputs
  
  //probably should enact motor states here, NOT in userMotorControl()
  if (pir_state) {
    RC_Car.drive('F', 128);//forward half speed
    
  } else if (RC_Car.driving_direction != 'S'){ //no need to keep driving so stop
    RC_Car.drive('S', 128);//stop
  }

}
