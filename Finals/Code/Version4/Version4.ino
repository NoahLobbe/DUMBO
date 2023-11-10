/*
 * Uses two Shift registers for stuff
 */

////Libraries
#include "MotorSystemReg.h"
#include "Ultrasonic.h"
#include "DoubleRegisters.h"
#include "Functions.h"
#include "CustomDataTypes.h"




/**** General stuff ****/
//#define PRINT_RESULTS false




/**** Motor system ****/
///Arduino pins, PWM
#define EN_STEERING_SPEED_PIN 5 
#define EN_DRIVING_SPEED_PIN 6 

///shift register pins
#define STEERING_MOTOR_A_PIN 0
#define STEERING_MOTOR_B_PIN 1
#define DRIVING_MOTOR_A_PIN 2
#define DRIVING_MOTOR_B_PIN 3

///constants
#define PWM_SPEED_ENABLED true 
#define NORMAL_SPEED 200
#define SLOW_SPEED 128

///variables
uint8_t vehicle_speed = NORMAL_SPEED;
char curr_turn_direction = 'F'; 
char curr_drive_direction = 'F';
bool has_braked = false;
bool is_vehicle_stuck = false;

///objects
MotorSystemReg RC_Car;




/**** General Ultrasonic stuff ****/
///Constants
#define PULSEIN_TIMEOUT_US 100000 //the maximum time the pulseIn(...) function can take to wait for a signal. I think...
#define TIME_BETWEEN_SENSORS_MS 100 //Timing. Ideally, 60ms between triggering an ultrasonic prevents wave interference 

//Distance Thresholds
#define STUCK_DISTANCE_THRESHOLD_MM 100 
#define IGNORE_DISTANCE_THRESHOLD_MM 2000
#define SLOW_DOWN_DISTANCE_THRESHOLD_MM 1000

//Distance tolerances
#define STUCK_TOLERANCE_MM 10

///variables
//timing
unsigned long current_loop_time_ms;
unsigned long previous_loop_time_ms;




/**** Front Ultrasonic (FU) Array ****/
#define NUM_FU_SENSORS 3

///Arduino pins
const uint8_t FU_ECHO_PINS[NUM_FU_SENSORS] = {A4,A2,A0};
const uint8_t FU_TRIG_PINS[NUM_FU_SENSORS] = {A5,A3,A1};

///Constants
const char FU_ID_CHARS[NUM_FU_SENSORS] = {'L','F','R'};
#define FU_ACUTE_ANGLE_BETWEEN_SENSORS_RAD (36 * (PI/180)) //36degrees, converting
#define FU_LR_DIST_DIFFERENCE_MM round(22 * sin(FU_ACUTE_ANGLE_BETWEEN_SENSORS_RAD)) //calculating the 'height' difference between the front and side sensors, from centre of side sensor (~22mm)

//distance stuff
#define FU_LR_DIST_TOLERANCE_MM 100
const uint16_t FU_STUCK_DISTANCES_MM[NUM_FU_SENSORS] = {FU_LR_DIST_DIFFERENCE_MM + STUCK_DISTANCE_THRESHOLD_MM, 
                                                        STUCK_DISTANCE_THRESHOLD_MM, 
                                                        FU_LR_DIST_DIFFERENCE_MM + STUCK_DISTANCE_THRESHOLD_MM};

///variables
uint16_t FU_curr_distances_mm[NUM_FU_SENSORS];
uint16_t FU_prev_distances_mm[NUM_FU_SENSORS];
bool FU_stuck_states[NUM_FU_SENSORS];

///Objects
Ultrasonic FU_Sensors[NUM_FU_SENSORS];




/**** Shift Registers ****/
///Arduino pins
#define R_DATA_PIN 2
#define R_LATCH_PIN 3
#define R_CLOCK_PIN 4
#define R_RESET_PIN 7

///Objects
DoubleRegisters * registersPointer = &RC_Car.Registers;




