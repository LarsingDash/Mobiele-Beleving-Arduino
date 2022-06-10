#include <ESP32Servo.h>

#include <afstandssensor.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Hardware pins
const int button = 34;

//Setup variables
const int playTime = 30000;
const int upChance = 25;

//Loop variables
int score = 0;
boolean lServoUp;
boolean rServoUp;
long timer;

//Initialize afstandssensor
AfstandsSensor lSensor(12, 13);
AfstandsSensor rSensor(27, 14);
Servo lServo;
Servo rServo;

//LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  //Initialize hardware
  pinMode(button, INPUT_PULLUP);
  
  //Initialize servo
  lServo.attach(32, 500, 2400);
  rServo.attach(33, 500, 2400);
  lServoUp = false;
  rServoUp = false;

  //Initialize LCD
  Wire.begin(25,26);
  lcd.init();
  lcd.backlight();
  
  Serial.begin(115200);
}

void updateLCD(){
  lcd.clear();

  Serial.println(score);
 
  //Draw game elements
  lcd.setCursor(0, 0);
  lcd.print("Score:");
  lcd.setCursor(7, 0);
  lcd.print(score);
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

void play(){
  if(lServoUp){
    Serial.println(lSensor.afstandCM());
    if(lSensor.afstandCM() > 0 && lSensor.afstandCM() < 10){
      lServo.write(100);
      lServoUp = false;
      score++;
    }
  }else{
    if(lSensor.afstandCM() < 0 || lSensor.afstandCM() > 10){
      if(random(0, upChance) == 0){
        lServo.write(0);
        lServoUp = true;
        Serial.println("Left servo up");
      }
    }
  }

  if(rServoUp){
    Serial.println(rSensor.afstandCM());
    if(rSensor.afstandCM() > 0 && rSensor.afstandCM() < 10){
      rServo.write(100);
      rServoUp = false;
      score++;
    }
  }else{
    if(rSensor.afstandCM() < 0 || rSensor.afstandCM() > 10){
      if(random(0, upChance) == 0){
        rServo.write(0);
        rServoUp = true;
        Serial.println("Right servo up");
      }
    }
  }

  updateLCD();
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

  Serial.println("Waiting for activation");
  while(digitalRead(button) == LOW){
  }

  countDown();
  updateLCD();

  Serial.println("Now playing");
  timer = millis();
  while(millis() < timer + playTime){
    play();
    delay(100);
  }

  lServo.write(100);
  rServo.write(100);
  lServoUp = false;
  rServoUp = false;

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
