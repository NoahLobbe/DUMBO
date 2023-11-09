/*
 * Code modifications with ultrasonic, so that when distances 
 * are the same it probably means the vehicle is stuck.
 * 
 * Version m.0.31.0
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
#define STUCK_LED_PIN 12



////variables
#define PRINT_RESULTS true
#define GET_USER_INPUT false

//motor driver
bool determined_turn_direction = false; // so that the random direction doesn't keep changing for the same object
bool is_stuck = false;
#define STUCK_DISTANCE_MM 20
#define SPEED 200 //0-255 for motor PWM

//ultrasonic 
#define SPEED_OF_SOUND_MM_PER_S 340000
#define object_avoidance_distance_threshold_mm 1000 //these should probably be uppercase...
#define minimum_turning_distance_mm 900
#define STUCK_TOLERANCE 10//eg plus or minus 10 of current vs previous distances means the vehicle is probably stuck
uint16_t prev_distance_mm;


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
  pinMode(STUCK_LED_PIN, OUTPUT);

  
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

bool withinTolerance(uint16_t value, uint16_t previous_value, uint16_t tolerance) {
  if (abs(value - previous_value) <= tolerance) { //https://www.mathsisfun.com/numbers/absolute-value.html
    return true;
  } else {
    return false;
  }
  
}


void loop() {
  ////inputs
  if (GET_USER_INPUT) {
    userMotorControl();
  }

  //PIR motion sensor
  PIR_Sensor.checkSensor(); 

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
    Serial.print("\nuS: " + String(duration_uS) + " | distance (mm): " + String(distance_mm) + " | prev_distance: " + String(prev_distance_mm));
  }

  /*
  if (PIR_Sensor.pir_state) {
    time_to_escape = true;
    digitalWrite(MOTION_LED_PIN, HIGH);
    
  } else {
    time_to_escape = false;
    digitalWrite(MOTION_LED_PIN, LOW);
  }
  */

  ////Control algorithm

  //check if stuck first
  if (withinTolerance(distance_mm, prev_distance_mm, STUCK_TOLERANCE) && (distance_mm <= STUCK_DISTANCE_MM)) {
    if (PRINT_RESULTS) Serial.print("...STUCK...");
    is_stuck = true;
    digitalWrite(STUCK_LED_PIN, HIGH);
    RC_Car.drive('S', SPEED);
    
  } else {
    is_stuck = false;
    digitalWrite(STUCK_LED_PIN, LOW);
  }

  //when not stuck and motion has been detect then it is time to escape!
  if (!is_stuck) { //&& time_to_escape
    //obstacle avoidance algorithm
    
    if (distance_mm <= object_avoidance_distance_threshold_mm) { //avoid object
      char direction_char;
      if (!determined_turn_direction) { 
        if (random(0,100) >= 50) { // currently has no method of determine whether left/right is better, so random!
          direction_char = 'R';
        } else {
          direction_char = 'L';
        }
        determined_turn_direction = true;
      }
      RC_Car.turn(direction_char, 255);

      
    } else if (distance_mm <= minimum_turning_distance_mm) { //not enough space to avoid object, so halt
      RC_Car.drive('S', SPEED); 
      RC_Car.turn('C', 255);
      
       
    } else { //just drive forward, thats the best way to escape when there are no obstacles :D
      determined_turn_direction = false; //reset because
      
      if (RC_Car.turning_direction != 'C') { //don't need to turn anymore
        RC_Car.turn('C', 255); 
      }
    
      if (RC_Car.driving_direction != 'F') { //drive forward because there is nothing in the way
        RC_Car.drive('F', SPEED); 
      }
    }
    
  } else { //what to do if stuck...
    //something!
  }



  prev_distance_mm = distance_mm; //update for next loop iteration
  delay(60); //to prevent sensor's sound waves from interfering with itself. Could probably implement a timer instead
}
