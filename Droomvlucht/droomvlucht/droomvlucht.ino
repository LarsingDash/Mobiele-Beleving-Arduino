#include <ESP32Servo.h>
#include <afstandssensor.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <WiFi.h>
#include <PubSubClient.h>

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

//Wifi
const char* WLAN_ssid = "MQTT Test Network";
const char* WLAN_access_key = "00177013";

//Client IP
const char* MQTT_client_id = "arduino_droomvlucht";

//MQTT broker
const char* MQTT_broker_url = "broker.hivemq.com";
const int   MQTT_port       = 1883;
const char* MQTT_username   = "";
const char* MQTT_password   = "";

//MQTT topics
const char* MQTT_topic_games_isAvailable = "esstelstrijd/games/droom/isAvailable";
const char* MQTT_topic_games_currentUser = "esstelstrijd/games/droom/currentUser";

//MQTT quality of service
const int MQTT_qos = 0;

//Create Wifi and MQTT variables
WiFiClient wifi;
PubSubClient mqtt(wifi);

//Initialize afstandssensor
AfstandsSensor lSensor(12, 13);
AfstandsSensor rSensor(27, 14);
Servo lServo;
Servo rServo;

//LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

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
  pinMode(button, INPUT_PULLUP);
  
  //Initialize servo
  lServo.attach(32, 500, 2400);
  rServo.attach(33, 500, 2400);
  lServoUp = false;
  rServoUp = false;

  Serial.begin(115200);
  Serial.println("Test start");

  //Initialize LCD
  Wire.begin(25,26);
  lcd.init();
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
     mqtt.loop();
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
