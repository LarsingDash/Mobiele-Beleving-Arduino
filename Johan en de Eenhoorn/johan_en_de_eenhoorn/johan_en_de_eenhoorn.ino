#include <Wire.h>

#include <LiquidCrystal_I2C.h>

const int atkButton = 34;
const int defButton = 35;
const int frameTime = 500;
const int playTime = 30000;
const int spawnChance = 2;
int score = 0;
long timer;

int atkPos = random(11, 16);
int defPos = random(0, 5);

//byte attack[] = {0b00100,0b00100,0b00100,0b00100,0b00100,0b10101,0b01110,0b00100};
//byte defend[] = {0b11111,0b11111,0b11111,0b11111,0b11111,0b01110,0b01110,0b00100};
//byte line[] = {0b00100,0b00100,0b00100,0b00100,0b00100,0b00100,0b00100,0b00100};
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  pinMode(atkButton, INPUT_PULLUP);
  pinMode(defButton, INPUT_PULLUP);

//  lcd.createChar(0, attack);
//  lcd.createChar(1, defend);
//  lcd.createChar(2, line);

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
  while(digitalRead(atkButton) == LOW || digitalRead(defButton) == LOW){
  }

  countDown();

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
  while(digitalRead(atkButton) == LOW || digitalRead(defButton) == LOW){
  }

  addPoints();

  //Wait for clickthrough
  while(digitalRead(atkButton) == LOW || digitalRead(defButton) == LOW){
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
  //Analyze attack position
  switch (atkPos) {
    case 8:
      //If at line (x = 8), determine whether corresponding button is pressed
      if (digitalRead(atkButton) == HIGH){
        score++;
      }

      //Set attack position to storage value to despawn character from LCD
      atkPos = 100;
      break;
    case 100:
      //If attack position is in storage, roll for chance to spawn
      if (random(0, spawnChance) == 0){
        atkPos = 15;
      }
      break;
    default:
      //If none of the above (meaning attack position is 15, 14, 13, 12, 10 or 9), decrease position
      if (digitalRead(atkButton) == HIGH){
        score--;
      }
      atkPos--;
      break;
  }

  //Analyze defend position
  switch (defPos) {
    case 6:
      //If at line (x = 6), determine whether corresponding button is pressed
      if (digitalRead(defButton) == HIGH){
        score++;
      } else {
        score--;
      }

      //Set defend position to storage value to despawn character from LCD
      defPos = 100;
      break;
    case 100:
    //If defend position is in storage, roll for chance to spawn
      if (random(0, spawnChance) == 0){
        defPos = 0;
      }
      break;
    default:
      //If none of the above (meaning defend position is 0, 1, 2, 3, 4 or 5), increase position
      if (digitalRead(defButton) == HIGH){
        score--;
      }
      defPos++;
      break;
  }

  //Score can't go into negative
  if (score < 1){
    score = 0;
  }
  
  //Draw on LCD
  updateLCD();
}

void updateLCD(){
  lcd.clear();

  //Draw game elements
  lcd.setCursor(6, 0);
  lcd.print("V");
  lcd.setCursor(8, 0);
  lcd.print("V");
  lcd.setCursor(15, 0);
  lcd.print(score);
  lcd.setCursor(7, 1);
  lcd.print("I");
  
  //Draw attack and/or defense on current position
  lcd.setCursor(defPos, 1);
  lcd.print("D");
  lcd.setCursor(atkPos, 1);
  lcd.print("A");
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
