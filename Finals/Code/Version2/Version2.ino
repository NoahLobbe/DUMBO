/*
 * Integrating the front ultrasonic array with the motor controller.
 */

////Libraries
#include "MotorSystem.h"
#include "Ultrasonic.h"
#include "Functions.h"
#include "CustomDataTypes.h"


////pin definitions
///motor system
#define EN_STEERING_SPEED_PIN 10
#define STEERING_MOTOR_A_PIN 4
#define STEERING_MOTOR_B_PIN 5

#define DRIVING_MOTOR_A_PIN 7
#define DRIVING_MOTOR_B_PIN 6
#define EN_DRIVING_SPEED_PIN 11


///Front ultrasonic array
#define NUM_F_U_SENSORS 3
const uint8_t F_ECHO_PINS[NUM_F_U_SENSORS] = {A4,A2,A0};
const uint8_t F_TRIG_PINS[NUM_F_U_SENSORS] = {A5,A3,A1};
const char F_ID_CHARS[NUM_F_U_SENSORS] = {'L','F','R'};
const uint8_t DIRECTION_LED_PINS[NUM_F_U_SENSORS] = {2,3,8};
const uint8_t DUD_MEASUREMENT_LED_PIN = 9;



////variables and other constants
///general
#define PRINT_RESULTS true


///motor system
#define PWM_SPEED_ENABLED true 
#define LR_DIST_TOLERANCE_MM 100
char curr_direction = 'F'; 
const uint8_t NO_DIRECTION_LED_PIN = 12;


///Front ultrasonic array
#define PULSEIN_TIMEOUT_US 100000//the maximum time the pulseIn(...) function can take to wait for a signal. I think...

//loop update timing stuff
#define TIME_BETWEEN_U_SENSORS_MS 100 //ideally, 60ms between triggering an ultrasonic prevents wave interference 
unsigned long U_current_time_ms;
unsigned long U_previous_time_ms;

//Distance Stuff
uint16_t distances_mm[NUM_F_U_SENSORS];
#define MIN_DISTANCE 100




////objects
MotorSystem RC_Car(PRINT_RESULTS);

Ultrasonic F_U_Sensors[NUM_F_U_SENSORS];



void setup() {
  ///motor system
  RC_Car.attach(STEERING_MOTOR_A_PIN, STEERING_MOTOR_B_PIN,  DRIVING_MOTOR_A_PIN, DRIVING_MOTOR_B_PIN);
  
  if (PWM_SPEED_ENABLED) {
    RC_Car.enablePWM(EN_STEERING_SPEED_PIN, EN_DRIVING_SPEED_PIN);
    
  } else {
    pinMode(EN_STEERING_SPEED_PIN, OUTPUT);
    pinMode(EN_DRIVING_SPEED_PIN, OUTPUT);
    digitalWrite(EN_STEERING_SPEED_PIN, HIGH); //default to max
    digitalWrite(EN_DRIVING_SPEED_PIN, HIGH); //default to max
  }
  
  //LED indicators
  for (int i=0; i<NUM_F_U_SENSORS; i++) {
    pinMode(DIRECTION_LED_PINS[i], OUTPUT);
    digitalWrite(DIRECTION_LED_PINS[i], LOW);
  }
  pinMode(DUD_MEASUREMENT_LED_PIN, OUTPUT);
  digitalWrite(DUD_MEASUREMENT_LED_PIN, LOW);

  pinMode(NO_DIRECTION_LED_PIN, OUTPUT);
  digitalWrite(NO_DIRECTION_LED_PIN, LOW);


  ///Front ultrasonic array
  for (int i=0; i<NUM_F_U_SENSORS; i++) {
    F_U_Sensors[i].init(F_ECHO_PINS[i], F_TRIG_PINS[i], F_ID_CHARS[i]);
  }
  U_previous_time_ms = millis();


  ///Miscenialous stuff
  Serial.begin(9600);

}

void determineDirection() {
  char max_dist_char_id;
  
  if (getMin(distances_mm, NUM_F_U_SENSORS) == 0) { //the sensor which returns zero, actually has the largest distance, so grab that char id
    max_dist_char_id = F_ID_CHARS[minIndex(distances_mm, NUM_F_U_SENSORS)]; 
    digitalWrite(DUD_MEASUREMENT_LED_PIN, HIGH);
    Serial.print(" 0 return measurement ");

  } else {
    max_dist_char_id = F_ID_CHARS[maxIndex(distances_mm, NUM_F_U_SENSORS)]; //find which sensor has the largest distance
  }

  
  bool is_left_right_in_tolerance = withinTolerance(distances_mm[0], distances_mm[2], LR_DIST_TOLERANCE_MM); 

  Serial.print("is_left_right_in_tolerance: " + String(is_left_right_in_tolerance) + " maxChar: " + String(max_dist_char_id));

  if ((max_dist_char_id != 'F') && (!is_left_right_in_tolerance)) { //left/right has max distance... only change direction if there is significant difference between
    curr_direction = max_dist_char_id;
    /* redundant anyway...
    if (distances_mm[0] > distances_mm[2]) { //more space on the left...
      curr_direction = 'L';
    } else {
      curr_direction = 'R';
    }
    */
  
  } else if (max_dist_char_id == 'F') { //front has maximum distance...
   curr_direction = max_dist_char_id; 
  
  } else {
    //just keep going in same direction (which will be left/right)
  }
}


void loop() {
  U_current_time_ms = millis();

  if ((U_current_time_ms - U_previous_time_ms) >= TIME_BETWEEN_U_SENSORS_MS) {
    ////INPUTS
    //get distances
    for (int i=0; i<NUM_F_U_SENSORS; i++) {
      distances_mm[i] = F_U_Sensors[i].getDistanceMM(PULSEIN_TIMEOUT_US);
    }


    ////CALCULATIONS   
    //old quick method: curr_direction = F_ID_CHARS[maxIndex(distances_mm, NUM_F_U_SENSORS)]; //update based on max distance
    determineDirection();


    ////OUTPUTS
    //turn on the relevant direction LED
    int8_t pin_index = index(curr_direction, F_ID_CHARS, NUM_F_U_SENSORS);
    for (int i=0; i<NUM_F_U_SENSORS; i++) {
      bool state = (i == pin_index);
      digitalWrite(DIRECTION_LED_PINS[i], state); //turns the others LOW
    }
    
    
    

    if (getMin(distances_mm, NUM_F_U_SENSORS) == 0) {
      Serial.print(" dud measurement ");
      digitalWrite(DUD_MEASUREMENT_LED_PIN, HIGH);

      /*
      uint8_t min_index = minIndex(distances_mm, NUM_F_U_SENSORS);

      for (int i=0; i<NUM_F_U_SENSORS; i++) {
        if (i != min_index) {
          digitalWrite(DIRECTION_LED_PINS[i], HIGH); //turn all the indicator LEDs on EXCEPT the one which had the dud measurement 
        }
      }
      */
      
    } else {
      digitalWrite(DUD_MEASUREMENT_LED_PIN, LOW);
    }

    
    RC_Car.turn(curr_direction, 255);
    RC_Car.drive('F', 200);
    
    //print results
    Serial.print(strArray(distances_mm, NUM_F_U_SENSORS, false) + " | curr_direction: " + String(curr_direction));

    Serial.print("\n");

    U_previous_time_ms = U_current_time_ms; //boomark
  }

}
