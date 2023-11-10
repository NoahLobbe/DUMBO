/*
 * Final version
 */

////Libraries
#include "MotorSystem.h"
#include "Ultrasonic.h"
#include "Functions.h"
#include "CustomDataTypes.h"


////pin definitions
///motor system
#define STEERING_SERVO_PIN 4
//#define EN_STEERING_SPEED_PIN 10
//#define STEERING_MOTOR_A_PIN 4
//#define STEERING_MOTOR_B_PIN 5

#define DRIVING_MOTOR_A_PIN 6
#define DRIVING_MOTOR_B_PIN 7
#define EN_DRIVING_SPEED_PIN 5


///Front ultrasonic array
#define NUM_F_U_SENSORS 3
const char F_ID_CHARS[NUM_F_U_SENSORS] = {'L','F','R'};

const uint8_t F_ECHO_PINS[NUM_F_U_SENSORS] = {A0,A2,A4};
const uint8_t F_TRIG_PINS[NUM_F_U_SENSORS] = {A1,A3,A5};

const uint8_t DIRECTION_LED_PINS[NUM_F_U_SENSORS] = {12,11,10};
const uint8_t ZERO_MEASUREMENT_LED_PIN = 8;
const uint8_t STUCK_LED_PIN = 9;


///PIT motion sensor
#define PIR_SIGNAL_PIN 2
#define HUMANS_LED_PIN 3


////variables and other constants
///general
#define PRINT_RESULTS false
bool is_vehicle_stuck = false;


///motor system
#define VEHICLE_SLOW_SPEED_LED_PIN 13
#define PWM_SPEED_ENABLED true 
#define NORMAL_SPEED 255
#define SLOW_SPEED 128
uint8_t vehicle_speed = NORMAL_SPEED;

char curr_turn_direction = 'F'; 
char curr_drive_direction = 'F';
bool has_braked = false;



///Front ultrasonic array
#define PULSEIN_TIMEOUT_US 100000//the maximum time the pulseIn(...) function can take to wait for a signal. I think...
#define ACUTE_ANGLE_BETWEEN_SENSORS_RAD (36 * (PI/180)) //36degrees, converting
#define DIST_DIFFERENCE_MM round(22 * sin(ACUTE_ANGLE_BETWEEN_SENSORS_RAD)) //calculating the 'height' difference between the front and side sensors, from centre of side sensor (~22mm)

//loop update timing stuff
#define TIME_BETWEEN_U_SENSORS_MS 100 //ideally, 60ms between triggering an ultrasonic prevents wave interference 
unsigned long U_current_time_ms;
unsigned long U_previous_time_ms;

///Distance Stuff
#define LR_DIST_TOLERANCE_MM 100
#define STUCK_TOLERANCE_MM 10
#define STUCK_DISTANCE_MM 100 
const uint16_t F_STUCK_DISTANCES_MM[NUM_F_U_SENSORS] = {DIST_DIFFERENCE_MM+STUCK_DISTANCE_MM, STUCK_DISTANCE_MM, DIST_DIFFERENCE_MM+STUCK_DISTANCE_MM};
#define IGNORE_DISTANCE_THRESHOLD_MM 2000
#define SLOW_DOWN_DISTANCE_THRESHOLD_MM 1000
#define BRAKE_DISTANCE_THRESHOLD_MM 200

uint16_t curr_distances_mm[NUM_F_U_SENSORS];
uint16_t prev_distances_mm[NUM_F_U_SENSORS];
bool front_sensor_stuck_states[NUM_F_U_SENSORS];


///PIR motion
bool is_human_present;




////objects
MotorSystem RC_Car(PRINT_RESULTS);

Ultrasonic F_U_Sensors[NUM_F_U_SENSORS];




void setup() {
  ///motor system
  RC_Car.attach(STEERING_SERVO_PIN,  DRIVING_MOTOR_A_PIN, DRIVING_MOTOR_B_PIN);
  
  if (PWM_SPEED_ENABLED) {
    RC_Car.enablePWM(EN_DRIVING_SPEED_PIN);
    pinMode(VEHICLE_SLOW_SPEED_LED_PIN, OUTPUT);
    
  } else {
    pinMode(EN_DRIVING_SPEED_PIN, OUTPUT);
    digitalWrite(EN_DRIVING_SPEED_PIN, HIGH); //default to max
  }
  
  ///Front ultrasonic array
  //LED indicators
  for (int i=0; i<NUM_F_U_SENSORS; i++) {
    pinMode(DIRECTION_LED_PINS[i], OUTPUT);
    digitalWrite(DIRECTION_LED_PINS[i], LOW);
  }
  pinMode(ZERO_MEASUREMENT_LED_PIN, OUTPUT);
  digitalWrite(ZERO_MEASUREMENT_LED_PIN, LOW);

  pinMode(STUCK_LED_PIN, OUTPUT);
  digitalWrite(STUCK_LED_PIN, LOW);

  //setup the sensors
  for (int i=0; i<NUM_F_U_SENSORS; i++) {
    F_U_Sensors[i].init(F_ECHO_PINS[i], F_TRIG_PINS[i], F_ID_CHARS[i]);
  }
  U_previous_time_ms = millis();


  ///PIR motion sensor
  pinMode(PIR_SIGNAL_PIN, INPUT);
  pinMode(HUMANS_LED_PIN, OUTPUT);


  ///Miscenialous stuff
  //so prev_distances_mm has something in it
  getDistances();
  copyArray(curr_distances_mm, prev_distances_mm, NUM_F_U_SENSORS);
  
  Serial.begin(115200);
  Serial.print("F_STUCK_DISTANCES_MM: ");
  Serial.println(strArray(F_STUCK_DISTANCES_MM, NUM_F_U_SENSORS, false));

  delay(1000);

}


