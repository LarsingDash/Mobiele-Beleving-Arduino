#include <LiquidCrystal_I2C.h>

const int red = 5;
const int green = 19;
const int blue = 23;
const int rButton = 2;
const int gButton = 4;
const int bButton = 18;
int score = 0;
int rNumber;
long currentTime;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void updateLCD(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Score:");
  lcd.setCursor(7, 0);
  lcd.print(score);
  lcd.setCursor(9, 0);
  lcd.print("/15");
}

void setup() {
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  
  pinMode(rButton, INPUT_PULLUP);
  pinMode(gButton, INPUT_PULLUP);
  pinMode(bButton, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
}

void loop() {
  digitalWrite(red, LOW);
  digitalWrite(green, LOW);
  digitalWrite(blue, LOW);

  updateLCD();

  if (score == 15){
    lcd.setCursor(0, 1);
    lcd.print("Code: 12345");

    while(digitalRead(rButton) == LOW || digitalRead(gButton) == LOW || digitalRead(bButton) == LOW){
    }

    score = 0;
    updateLCD();
  }

  delay(500);

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
}
