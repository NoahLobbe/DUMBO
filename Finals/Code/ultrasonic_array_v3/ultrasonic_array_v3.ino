/*
 * V3, 12 Oct, Noah
 */

#include "ultrasonic.h"
#include "custom_datatypes.h"












//constants
#define TIME_BETWEEN_SENSORS_US (60 * 1000) //ideally, 60ms between triggering an ultrasonic prevents wave interference 
#define PULSEIN_TIMEOUT_US 100000//the maximum time the pulseIn(...) function can take to wait for a signal. I think.


//objects
Ultrasonic FrontSensor('F');
Ultrasonic LeftSensor('L');
Ultrasonic RightSensor('R');




//variables

void setup() {
  RightSensor.attach(2,3); //echo, trigger
  FrontSensor.attach(4,5);
  LeftSensor.attach(6,7);

  
  Serial.begin(9600);

  //Serial.println("Speed of sound mm/second: " + String(FrontSensor.SPEED_OF_SOUND_MM_PER_S));
  Serial.println("Enter 'F', 'L', or 'R' to select sensor: ");
}




uint16_triple sweep() {
  uint16_triple distances_mm;

  distances_mm.x = LeftSensor.getDistanceMM(PULSEIN_TIMEOUT_US);
  distances_mm.y = FrontSensor.getDistanceMM(PULSEIN_TIMEOUT_US);
  distances_mm.z = RightSensor.getDistanceMM(PULSEIN_TIMEOUT_US);

  return distances_mm;
}


void user() {
  if (Serial.available() > 0) {
    char input = toupper(Serial.read());
    Serial.println("You inputted: " + String(input));

    uint16_t result;
    switch (input) {
      case 'F':
        //Serial.println("1. Previous distance: " + String(FrontSensor.prev_distance_mm));
        result = FrontSensor.getDistanceMM(PULSEIN_TIMEOUT_US);
        //Serial.println("2. Previous distance: " + String(FrontSensor.prev_distance_mm));
        break;
      case 'L':
        result = LeftSensor.getDistanceMM(PULSEIN_TIMEOUT_US);
        break;
      case 'R':
        result = RightSensor.getDistanceMM(PULSEIN_TIMEOUT_US);
        break;
      case 'S':
        unsigned long start_time = millis();

        uint16_triple distances_mm = sweep();
        Serial.println("Sweeping:" + strTriple(distances_mm));
        
        unsigned long end_time = millis();
        unsigned long delta_time = end_time - start_time;
        Serial.println("Delta time is: " + String(delta_time));

        
        result = 0; //nothing
        //uint16_tuple left = distances_and_times.tupx;
        //uint16_tuple front = distances_and_times.tupy;
        //uint16_tuple right = distances_and_times.tupz;
        break;
      default:
        result = 0; //nothing
        Serial.println("unrecognised sensor selection char");
        break;
    }

    


    //Serial.println("Distance from '" + String(input) + "' sensor is: " + String(result) + "mm\n");

    FrontSensor.rememberDistance();
    LeftSensor.rememberDistance();
    RightSensor.rememberDistance();
      
  }
}



void loop() {
  user();
  

}
