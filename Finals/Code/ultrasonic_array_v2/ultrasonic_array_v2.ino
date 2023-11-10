
#define SPEED_OF_SOUND_MM_PER_S 340000

struct uint16_tuple {
   uint16_t x;
   uint16_t y;
};

struct uint16_truple_tuple {
   uint16_tuple tupx;
   uint16_tuple tupy;
   uint16_tuple tupz;
};


class Ultrasonic { /////potentially get rid of the timeout_us and replace with #define to save 4bytes
  private:
    uint8_t _echo;
    uint8_t _trigger;
  public:
    char id_char;
    const uint8_t TRIGGER_LENGTH_US = 10;
    //const uint32_t SPEED_OF_SOUND_MM_PER_S = 340000;
    uint16_t prev_distance_mm;
    uint16_t curr_distance_mm;
    
    Ultrasonic(char char_name);
    void attach(uint8_t echo_pin, uint8_t trigger_pin);
    void sendPulse();
    uint16_tuple getDistanceMM(unsigned long timeout_us);
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
  delayMicroseconds(TRIGGER_LENGTH_US);
  digitalWrite(_trigger, LOW);
}

uint16_tuple Ultrasonic::getDistanceMM(unsigned long timeout_us) {
  sendPulse();
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


void printTuple(uint16_tuple tuple, bool new_line=true) {
  char end_char = (new_line) ? '\n':' ';
  Serial.print("(" + String(tuple.x) + "," + String(tuple.y) + ")" + end_char);
}

uint16_truple_tuple sweep() {
  uint16_truple_tuple distances_mm;

  distances_mm.tupx = LeftSensor.getDistanceMM(PULSEIN_TIMEOUT_US);
  distances_mm.tupy = FrontSensor.getDistanceMM(PULSEIN_TIMEOUT_US);
  distances_mm.tupz = RightSensor.getDistanceMM(PULSEIN_TIMEOUT_US);

  return distances_mm;
  
}

void user() {
  if (Serial.available() > 0) {
    char input = toupper(Serial.read());
    Serial.println("You inputted: " + String(input));

    uint16_tuple result;
    switch (input) {
      case 'F':
      Serial.println("1. Previous distance: " + String(FrontSensor.prev_distance_mm));
        result = FrontSensor.getDistanceMM(PULSEIN_TIMEOUT_US);
        Serial.println("2. Previous distance: " + String(FrontSensor.prev_distance_mm));
        break;
      case 'L':
        result = LeftSensor.getDistanceMM(PULSEIN_TIMEOUT_US);
        break;
      case 'R':
        result = RightSensor.getDistanceMM(PULSEIN_TIMEOUT_US);
        break;
      case 'S':
        uint16_truple_tuple distances_and_times = sweep();
        uint16_tuple left = distances_and_times.tupx;
        uint16_tuple front = distances_and_times.tupy;
        uint16_tuple right = distances_and_times.tupz;
        break;
      default:
        Serial.println("unrecognised sensor selection char");
        break;
    }

    printTuple(result);

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