void getDistances() {
  for (int i=0; i<NUM_F_U_SENSORS; i++) {
      curr_distances_mm[i] = F_U_Sensors[i].getDistanceMM(PULSEIN_TIMEOUT_US);
  }
}

bool isStuck() {
  
  for (uint8_t i=0; i<NUM_F_U_SENSORS; i++) {
    /*
    if ((curr_distances_mm[i] == 0) || (curr_distances_mm[i] > IGNORE_DISTANCE_THRESHOLD_MM)) {
      //false, can safely ignore stuff
    }
    */
    /*
    //sensor_stuck_states_array[i] = ( withinTolerance(curr_distances_mm[i], prev_distances_mm[i], STUCK_TOLERANCE_MM) && (curr_distances_mm[i] <= F_STUCK_DISTANCES_MM[i]) );
    bool within_tolerance = withinTolerance(curr_distances_mm[i], prev_distances_mm[i], STUCK_TOLERANCE_MM);
    bool within_stuck_dist = (curr_distances_mm[i] <= F_STUCK_DISTANCES_MM[i]);
    ////DID REALLY DO ANYTHING?????????????????????????????????????????????????????????????????????????????????????????????????????
    bool inside_ignore_dist = (curr_distances_mm[i] <= IGNORE_DISTANCE_THRESHOLD_MM); 
    front_sensor_stuck_states[i] = (curr_distances_mm[i] != 0) && (within_stuck_dist || (within_tolerance && inside_ignore_dist) ); //
    */


    bool is_distance_NOT_too_far = (curr_distances_mm[i] != 0);
    bool is_within_tolerance = withinTolerance(curr_distances_mm[i], prev_distances_mm[i], STUCK_TOLERANCE_MM);
    bool is_within_ignore_dist = (curr_distances_mm[i] <= IGNORE_DISTANCE_THRESHOLD_MM);
    bool is_within_stuck_dist = (curr_distances_mm[i] <= F_STUCK_DISTANCES_MM[i]);
    
    bool is_object_blocking = (is_within_stuck_dist && is_within_tolerance);

    bool is_stuck = (is_distance_NOT_too_far && is_object_blocking);
        
    front_sensor_stuck_states[i] = is_stuck;
    Serial.print(is_distance_NOT_too_far);
    Serial.print(is_object_blocking);
  }
  Serial.print(" stuck bools: " + strArray(front_sensor_stuck_states, NUM_F_U_SENSORS, false));


  //return isAllBool(sensor_stuck_states_array, true, NUM_F_U_SENSORS);
  return (numOccurances(front_sensor_stuck_states, true, NUM_F_U_SENSORS) >= (NUM_F_U_SENSORS-1)); // eg, at least (NUM_F_U_SENSORS-1) sensors have to be stuck
}


void determineDirection() {
  char max_dist_char_id;
  
  if (getMin(curr_distances_mm, NUM_F_U_SENSORS) == 0) { //the sensor which returns zero, actually has the largest distance, so grab that char id
    max_dist_char_id = F_ID_CHARS[minIndex(curr_distances_mm, NUM_F_U_SENSORS)]; 
    digitalWrite(ZERO_MEASUREMENT_LED_PIN, HIGH);
    Serial.print(" 0 return measurement ");

  } else {
    max_dist_char_id = F_ID_CHARS[maxIndex(curr_distances_mm, NUM_F_U_SENSORS)]; //find which sensor has the largest distance
    digitalWrite(ZERO_MEASUREMENT_LED_PIN, LOW);
  }

  
  bool is_left_right_in_tolerance = withinTolerance(curr_distances_mm[0], curr_distances_mm[2], LR_DIST_TOLERANCE_MM); 

  //Serial.print("is_left_right_in_tolerance: " + String(is_left_right_in_tolerance));
  //Serial.print(" maxChar: " + String(max_dist_char_id));

  if ((max_dist_char_id != 'F') && (!is_left_right_in_tolerance)) { //left/right has max distance... only change direction if there is significant difference between
    curr_turn_direction = max_dist_char_id;
  
  } else if (max_dist_char_id == 'F') { //front has maximum distance...
    curr_turn_direction = max_dist_char_id; 
  
  } else {
    //just keep going in same direction (which will be left/right)
  }
}


