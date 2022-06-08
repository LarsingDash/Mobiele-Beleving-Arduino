#include <ESP32Servo.h>

#include <afstandssensor.h>

boolean servoUp;

//Initialize afstandssensor
AfstandsSensor sensor(25, 26);
Servo servo;

void setup() {
  servo.attach(32, 500, 2400);
  servoUp = false;
  Serial.begin(115200);
}

void loop() {
  //Randomly activate (1 in 100 every 100 ms = once every 10 seconds)
  if (random(0, 100) == 0) {
    //Servo up
    servo.write(0);
    servoUp = true;
    Serial.println("Servo up");

    //Wait for distance to get between 0 and 10, check every 100 ms
    while(servoUp){
      if(sensor.afstandCM() > 0 && sensor.afstandCM() < 10){
        servo.write(100);
        servoUp = false;
        Serial.println("Servo down");
      }

      Serial.println(sensor.afstandCM());
      delay(100);
    }
  }

  Serial.println(sensor.afstandCM());
  delay(100);
}
