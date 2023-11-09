/*
 * Quick and dirty motor driver test code
 * 
 * Version m.0.10.0
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
    Serial.println("...forward...");
    //forward
    digitalWrite(_DRIVING_MOTOR_A_PIN, HIGH);
    digitalWrite(_DRIVING_MOTOR_B_PIN, LOW);
    
  } else if (driving_direction_char == 'B') {
    Serial.println("...backward...");
    //backward
    digitalWrite(_DRIVING_MOTOR_A_PIN, LOW);
    digitalWrite(_DRIVING_MOTOR_B_PIN, HIGH);
    
  } else {
    Serial.println("...stop...");
    //stop - stop powering the drive motor
    digitalWrite(_DRIVING_MOTOR_A_PIN, LOW);
    digitalWrite(_DRIVING_MOTOR_B_PIN, LOW);
  }
  driving_direction = driving_direction_char; //update for future use
}

//this method handles everything for steering
void MotorSystem::turn(char turning_direction_char) {
  if (turning_direction_char == 'L') {
    Serial.println("...left...");
    //forward
    digitalWrite(_STEERING_MOTOR_A_PIN, HIGH);
    digitalWrite(_STEERING_MOTOR_B_PIN, LOW);
    
  } else if (turning_direction_char == 'R') {
    Serial.println("...right...");
    //backward
    digitalWrite(_STEERING_MOTOR_A_PIN, LOW);
    digitalWrite(_STEERING_MOTOR_B_PIN, HIGH);
    
  } else { // 'C'
    Serial.println("...centre...");
    //centre - spring system should centre the steering
    digitalWrite(_STEERING_MOTOR_A_PIN, LOW);
    digitalWrite(_STEERING_MOTOR_B_PIN, LOW);
  }
  turning_direction = turning_direction_char; //update for future use
}






//pin definitions
#define PWM_SPEED_ENABLED false //code not added yet

#define STEERING_SPEED_PIN 8
#define STEERING_MOTOR_A_PIN 9
#define STEERING_MOTOR_B_PIN 10

#define DRIVING_MOTOR_A_PIN 11
#define DRIVING_MOTOR_B_PIN 12
#define DRIVING_SPEED_PIN 13




//variables


//objects
MotorSystem RC_Car; //object to control the RC car motors


void setup() {
  pinMode(STEERING_SPEED_PIN, OUTPUT);
  pinMode(DRIVING_SPEED_PIN, OUTPUT);
  if (PWM_SPEED_ENABLED) {
    analogWrite(STEERING_SPEED_PIN, 0);
    analogWrite(DRIVING_SPEED_PIN, 0);
  } else {
    digitalWrite(STEERING_SPEED_PIN, HIGH); //default to max
    digitalWrite(DRIVING_SPEED_PIN, HIGH); //default to max
  }
  
  RC_Car.attach(STEERING_MOTOR_A_PIN, 
                STEERING_MOTOR_B_PIN, 
                DRIVING_MOTOR_A_PIN, 
                DRIVING_MOTOR_B_PIN);
  Serial.begin(9600);
  Serial.println("...Enter a two char command...");
  Serial.println("Drive motor \n\tMotor Select: 'D' \n\tcommands: F = forward, B = backward, S = stop"
                 "\nSteering motor \n\tMotor Select: 'S' \n\tcommands: L = left, R = right, C = centre");
}

void loop() {

  if (Serial.available() > 0) {
    String input = Serial.readString();
    char motor_select = input[0];
    char command = input[1];
    String text = "\nYou inputted..."
                  "\n\tmotor select: " + String(motor_select) +
                  "\n\tcommand: " + String(command);
    Serial.println(text);

    if (motor_select == 'D') {
      Serial.println("Command given to drive motor...");
      RC_Car.drive(command);
      
    } else if (motor_select == 'S') {
      Serial.println("Command given to steering motor...");
      RC_Car.turn(command);
      
    } else {
      Serial.println("...unrecognized motor_select...");
    }
    
  }
  

}
