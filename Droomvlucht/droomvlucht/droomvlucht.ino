#include <ESP32Servo.h>

#include <afstandssensor.h>

//Initialize afstandssensor
AfstandsSensor sensor(13, 12);
Servo servo;

void setup() {
  servo.attach(32, 500, 2400);
  Serial.begin(115200);
}

void loop() {
  //Randomly activate (1 in 100 every 100 ms = once every 10 seconds)
  if (random(0, 100) == 0) {
    //Servo up
    servo.write(100);

    //Wait for distance to get between 0 and 10, check every 100 ms
    while(!(getDistance() < 10 && getDistance() > 0)){
      delay(100);
      Serial.println(getDistance());
    }

    //Servo down
    servo.write(0);
  }

  Serial.println(getDistance());
  
  delay(100);
}

double getDistance(){
  //Retrieve distance
  double distance = sensor.afstandCM();

  //If distance is 0 or lower, set to 0
  if (distance < 1){
    distance = 0
  }

  return distance;
}
