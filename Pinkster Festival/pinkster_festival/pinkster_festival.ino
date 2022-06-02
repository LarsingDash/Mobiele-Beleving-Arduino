#include <Wire.h>

#include <LiquidCrystal_I2C.h>

const int red = 13;
const int green = 12;
const int blue = 14;
const int rButton = 32;
const int gButton = 35;
const int bButton = 34;
int score = 0;
int timer = 0;
int rNumber;
long currentTime;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  
  pinMode(rButton, INPUT_PULLUP);
  pinMode(gButton, INPUT_PULLUP);
  pinMode(bButton, INPUT_PULLUP);

  Wire.begin(25,26);

  lcd.init();
  lcd.backlight();
}

void loop() {
  //Initialize display
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

  //Play game for 30 seconds
  timer = millis();
  while(millis() < timer + 30000){
    play();
  }

  //Write congratulations
  lcd.setCursor(0, 1);
  lcd.print("Congratulations!");

  //Wait for clickthrough
  while(digitalRead(rButton) == LOW || digitalRead(gButton) == LOW || digitalRead(bButton) == LOW){
  }

  //Reset score
  score = 0;
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
  currentTime = millis();
  while(millis() < currentTime + 5000){
    //If light is red and red button is pressed
    if (rNumber == 0 && digitalRead(rButton) == HIGH){
      score++;
      break;
    }
    //If light is green and green button is pressed
    if (rNumber == 1 && digitalRead(gButton) == HIGH){
      score++;
      break;
    }
    //If light is blue and blue button is pressed
    if (rNumber == 2 && digitalRead(bButton) == HIGH){
      score++;
      break;
    }
  }

  //Reset led
  digitalWrite(red, LOW);
  digitalWrite(green, LOW);
  digitalWrite(blue, LOW);

  updateLCD();

  delay(250);
}

void updateLCD(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Score:");
  lcd.setCursor(7, 0);
  lcd.print(score);
}
