
#define SPEED_OF_SOUND_MM_PER_S 340000

struct uint16_tuple {
   uint16_t x;
   uint16_t y;
};


class Ultrasonic { /////potentially get rid of the timeout_us and replace with #define to save 4bytes
  private:
    uint8_t _echo;
    uint8_t _trigger;
  public:
    char id_char;
    //const uint32_t SPEED_OF_SOUND_MM_PER_S = 340000;
    uint16_t prev_distance_mm;
    uint16_t curr_distance_mm;
    
    Ultrasonic(char char_name);
    void attach(uint8_t echo_pin, uint8_t trigger_pin);
    void sendPulse();
    uint16_tuple getDistance(unsigned long timeout_us);
    void rememberDistance();
};

Ultrasonic::Ultrasonic(char char_name){
  id_char = char_name;
}

void Ultrasonic::attach(uint8_t echo_pin, uint8_t trigger_pin) {
  _echo = echo_pin;
  _trigger = trigger_pin;
  pinMode(_echo, INPUT);
  pinMode(_trigger, OUTPUT); 
  digitalWrite(_trigger, LOW);
}

void Ultrasonic::sendPulse() {
  Serial.println(String(id_char) + " sensor is sending pulse");
  digitalWrite(_trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(_trigger, LOW);
}

uint16_tuple Ultrasonic::getDistance(unsigned long timeout_us) {

  uint16_t duration_uS = pulseIn(_echo, HIGH, timeout_us);
  curr_distance_mm = (SPEED_OF_SOUND_MM_PER_S * duration_uS * 1e-6) / 2;
  Serial.println(String(id_char) + " sensor duration_uS: " + String(duration_uS) + ", distance_mm: " + String(curr_distance_mm));

  uint16_tuple result = {curr_distance_mm, duration_uS};
  return result; 
}

void Ultrasonic::rememberDistance() {
  prev_distance_mm = curr_distance_mm; //bookmark
}


//constants

#define PULSEIN_TIMEOUT_US 100000//the maximum time the pulseIn(...) function can take to wait for a signal. I think.


//objects
Ultrasonic FrontSensor('F');
Ultrasonic LeftSensor('L');
Ultrasonic RightSensor('R');




//variables

void setup() {
  RightSensor.attach(2,3);
  FrontSensor.attach(4,5);
  LeftSensor.attach(6,7);

  
  Serial.begin(9600);

  //Serial.println("Speed of sound mm/second: " + String(FrontSensor.SPEED_OF_SOUND_MM_PER_S));
  Serial.println("Enter 'F', 'L', or 'R' to select sensor: ");
}


void user() {
  if (Serial.available() > 0) {
    char input = toupper(Serial.read());
    Serial.println("You inputted: " + String(input));

    uint16_tuple result;
    switch (input) {
      case 'F':
      Serial.println("1. Previous distance: " + String(FrontSensor.prev_distance_mm));
        FrontSensor.sendPulse();
        result = FrontSensor.getDistance(PULSEIN_TIMEOUT_US);
        Serial.println("2. Previous distance: " + String(FrontSensor.prev_distance_mm));
        break;
      case 'L':
        LeftSensor.sendPulse();
        result = LeftSensor.getDistance(PULSEIN_TIMEOUT_US);
        break;
      case 'R':
        RightSensor.sendPulse();
        result = RightSensor.getDistance(PULSEIN_TIMEOUT_US);
        break;
      default:
        Serial.println("unrecognised sensor selection char");
        break;
    }

    uint16_t current_distance_mm = result.x;
    uint16_t current_duration_us = result.y;
    Serial.println("Distance from '" + String(input) + "' sensor is: " + String(current_distance_mm) + "mm\n");

    FrontSensor.rememberDistance();
    LeftSensor.rememberDistance();
    RightSensor.rememberDistance();
      
  }
}


void loop() {
  user();
  

}
