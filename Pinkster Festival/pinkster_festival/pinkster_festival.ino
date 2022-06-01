const int red = 21;
const int green = 22;
const int blue = 23;
const int rButton = 2;
const int gButton = 5;
const int bButton = 18;
int rNumber;
long currentTime;

void setup() {
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  
  pinMode(rButton, INPUT_PULLUP);
  pinMode(gButton, INPUT_PULLUP);
  pinMode(bButton, INPUT_PULLUP);

  Serial.begin(115200);
}

void loop() {
  digitalWrite(red, LOW);
  digitalWrite(green, LOW);
  digitalWrite(blue, LOW);

  delay(500);

  //Pick random color, display this
  rNumber = random(3);
  switch (rNumber){
    case 0:
      digitalWrite(red, HIGH);
      Serial.println("Picked red");
      break;
    case 1:
      digitalWrite(green, HIGH);
      Serial.println("Picked green");
      break;
    case 2:
      digitalWrite(blue, HIGH);
      Serial.println("Picked blue");
      break;
  }

  //Active pause
  currentTime = millis();
  while(millis() < currentTime + 5000){
    //If light is red and red button is pressed
    if (rNumber == 0 && digitalRead(rButton) == HIGH){
      Serial.println("Pressed red button");
      break;
    }
    //If light is green and green button is pressed
    if (rNumber == 1 && digitalRead(gButton) == HIGH){
      Serial.println("Pressed green button");
      break;
    }
    //If light is blue and blue button is pressed
    if (rNumber == 2 && digitalRead(bButton) == HIGH){
      Serial.println("Pressed blue button");
      break;
    }
  }
}