void evadeLoop() {
  U_current_time_ms = millis();

  if ((U_current_time_ms - U_previous_time_ms) >= TIME_BETWEEN_U_SENSORS_MS) {
    ////INPUTS
    //get distances
    getDistances();


    ////CALCULATIONS 
    ///speed control
    if (containsValueWithinRange(curr_distances_mm, 0, BRAKE_DISTANCE_THRESHOLD_MM, NUM_F_U_SENSORS)) { // make sure a zero measurement doesn't trigger it...
      /*
      if (!has_braked) { //this tries to prevent continual braking when not needed
        curr_drive_direction = 'B'; 
        has_braked = true;
        digitalWrite(VEHICLE_SLOW_SPEED_LED_PIN, HIGH);
        Serial.print(" BRAKING!!!");
      }*/
      
    } else if (containsValueWithinRange(curr_distances_mm, 0, SLOW_DOWN_DISTANCE_THRESHOLD_MM, NUM_F_U_SENSORS)) { // make sure a zero measurement doesn't trigger it...
      //vehicle_speed = SLOW_SPEED;
      digitalWrite(VEHICLE_SLOW_SPEED_LED_PIN, HIGH);
      //Serial.print(" slow");
      
    } else {
      vehicle_speed = NORMAL_SPEED;
      digitalWrite(VEHICLE_SLOW_SPEED_LED_PIN, LOW);
      //Serial.print(" normal");
    }

    ///stuck logic
    is_vehicle_stuck  = isStuck();
    if (is_vehicle_stuck) {
      //don't waste power on needlessly on motors
      curr_drive_direction = 'S'; 
      curr_turn_direction = 'C';
      digitalWrite(STUCK_LED_PIN, HIGH);

      Serial.print("...vehicle stuck...");
      
    } else {
      curr_drive_direction = 'F';
      has_braked = false; 
      digitalWrite(VEHICLE_SLOW_SPEED_LED_PIN, LOW);
      determineDirection();
      digitalWrite(STUCK_LED_PIN, LOW);
    }


    


    ////OUTPUTS
    //LED's
    if (is_vehicle_stuck) {
      for (int i=0; i<NUM_F_U_SENSORS; i++) {
        digitalWrite(DIRECTION_LED_PINS[i], front_sensor_stuck_states[i]); //turn on the LEDs which correspond to the stuck ultrasonics
      }
      
    } else {
      int8_t pin_index = index(F_ID_CHARS, curr_turn_direction, NUM_F_U_SENSORS);
      for (int i=0; i<NUM_F_U_SENSORS; i++) {
        bool state = (i==pin_index);
        digitalWrite(DIRECTION_LED_PINS[i], state); //turn on the relevant direction LED, and off the others
      }
    }
 


    //make it go! (vroom vroom)    
    RC_Car.turn(curr_turn_direction, true);
    RC_Car.drive(curr_drive_direction, vehicle_speed);
    
    //print results
    Serial.print(" | curr/prev distances: " + strArray(curr_distances_mm, NUM_F_U_SENSORS, false));
    Serial.print(", " + strArray(prev_distances_mm, NUM_F_U_SENSORS, false));
    Serial.print(", turn: " + String(curr_turn_direction));
    Serial.print(", drive: " + String(curr_drive_direction));
    Serial.print(", speed: " + String(vehicle_speed));

    Serial.print("\n");

    U_previous_time_ms = U_current_time_ms; //boomark
    copyArray(curr_distances_mm, prev_distances_mm, NUM_F_U_SENSORS);
  }
}

void checkPIR() {
  bool state = digitalRead(PIR_SIGNAL_PIN);
  Serial.println(" PIR: " + String(state));
  if (state == true) { //if motion detected...
    //if motion is detected...
    digitalWrite(HUMANS_LED_PIN, HIGH);

    if (is_human_present == false) { //if the motion hasn't been detected before...
      Serial.println("New Motion detected!");
      is_human_present = true;
    }
    
  } else { //if no motion detected...
    digitalWrite(HUMANS_LED_PIN, LOW);

    if (is_human_present) { //if there was previously motion...
      Serial.println("Motion ended");
      is_human_present = false;
    }
  }
}

void loop() {
  
  checkPIR();
  if (is_human_present) {
    digitalWrite(HUMANS_LED_PIN, HIGH);
    evadeLoop();  
  } else {
    digitalWrite(HUMANS_LED_PIN, LOW);
    RC_Car.drive('S', vehicle_speed);
  }
  
}
