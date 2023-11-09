/*
 * Trying to reintroduce previous_distance and changing directions even when object is within range
 * 
 * Version: b.0.40.rebuild
 * Version made: 12 September 2023, Noah Lobbe
 * Version latest update: ???????????????????????????????????????????????????????????
 * 
 * How version system works, V.W.XY.Z
 * V is either 'b' or 'm' to indicate a branch or master
 * W is iteration of completed product.
 * X is incremented when a big function/feature/sensor is added
 * Y is just code improvements, no new stuff as that would classify as X
 * Z is the name of the branch, or if master then just = 0
 */


#include "MotorSystem.h"
//#include "biss0001_PIR.h"

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
#define PIR_PIN 9 //not implemented yet
#define MOTION_LED_PIN 8

//Ultrasonic sensor
#define ECHO_PIN 2
#define TRIGGER_PIN 3
#define STUCK_LED_PIN 12 //not implemented yet, except for start up indicators
#define OBJECT_DETECTED_LED_PIN 13


////variables
#define PRINT_RESULTS true
#define GET_USER_INPUT false

//motor driver
//bool determined_direction = false; // so that the random direction doesn't keep changing for the same object
#define DEFAULT_SPEED 200 //0-255 for motor PWM
uint16_t vehicle_speed = DEFAULT_SPEED;

//ultrasonic 
#define SPEED_OF_SOUND_MM_PER_S 340000
#define PULSEIN_TIMEOUT_US 100000//the maximum time the pulseIn(...) function can take to wait for a signal. I think.



#define OBJECT_AVOIDANCE_DISTANCE_THRESHOLD_MM 500 
//#define MINIMUM_TURNING_DISTANCE_MM 600


////objects
//object to control the RC car motors
MotorSystem RC_Car(PRINT_RESULTS); //print results parameter
//biss0001_PIR PIR_Sensor(PRINT_RESULTS); //pir motion sensor object


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
  //PIR_Sensor.attach(PIR_PIN); implemented later
  pinMode(MOTION_LED_PIN, OUTPUT);


  //Ultrasonc sensor
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  digitalWrite(TRIGGER_PIN, LOW); //no accidental triggering
  pinMode(STUCK_LED_PIN, OUTPUT);
  pinMode(OBJECT_DETECTED_LED_PIN, OUTPUT);

  
  ////Other stuff
  if (PRINT_RESULTS or GET_USER_INPUT) {       
    Serial.begin(9600);
    Serial.println("...Enter a two char command...");
    Serial.println("Drive motor \n\tMotor Select: 'D' \n\tcommands: F = forward, B = backward, S = stop"
                   "\nSteering motor \n\tMotor Select: 'S' \n\tcommands: L = left, R = right, C = centre");
  }

  randomSeed(analogRead(0));//make sure A0 doesn't have anything on it!!!

  //to signal that the arduino is in setup
  digitalWrite(OBJECT_DETECTED_LED_PIN, HIGH);
  for (int i=0; i<8; i++) {
    if (i%2 == 0) { //if i is even
      digitalWrite(STUCK_LED_PIN, HIGH);
      digitalWrite(MOTION_LED_PIN, LOW);
      
    } else {
      digitalWrite(STUCK_LED_PIN, LOW);
      digitalWrite(MOTION_LED_PIN, HIGH);
    }
    delay(250);
  }
  //remember to turn them all off!
  digitalWrite(OBJECT_DETECTED_LED_PIN, LOW);
  digitalWrite(STUCK_LED_PIN, LOW);
  digitalWrite(MOTION_LED_PIN, LOW);
  
}


void loop() {
  ////inputs
  if (GET_USER_INPUT) {
    //nothing to do :)
  }

  //PIR motion sensor
  //not implemented yet

  //Utlrasonic sensor
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10); //send the trigger pulse
  digitalWrite(TRIGGER_PIN, LOW);


  ////Calculations
  
  //Ultrasonic sensor
  uint16_t duration_uS = pulseIn(ECHO_PIN, HIGH, PULSEIN_TIMEOUT_US); //How many microseconds the ECHO pin pulse is HIGH. 
  uint16_t distance_mm = 0.5 * SPEED_OF_SOUND_MM_PER_S * duration_uS * 1e-6; //conversions done inline to prevent inaccuracies through rounding


  ////outputs
  if (PRINT_RESULTS) {
    Serial.print("uS: " + String(duration_uS) + " | distance (mm): " + String(distance_mm) + " | ");
  }

  
    if (distance_mm <= 0) {
      Serial.print("\t\t0m DISTANCE");
      digitalWrite(MOTION_LED_PIN, HIGH);
    } else {
      digitalWrite(MOTION_LED_PIN, LOW);
    }

  //algorithm
  if ((distance_mm <= OBJECT_AVOIDANCE_DISTANCE_THRESHOLD_MM) && (distance_mm > 0)) { //if distance_mm just means pulseIn(...) timed out
    Serial.print("...OBJECT DETECTED!!!...");
    digitalWrite(OBJECT_DETECTED_LED_PIN, HIGH);
    
    //if turning direction has NOT been determined then do so
    if (RC_Car.turning_direction == 'C') { 
      char direction_char = (random(0,100) >= 50) ? 'R':'L'; //ternary operators so much nicer to look at :D
      RC_Car.turn(direction_char, 255); 
    }
    RC_Car.drive('F', vehicle_speed); //need to actually go forward :D

    
  } else { //no objects to avoid...
    
    digitalWrite(OBJECT_DETECTED_LED_PIN, LOW);
    if (PRINT_RESULTS) Serial.print("...just driving...");
      
    if (RC_Car.turning_direction != 'C') { //don't need to turn anymore
      RC_Car.turn('C', 255); 
    }
    
    if (RC_Car.driving_direction != 'F') { //drive forward because there is nothing in the way
      RC_Car.drive('F', vehicle_speed); 
    }
  }



  ////update for next loop iteration
  if (PRINT_RESULTS) Serial.print("\n");
  
  delay(60); //to prevent sensor's sound waves from interfering with itself
}
