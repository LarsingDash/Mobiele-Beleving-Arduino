#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <WiFi.h>
#include <PubSubClient.h>

//Hardware pins
const int atkButton = 34;
const int defButton = 35;

//Setup variables
const int frameTime = 500;
const int playTime = 30000;
const int spawnChance = 2;

//Loop variables
int atkPos = random(11, 16);
int defPos = random(0, 5);
int score = 0;
long timer;

//Wifi
const char* WLAN_ssid = "MQTT Test Network";
const char* WLAN_access_key = "00177013";

//Client IP
const char* MQTT_client_id = "arduino_johan_en_de_eenhoorn";

//MQTT broker
const char* MQTT_broker_url = "broker.hivemq.com";
const int   MQTT_port       = 1883;
const char* MQTT_username   = "";
const char* MQTT_password   = "";

//MQTT topics
const char* MQTT_topic_games_isAvailable = "esstelstrijd/games/jede/isAvailable";
const char* MQTT_topic_games_currentUser = "esstelstrijd/games/jede/currentUser";

//MQTT quality of service
const int MQTT_qos = 0;

//Create Wifi and MQTT variables
WiFiClient wifi;
PubSubClient mqtt(wifi);

//LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);
byte line[] = {B00100,B00100,B00100,B00100,B00100,B00100,B00100,B00100};
byte arrow[] = {B00100,B00100,B00100,B00100,B10101,B11111,B01110,B00100};
byte attack[] = {B00100,B00100,B00100,B00100,B00100,B00100,B01110,B00100};
byte defend[] = {B00000,B11111,B11111,B11111,B11111,B01110,B00100,B00000};

//Setup callback
void mqttCallback(char* topic, byte* payload, unsigned int length){
  //Logging
  Serial.println("--------------------------------");
  Serial.println("MQTT callback called for topic: ");
  Serial.println(topic);
  Serial.println("Message: ");
  for (int i = 0; i < length; i++){
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup() {
  //Initialize hardware
  pinMode(atkButton, INPUT_PULLUP);
  pinMode(defButton, INPUT_PULLUP);

  Serial.begin(115200);
  Serial.println("Test start");

  //Initialize LCD
  Wire.begin(25,26);
  lcd.init();
  lcd.createChar(0, line);
  lcd.createChar(1, arrow);
  lcd.createChar(2, attack);
  lcd.createChar(3, defend);
  lcd.backlight();

  //WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  Serial.println("Connecting to ");
  Serial.println(WLAN_ssid);
  WiFi.begin(WLAN_ssid, WLAN_access_key);
  
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(1000);
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //MQTT
  //Setup client
  mqtt.setServer(MQTT_broker_url, MQTT_port);
  mqtt.setCallback(mqttCallback);
  
  //Connect to broker
  if(mqtt.connect(MQTT_client_id, MQTT_username, MQTT_password)){
    Serial.println("Connected to MQTT broker");
  }else{
    Serial.println("Failed to connect to MQTT broker");
  }

  //Subscribe to topic
  if(mqtt.subscribe(MQTT_topic_games_isAvailable, MQTT_qos)){
    Serial.println("Subscribed to ");
    Serial.println(MQTT_topic_games_isAvailable);
  }else{
    Serial.println("Failed to subscribe to ");
    Serial.println(MQTT_topic_games_isAvailable);
  }

  if(mqtt.subscribe(MQTT_topic_games_currentUser, MQTT_qos)){
    Serial.println("Subscribed to ");
    Serial.println(MQTT_topic_games_currentUser);
  }else{
    Serial.println("Failed to subscribe to ");
    Serial.println(MQTT_topic_games_currentUser);
  }
}

void updateLCD(){
  lcd.clear();

  //Draw game elements
  lcd.setCursor(6, 0);
  lcd.write(1);
  lcd.setCursor(7, 0);
  lcd.write(0);
  lcd.setCursor(8, 0);
  lcd.write(1);
  lcd.setCursor(15, 0);
  lcd.print(score);
  lcd.setCursor(7, 1);
  lcd.write(0);
  
  //Draw attack and/or defense on current position
  lcd.setCursor(defPos, 1);
  lcd.write(3);
  lcd.setCursor(atkPos, 1);
  lcd.write(2);
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
    mqtt.loop();
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
