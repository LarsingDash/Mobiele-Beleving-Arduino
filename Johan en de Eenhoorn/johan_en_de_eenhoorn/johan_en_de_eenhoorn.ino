#include <Wire.h>

#include <LiquidCrystal_I2C.h>

const int atkButton = 12;
const int defButton = 13;
const int frameTime = 2000;
const int spawnChance = 2;
int score = 0;
long timer;

int atkPos = random(8, 16);
int defPos = random(0, 7);

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

  Serial.begin(115200);
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

  Serial.println("Waiting for activation");
  //Wait for activation
  while(digitalRead(atkButton) == LOW || digitalRead(defButton) == LOW){
  }

  Serial.println("Activated, now playing");
  //Play game for 30 seconds
  timer = millis();
  while(millis() < timer + 30000){
    play();
    delay(frameTime);
  }
}

void play(){
  if (atkPos == 8){
    if (digitalRead(atkButton == HIGH)){
      Serial.println("Succesfully attacked, added 1 point");
      score++;
    }
    atkPos = 100;
  } else if (atkPos == 100){
    if (random(0, spawnChance) == 0){
      atkPos = 15;
    } else {
      atkPos = 100;
    }
  } else {
    atkPos--;
  }

  
  if (defPos == 6){
    if (digitalRead(defButton == HIGH)){
      Serial.println("Succesfully defended, added 1 point");
      score++;
    } else {
      Serial.println("Unsuccesfully defend, subtracted 1 point");
      score--;
    }
    defPos = 100;
  } else if (defPos == 100){
    if (random(0, spawnChance) == 0){
      defPos = 0;
    } else{
      atkPos = 100;
    }
  } else {
    atkPos++;
  }
  
  updateLCD();
}

void updateLCD(){
  lcd.clear();

  lcd.setCursor(6, 0);
  lcd.print("V");
  lcd.setCursor(8, 0);
  lcd.print("V");
  lcd.setCursor(15, 0);
  lcd.print(score);
  lcd.setCursor(7, 1);
  lcd.print("I");
  
 
  lcd.setCursor(defPos, 1);
  lcd.print("D");

  lcd.setCursor(atkPos, 1);
  lcd.print("A");
}
