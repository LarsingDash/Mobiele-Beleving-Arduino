#include <ESP32Servo.h>

#include <afstandssensor.h>


AfstandsSensor sensor(13, 12);
Servo servo;

void setup() {
  Serial.begin(115200);
  servo.attach(32, 500, 2400);
}

void loop() {
  if (random(0, 100) == 0) {
    servo.write(100);
    while(getDistance() > 10){
      delay(100);
      Serial.println(getDistance());
    }
    servo.write(0);
  }

  Serial.println(getDistance());
  
  delay(100);
}

double getDistance(){
  double distance = sensor.afstandCM();
  
  if (distance < 0){
    distance = 42;
  }

  return distance;
}
