#include <Wire.h>

#include <LiquidCrystal_I2C.h>

const int red = 13;
const int green = 12;
const int blue = 14;
const int rButton = 32;
const int gButton = 35;
const int bButton = 34;
int score = 0;
long timer;
long timer2;
int rNumber;

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

  //Play game for 15 seconds
  timer = millis();
  while(millis() < timer + 15000){
    play();
    delay(250);
  }

  //Write congratulations
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
  while(millis() < timer2 + 5000){
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

  updateLCD();
}

void updateLCD(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Score:");
  lcd.setCursor(7, 0);
  lcd.print(score);
}

void addPoints(){
  updateLCD();
  lcd.setCursor(0, 1);
  lcd.print("Total:");
  lcd.setCursor(7, 1);
  lcd.print("50");

  for(int i = 1; i < score + 1; i++){
    lcd.setCursor(7, 0);
    lcd.print(score - i);
    lcd.setCursor(7, 1);
    lcd.print(50 + i);
    delay(100);
  }
}
