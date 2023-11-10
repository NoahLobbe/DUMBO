/*
 * V4, 12 Oct, Noah
 */

#include "ultrasonic.h"
#include "custom_datatypes.h"




////timing stuff
#define TIME_BETWEEN_U_SENSORS_MS 100 //ideally, 60ms between triggering an ultrasonic prevents wave interference 
unsigned long U_current_time_ms;
unsigned long U_previous_time_ms;



////Ultrasonics
//general constants
#define PULSEIN_TIMEOUT_US 100000//the maximum time the pulseIn(...) function can take to wait for a signal. I think.


//Front set of Ultrasonic Sensors
#define NUM_F_U_SENSORS 3
const uint8_t F_ECHO_PINS[NUM_F_U_SENSORS] = {6,4,2};
const uint8_t F_TRIG_PINS[NUM_F_U_SENSORS] = {7,5,3};
const char F_ID_CHARS[NUM_F_U_SENSORS] = {'L','F','R'};
Ultrasonic F_U_Sensors[NUM_F_U_SENSORS];






//variables
char curr_direction = 'F'; // Forward, Leftward, and Rightward

void setup() {
  U_previous_time_ms = millis();
  /*
  RightSensor.attach(2,3); //echo, trigger
  FrontSensor.attach(4,5);
  LeftSensor.attach(6,7);
  */
  for (int i=0; i<NUM_F_U_SENSORS; i++) {
    F_U_Sensors[i].init(F_ECHO_PINS[i], F_TRIG_PINS[i], F_ID_CHARS[i]);
  }

  
  Serial.begin(9600);


}

int8_t index(char val, char *arr, uint8_t arr_len) { //find index of a char in an array
  for (int i=0; i<arr_len; i++) {
    if (val == arr[i]) {
      return i;
    }
  }
  return -1; //couldn't find it :(
}

void printArray(int *arr, uint8_t arr_len, bool new_line=true) {
  String str = "[";
  for (int i=0; i<arr_len; i++) {
    str += String(arr[i]) + ((i<arr_len-1) ? ",":"");
  }
  str += "]";
  str += (new_line) ? "\n" : "";
  Serial.print(str);
}

uint8_t maxIndex(uint16_t *arr, uint8_t arr_len) {
  uint8_t index = 0;
  uint16_t curr_max = arr[index];
  
  for (int i=0; i<arr_len; i++) {
    if (arr[i] > curr_max) {
      curr_max = arr[i];
      index = i;
    }
  }
  return index;
  
}




void loop() {
  U_current_time_ms = millis();

  if ((U_current_time_ms - U_previous_time_ms) >= TIME_BETWEEN_U_SENSORS_MS) {
    uint16_t distances_mm[NUM_F_U_SENSORS];

    for (int i=0; i<NUM_F_U_SENSORS; i++) { //get distances
      distances_mm[i] = F_U_Sensors[i].getDistanceMM(PULSEIN_TIMEOUT_US);
    }

    curr_direction = F_ID_CHARS[maxIndex(distances_mm, NUM_F_U_SENSORS)]; //update based on max distance



    //print results
    printArray(distances_mm, NUM_F_U_SENSORS, false);
    //Serial.print(" | time: " + String(U_current_time_ms));
    Serial.print(" | curr_direction: " + String(curr_direction));

    Serial.print("\n");

    U_previous_time_ms = U_current_time_ms;
  }
  

}
