/*
 * The prototype code.
 */

class MotorSystem { //make a class for handling the RC car motors
  private:
    uint8_t _STEERING_MOTOR_A_PIN;
    uint8_t _STEERING_MOTOR_B_PIN;
    uint8_t _DRIVING_MOTOR_A_PIN;
    uint8_t _DRIVING_MOTOR_B_PIN;
    
  public:
    char driving_direction;
    char turning_direction;
    MotorSystem();
    void attach(uint8_t steering_motor_a, uint8_t steering_motor_b, uint8_t driving_motor_a, uint8_t driving_motor_b);
    void drive(char driving_direction_char);
    void turn(char turning_direction_char);
};

MotorSystem::MotorSystem() {
  
}
void MotorSystem::attach(uint8_t steering_motor_a, uint8_t steering_motor_b, uint8_t driving_motor_a, uint8_t driving_motor_b) {
  _STEERING_MOTOR_A_PIN = steering_motor_a;
  _STEERING_MOTOR_B_PIN = steering_motor_b;
  _DRIVING_MOTOR_A_PIN = driving_motor_a;
  _DRIVING_MOTOR_B_PIN = driving_motor_b;

  pinMode(_STEERING_MOTOR_A_PIN, OUTPUT);
  pinMode(_STEERING_MOTOR_B_PIN, OUTPUT);
  pinMode(_DRIVING_MOTOR_A_PIN, OUTPUT);
  pinMode(_DRIVING_MOTOR_B_PIN, OUTPUT);
}

//this method handles everything for driving
void MotorSystem::drive(char driving_direction_char) {
  if (driving_direction_char == 'F') {
    //forward
    digitalWrite(_DRIVING_MOTOR_A_PIN, HIGH);
    digitalWrite(_DRIVING_MOTOR_B_PIN, LOW);
    
  } else if (driving_direction_char == 'B') {
    //backward
    digitalWrite(_DRIVING_MOTOR_A_PIN, LOW);
    digitalWrite(_DRIVING_MOTOR_B_PIN, HIGH);
    
  } else {
    //stop - stop powering the drive motor
    digitalWrite(_DRIVING_MOTOR_A_PIN, LOW);
    digitalWrite(_DRIVING_MOTOR_B_PIN, LOW);
  }
  driving_direction = driving_direction_char; //update for future use
}

//this method handles everything for steering
void MotorSystem::turn(char turning_direction_char) {
  if (turning_direction_char == 'L') {
    //forward
    digitalWrite(_STEERING_MOTOR_A_PIN, HIGH);
    digitalWrite(_STEERING_MOTOR_B_PIN, LOW);
    
  } else if (turning_direction_char == 'R') {
    //backward
    digitalWrite(_STEERING_MOTOR_A_PIN, LOW);
    digitalWrite(_STEERING_MOTOR_B_PIN, HIGH);
    
  } else { // 'C'
    //centre - spring system should centre the steering
    digitalWrite(_STEERING_MOTOR_A_PIN, LOW);
    digitalWrite(_STEERING_MOTOR_B_PIN, LOW);
  }
  turning_direction = turning_direction_char; //update for future use
}





//pin definitions
#define PIR_PIN 3
#define ECHO_PIN 4
#define TRIGGER_PIN 5

#define STEERING_MOTOR_A_PIN 6
#define STEERING_MOTOR_B_PIN 7
#define DRIVING_MOTOR_A_PIN 8
#define DRIVING_MOTOR_B_PIN 9

//variables
#define SPEED_OF_SOUND_MM_PER_S (343 * 1000) //easier to make sure there is the right about of zeros :D

bool motion_has_been_detected = false;
//MotorSystem records these states already
//char driving_direction = 'S'; //F = forward, B = backward, S = stop
//char turning_direction = 'C'; // L = left, R = right, C = centre
#define object_avoidance_distance_threshold_mm 750
#define turning_distance_minimum_mm 500

//objects
MotorSystem RC_Car; //object to control the RC car motors

void setup() {
  pinMode(PIR_PIN, INPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  RC_Car.attach(STEERING_MOTOR_A_PIN, STEERING_MOTOR_B_PIN, DRIVING_MOTOR_A_PIN, DRIVING_MOTOR_B_PIN);

  digitalWrite(TRIGGER_PIN, LOW); //make sure the arduino is telling the ultrasonic to trigger
}

uint16_t getDistanceMM() {
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10); //send a 10uS trigger pulse
  digitalWrite(TRIGGER_PIN, LOW);

  uint16_t duration_uS = pulseIn(ECHO_PIN, HIGH); //Time (in microseconds, uS) how long the ECHO pin pulse is HIGH
  
  return (uint16_t)(0.5 * SPEED_OF_SOUND_MM_PER_S * duration_uS * 1e-6); //distance in mm. Conversions done inline to prevent inaccuracies through rounding
}

void updateMotionState(){
  if (digitalRead(PIR_PIN)) { //read the PIR motion sensor's state
    //if motion is detected...
    if (motion_has_been_detected == false) { //if motion hasn't been detected
      //Serial.println("New Motion detected!");
      motion_has_been_detected = true;
    }
    
  } else {
    //if motion not detected...
    if (motion_has_been_detected) {
      //Serial.println("Motion ended");
      motion_has_been_detected = false;
    }
  }
}

void loop() {
  ////inputs
  
  updateMotionState();
  uint16_t distance_mm = getDistanceMM(); //might need to take into account the vehicle's motion, like taking into account vehicle speed

  ////output
  if (motion_has_been_detected == true) {
    if (distance_mm <= object_avoidance_distance_threshold_mm) {
      RC_Car.turn('R'); //default turn right
      
    } else if (distance_mm <= turning_distance_minimum_mm) {
      RC_Car.drive('S'); //don't go any futher because turning won't avoid the object
     
    } else {
      if (RC_Car.turning_direction != 'C') {
        RC_Car.turn('C'); //return to centre if previously turning and don't need to anymore
      }
  
      if (RC_Car.driving_direction != 'F') {
        RC_Car.drive('F'); //drive forward because there is nothing in the way
      }
    }
  } //else just chill out by doing nothing :D

  delay(60); //to prevent ultrasonic waves interferring with themselves.
}
