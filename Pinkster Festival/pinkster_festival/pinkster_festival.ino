#include <Wire.h>

#include <LiquidCrystal_I2C.h>

//Hardware pins
const int red = 13;
const int green = 12;
const int blue = 14;
const int rButton = 32;
const int gButton = 35;
const int bButton = 34;

//Setup variables
const int frameTime = 250;
const int playTime = 15000;
const int pressWindow = 5000;

//Loop variables
int score = 0;
long timer;
long timer2;
int rNumber;

//LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  //Initialize hardware
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  
  pinMode(rButton, INPUT_PULLUP);
  pinMode(gButton, INPUT_PULLUP);
  pinMode(bButton, INPUT_PULLUP);

  //Initialize LCD
  Wire.begin(25,26);
  lcd.init();
  lcd.backlight();
}

void loop() {
  //Reset display
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan phone");
  lcd.setCursor(0, 1);
  lcd.print("to start game");

  //Buffer
  delay(2000);

  //Wait for activation
  while(digitalRead(rButton) == LOW || digitalRead(gButton) == LOW || digitalRead(bButton) == LOW){
  }

  countDown();
  updateLCD();

  //Play game for however long playTime is set to
  timer = millis();
  while(millis() < timer + playTime){
    play();
    delay(frameTime);
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
  while(digitalRead(rButton) == LOW || digitalRead(gButton) == LOW || digitalRead(bButton) == LOW){
  }

  addPoints();

  //Wait for clickthrough
  while(digitalRead(rButton) == LOW || digitalRead(gButton) == LOW || digitalRead(bButton) == LOW){
  }

  //Reset score
  score = 0;
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

void play(){
  //Pick random color, display this
  rNumber = random(3);
  switch (rNumber){
    case 0:
      digitalWrite(red, HIGH);
      break;
    case 1:
      digitalWrite(green, HIGH);
      break;
    case 2:
      digitalWrite(blue, HIGH);
      break;
  }

  //Active pause
  timer2 = millis();
  while(millis() < timer2 + pressWindow){
    //If light is red and red button is pressed
    if (digitalRead(rButton) == HIGH){
      if (rNumber == 0){
        score++;
      }
      break;
    }
    //If light is green and green button is pressed
    if (digitalRead(gButton) == HIGH){
      if (rNumber == 1){
        score++;
      }
      break;
    }
    //If light is blue and blue button is pressed
    if (digitalRead(bButton) == HIGH){
      if (rNumber == 2){
        score++;
      }
      break;
    }
  }

  //Reset led
  digitalWrite(red, LOW);
  digitalWrite(green, LOW);
  digitalWrite(blue, LOW);

  //Draw on LCD
  updateLCD();
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