/**** Indicator LEDs ****/
//shift register pins
const uint8_t DIRECTION_LED_PINS[NUM_FU_SENSORS] = {4, 5, 6};
#define ZERO_MEASUREMENT_LED_PIN 7
#define VEHICLE_SLOW_SPEED_LED_PIN 8
#define STUCK_LED_PIN 9




void setup() {
  ///motor system
  RC_Car.init(STEERING_MOTOR_A_PIN, STEERING_MOTOR_B_PIN,  DRIVING_MOTOR_A_PIN, DRIVING_MOTOR_B_PIN,
              R_DATA_PIN, R_LATCH_PIN, R_CLOCK_PIN, R_RESET_PIN);
  
  if (PWM_SPEED_ENABLED) {
    RC_Car.enablePWM(EN_STEERING_SPEED_PIN, EN_DRIVING_SPEED_PIN);
  } 
  
  ///Front ultrasonic arrayr
  for (int i=0; i<NUM_FU_SENSORS; i++) {
    FU_Sensors[i].init(FU_ECHO_PINS[i], FU_TRIG_PINS[i], FU_ID_CHARS[i]);
  }
  getDistances();
  copyArray(FU_curr_distances_mm, FU_prev_distances_mm, NUM_FU_SENSORS); //so prev_distances_mm has something in it
  

  ///LED indicators
  for (int i=0; i<NUM_FU_SENSORS; i++) {
    registersPointer->setPin(DIRECTION_LED_PINS[i], LOW);
  }
  registersPointer->setPin(ZERO_MEASUREMENT_LED_PIN, LOW);
  registersPointer->setPin(STUCK_LED_PIN, LOW);


  ///Miscenialous stuff
  previous_loop_time_ms = millis();
  Serial.begin(115200);


  ///Send out stuff
  registersPointer->writeOutputsMSB();
  Serial.print("FU_STUCK_DISTANCES_MM: ");
  Serial.println(strArray(FU_STUCK_DISTANCES_MM, NUM_FU_SENSORS, false));

  delay(1000);
}


void getDistances() {
  for (int i=0; i<NUM_FU_SENSORS; i++) {
      FU_curr_distances_mm[i] = FU_Sensors[i].getDistanceMM(PULSEIN_TIMEOUT_US);
  }
}

bool isStuck() {
  for (uint8_t i=0; i<NUM_FU_SENSORS; i++) {
    bool within_tolerance = withinTolerance(FU_curr_distances_mm[i], FU_prev_distances_mm[i], STUCK_TOLERANCE_MM);
    bool within_stuck_dist = (FU_curr_distances_mm[i] <= FU_STUCK_DISTANCES_MM[i]);
    FU_stuck_states[i] = (FU_curr_distances_mm[i] != 0) && within_stuck_dist && within_tolerance;
  }
  Serial.print(" stuck bools: " + strArray(FU_stuck_states, NUM_FU_SENSORS, false));

  return (numOccurances(FU_stuck_states, true, NUM_FU_SENSORS) > 0);
}


void determineDirection() {
  char max_dist_char_id;
  
  if (getMin(FU_curr_distances_mm, NUM_FU_SENSORS) == 0) { //the sensor which returns zero, actually has the largest distance, so grab that char id
    max_dist_char_id = FU_ID_CHARS[minIndex(FU_curr_distances_mm, NUM_FU_SENSORS)]; 
    registersPointer->setPin(ZERO_MEASUREMENT_LED_PIN, HIGH);
    Serial.print(" 0 return measurement ");

  } else {
    max_dist_char_id = FU_ID_CHARS[maxIndex(FU_curr_distances_mm, NUM_FU_SENSORS)]; //find which sensor has the largest distance
    registersPointer->setPin(ZERO_MEASUREMENT_LED_PIN, LOW);
  }

  bool is_left_right_in_tolerance = withinTolerance(FU_curr_distances_mm[0], FU_curr_distances_mm[2], FU_LR_DIST_TOLERANCE_MM); 

  if ((max_dist_char_id != 'F') && (!is_left_right_in_tolerance)) { //left or right has max distance... Previous versions also had the condition:  && (!is_left_right_in_tolerance), which is redudant
    curr_turn_direction = max_dist_char_id;
  
  } else if (max_dist_char_id == 'F') { //front has maximum distance...
    curr_turn_direction = max_dist_char_id; 
  
  } else {
    //just keep going in same direction (which will be left/right)
  }
}


