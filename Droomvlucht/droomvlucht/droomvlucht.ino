#include <ESP32Servo.h>

#include <afstandssensor.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Hardware pins
const int button = 34;

//Setup variables
const int playTime = 30000;
const int upChance = 50;

//Loop variables
int score = 0;
boolean servoUp;
long timer;

//Initialize afstandssensor
AfstandsSensor sensor(12, 13);
Servo servo;

//LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  //Initialize hardware
  pinMode(button, INPUT_PULLUP);
  
  //Initialize servo
  servo.attach(32, 500, 2400);
  servoUp = false;

  //Initialize LCD
  Wire.begin(25,26);
  lcd.init();
  lcd.backlight();
  
  Serial.begin(115200);
}

void loop(){
  //Reset display
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan phone");
  lcd.setCursor(0, 1);
  lcd.print("to start game");

  //Buffer
  delay(2000);

  while(digitalRead(button) == LOW){
  }

  countDown();
  updateLCD();

  timer = millis();
  while(millis() < timer + playTime){
    play();
    delay(100);
  }

  //Ask to add points
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Score:");
  lcd.setCursor(7, 0);
  lcd.print(score);
  lcd.setCursor(0, 1);
  lcd.print("Add points?");

  //Wait for clickthrough
  while(digitalRead(button) == LOW){
  }

  addPoints();

  //Wait for clickthrough
  while(digitalRead(button) == LOW){
  }

  //Reset score;
  score = 0;
}

void play() {
  //Randomly activate (1 in 50 every 100 ms = once every 5 seconds)
  if (sensor.afstandCM() > 10 || sensor.afstandCM() < 1){
    if (random(0, upChance) == 0) {
    //Servo up
    servo.write(0);
    servoUp = true;

    //Wait for distance to get between 0 and 10, check every 100 ms
      while(servoUp){
        if(sensor.afstandCM() > 0 && sensor.afstandCM() < 10){
        servo.write(100);
        servoUp = false;
        score++;
        }
        
      delay(100);
      }
    }
  }
}

void countDown(){
  //Countdown
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Starting game");
  lcd.setCursor(0, 1);
  lcd.print("in:");

  for (int i = 3; i > 0; i--){
    lcd.setCursor(4, 1);
    lcd.print(i);
    delay(1000);
  }
}

void updateLCD(){
  lcd.clear();

  //Draw game elements
  lcd.setCursor(0, 0);
  lcd.print("Score:");
  lcd.setCursor(7, 0);
  lcd.print(score);
}

void addPoints(){
  //Write everything on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Score:");
  lcd.setCursor(7, 0);
  lcd.print(score);
  lcd.setCursor(0, 1);
  lcd.print("Total:");
  lcd.setCursor(7, 1);
  lcd.print("50");

  //Buffer
  delay(1000);

  //Subtract from score and add to total
  for(int i = 1; i < score + 1; i++){
    lcd.setCursor(7, 0);
    lcd.print(score - i);
    lcd.setCursor(7, 1);
    lcd.print(50 + i);
    delay(100);
  }
}
