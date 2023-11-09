//pin definitions
//Ultrasonic sensor
#define ECHO_PIN 2
#define TRIGGER_PIN 3
//#define STUCK_LED_PIN 12 //not implemented yet, except for start up indicators
#define OBJECT_DETECTED_LED_PIN 13

//variables
#define SPEED_OF_SOUND_MM_PER_S 340000
//Remember the formula is distance = 0.5 * speed * time; therefore time = 2 * (distance / speed) 
#define ONE_MICROSECOND_IS_X_MILLIMETRES (0.5 * SPEED_OF_SOUND_MM_PER_S * 1e-6)
#define TIMEOUT_DISTANCE_MM 1000
#define PULSEIN_TIMEOUT_US (TIMEOUT_DISTANCE_MM / ONE_MICROSECOND_IS_X_MILLIMETRES) 



void setup() {
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  digitalWrite(TRIGGER_PIN, LOW); //no accidental triggering
  //pinMode(STUCK_LED_PIN, OUTPUT);
  pinMode(OBJECT_DETECTED_LED_PIN, OUTPUT);

  Serial.begin(9600);
  Serial.println("PULSEIN_TIMEOUT_US: " + String(PULSEIN_TIMEOUT_US) + " | ONE_MICROSECOND_IS_X_MILLIMETRES: " + String(ONE_MICROSECOND_IS_X_MILLIMETRES));

  delay(1000);
}

void loop() {

  //send pulse
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10); //send the trigger pulse
  digitalWrite(TRIGGER_PIN, LOW);


  //Calculations
  signed long duration_uS = pulseInLong(ECHO_PIN, HIGH); //How many microseconds the ECHO pin pulse is HIGH. 
  int16_t distance_mm = 0.5 * SPEED_OF_SOUND_MM_PER_S * (duration_uS * 1e-6); //conversions done inline to prevent inaccuracies through rounding

  if (duration_uS == 0) {
    digitalWrite(OBJECT_DETECTED_LED_PIN, HIGH);
  } else {
    digitalWrite(OBJECT_DETECTED_LED_PIN, LOW);
  }

  Serial.println("\nuS: " + String(duration_uS) + " | distance (mm): " + String(distance_mm));
  delay(60);

}