void loop() {
  current_loop_time_ms = millis();

  if ((current_loop_time_ms - previous_loop_time_ms) >= TIME_BETWEEN_SENSORS_MS) {
    ////INPUTS
    getDistances();


    ////CALCULATIONS 
    ///speed control
    if (containsValueWithinRange(FU_curr_distances_mm, 0, SLOW_DOWN_DISTANCE_THRESHOLD_MM, NUM_FU_SENSORS)) { // make sure a zero measurement doesn't trigger it...
      vehicle_speed = SLOW_SPEED;
      registersPointer->setPin(VEHICLE_SLOW_SPEED_LED_PIN, HIGH);
      Serial.print(" slow");
      
    } else {
      vehicle_speed = NORMAL_SPEED;
      registersPointer->setPin(VEHICLE_SLOW_SPEED_LED_PIN, LOW);
      Serial.print(" normal");
    }

    ///stuck logic
    is_vehicle_stuck  = isStuck();
    if (is_vehicle_stuck) {
      //don't waste power on needlessly on motors
      curr_drive_direction = 'S'; 
      curr_turn_direction = 'C';
      has_braked = true;
      registersPointer->setPin(STUCK_LED_PIN, HIGH);

      Serial.print("...vehicle is stuck...");
      
    } else {
      curr_drive_direction = 'F';
      has_braked = false; 
      registersPointer->setPin(VEHICLE_SLOW_SPEED_LED_PIN, LOW);
      determineDirection();
      digitalWrite(STUCK_LED_PIN, LOW);
    }



    ////OUTPUTS
    ///LED's
    if (is_vehicle_stuck) {
      for (int i=0; i<NUM_FU_SENSORS; i++) {
        registersPointer->setPin(DIRECTION_LED_PINS[i], FU_stuck_states[i]); //turn on the LEDs which correspond to the stuck ultrasonics
      }
      
    } else {
      int8_t pin_index = index(FU_ID_CHARS, curr_turn_direction, NUM_FU_SENSORS);
      for (int i=0; i<NUM_FU_SENSORS; i++) {
        bool state = (i==pin_index);
        registersPointer->setPin(DIRECTION_LED_PINS[i], state); //turn on the relevant direction LED, and off the others
      }
    }
 


    ///Write outputs 
    RC_Car.turn(curr_turn_direction, 255);
    RC_Car.drive(curr_drive_direction, vehicle_speed);

    //same as (*registersPointer).writeOutputsMSB(); 
    //It means call the function ('writeOutputsMSB()') that belongs to the object ('RC_Car.Registers'), which is pointed to by 'registersPointer' 
    registersPointer->writeOutputsMSB(); 

    
    
    ///print results
    Serial.print(" | curr/prev distances: " + strArray(FU_curr_distances_mm, NUM_FU_SENSORS, false));
    Serial.print(", " + strArray(FU_prev_distances_mm, NUM_FU_SENSORS, false));
    Serial.print(" | turn dir: " + String(curr_turn_direction));
    Serial.print(" | drive dir: " + String(curr_drive_direction));
    Serial.print(" vehicle_speed: " + String(vehicle_speed));
    Serial.print("\n");

    ////bookmarking
    previous_loop_time_ms = current_loop_time_ms; 
    copyArray(FU_curr_distances_mm, FU_prev_distances_mm, NUM_FU_SENSORS);
  }

}
