/*
 * Adding Ultrasonic sensor!
 * Disabling PIR motion sensor so that it is easier to work with
 * 
 * Version m.0.30.0
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

//Ultrasonic sensor
#define ECHO_PIN 2
#define TRIGGER_PIN 3


////variables
#define PRINT_RESULTS false
#define GET_USER_INPUT false

//motor driver
bool determined_direction = false; // so that the random direction doesn't keep changing for the same object
#define SPEED 200 //0-255 for motor PWM

//ultrasonic 
#define SPEED_OF_SOUND_MM_PER_S 340000
#define object_avoidance_distance_threshold_mm 1000 //these should probably be uppercase...
#define minimum_turning_distance_mm 600


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

  //Ultrasonc sensor
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  digitalWrite(TRIGGER_PIN, LOW); //no accidental triggering

  
  ////Other stuff
  if (PRINT_RESULTS or GET_USER_INPUT) {       
    Serial.begin(9600);
    Serial.println("...Enter a two char command...");
    Serial.println("Drive motor \n\tMotor Select: 'D' \n\tcommands: F = forward, B = backward, S = stop"
                   "\nSteering motor \n\tMotor Select: 'S' \n\tcommands: L = left, R = right, C = centre");
  }

  randomSeed(analogRead(0));//make sure A0 doesn't have anything on it!!!
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
  //PIR_Sensor.checkSensor(); disable for the moment

  //Utlrasonic sensor
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10); //send the trigger pulse
  digitalWrite(TRIGGER_PIN, LOW);


  ////Calculations
  
  //Ultrasonic sensor
  
  uint16_t duration_uS = pulseIn(ECHO_PIN, HIGH, 0); //Time (in microseconds, uS) how long the ECHO pin pulse is HIGH
  
  uint16_t distance_mm = 0.5 * SPEED_OF_SOUND_MM_PER_S * duration_uS * 1e-6; //conversions done inline to prevent inaccuracies through rounding


  ////outputs
  if (PRINT_RESULTS) {
    Serial.println("\nuS: " + String(duration_uS) + " | distance (mm): " + String(distance_mm));
  }

  /*
  //probably should enact motor states here, NOT in userMotorControl()
  if (PIR_Sensor.pir_state) {
    digitalWrite(MOTION_LED_PIN, HIGH);
    RC_Car.drive('F', SPEED);//forward half speed
    
  } else {
    digitalWrite(MOTION_LED_PIN, LOW);
    if (RC_Car.driving_direction != 'S'){ //no need to keep driving so stop. This if statement also prevents it from running needlessly
      RC_Car.drive('S', SPEED);//stop
    } 
  }
  */

  //algorithm
  if (distance_mm <= object_avoidance_distance_threshold_mm) {
    //RC_Car.turn('R', 255); //default turn right
    char direction_char;
    if (!determined_direction) {
      if (random(0,100) >= 50) {
        direction_char = 'R';
      } else {
        direction_char = 'L';
      }
      determined_direction = true;
    }
    RC_Car.turn(direction_char, 255);
    
      
  } else if (distance_mm <= minimum_turning_distance_mm) {
    RC_Car.drive('S', SPEED); //don't go any futher because turning won't avoid the object
     
  } else {
    determined_direction = false; //reset
    
    if (RC_Car.turning_direction != 'C') {
      RC_Car.turn('C', 255); //return to centre if previously turning and don't need to do so anymore
    }
  
    if (RC_Car.driving_direction != 'F') {
      RC_Car.drive('F', SPEED); //drive forward because there is nothing in the way
    }
  }

  
  delay(60); //to prevent sensor's sound waves from interfering with itself
}
