#include <ESP32Servo.h>
#include <afstandssensor.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <WiFi.h>
#include <PubSubClient.h>

//Hardware pins

//Setup variables
const int frameTime = 100;
const int playTime = 30000;
const int upChance = 25;

//Loop variables
int score = 0;
boolean lServoUp;
boolean rServoUp;
long playTimer;

bool isAvailable;
int landPoints;
int userPoints;
String userLand;

//Wifi
const char* ssid = "MQTT Network";
const char* access_key = "00177013";

//Client IP
const char* client_id = "arduino_droomvlucht";

//MQTT broker
const char* broker_url = "broker.hivemq.com";
const int   port       = 1883;
const char* username   = "";
const char* password   = "";

//MQTT topics
const String games_droom_isAvailable   = "esstelstrijd/games/droom/isAvailable";
const String games_droom_currentUser   = "esstelstrijd/games/droom/currentUser";
String users_user                     = "esstelstrijd/users/";
String lands_land                     = "esstelstrijd/lands/";
const String points_END               = "/points";
const String users_user_land_END      = "/land";

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

//Callback
void callback(char* topic, byte* payload, unsigned int length){
  //Logging
  Serial.println("-------------------------------");
  Serial.print("MQTT callback called for topic: ");
  Serial.println(topic);

  char messageBuffer[30]; //Buffer to hold string
  memcpy(messageBuffer, payload, length); //Copy payload to buffer
  messageBuffer[length] = '\0'; // Terminate array to transform buffer into string
  String message = messageBuffer; // Write to permanent variable

  //Action
  if(!strcmp(topic, (char*)games_droom_isAvailable.c_str())){ // Topic filter
    if(message.equals("no") && isAvailable != false){ // Message filter
      isAvailable = false; // Action
      Serial.println("Set isAvailable_local to false");
    }else if(message.equals("yes") && isAvailable != true){
      isAvailable = true;
      Serial.println("Set isAvailable_local to true");
    }
  }else if(!strcmp(topic, (char*)games_droom_currentUser.c_str())){
    users_user = users_user + message;
    Serial.print("Received a user: ");
    Serial.println(users_user);

    mqtt.subscribe((char*)(users_user + points_END).c_str());
    Serial.println("Subscribed to user points");
    mqtt.unsubscribe((char*)(users_user + points_END).c_str());
    Serial.println("Unsubscribed to user points");
    mqtt.loop();

    mqtt.subscribe((char*)(users_user + users_user_land_END).c_str());
    Serial.println("Subscribed to user land");
    mqtt.unsubscribe((char*)(users_user + users_user_land_END).c_str());
    Serial.println("Unsubscribed to user land");
    mqtt.loop();
    
  } else if(!strcmp(topic, (char*)(users_user + points_END).c_str())){
    userPoints = message.toInt();
    Serial.print("Received user points: ");
    Serial.println(userPoints);
  } else if(!strcmp(topic, (char*)(users_user + users_user_land_END).c_str())){
    userLand = message;
    Serial.print("Received user land: ");
    Serial.println(userLand);
  } else if(!strcmp(topic, (char*)(lands_land + userLand + points_END).c_str())){
    landPoints = message.toInt();
    Serial.print("Received land points: ");
    Serial.println(landPoints);
    Serial.print("User score: ");
    Serial.println(score);

    publishRetain((users_user + points_END), String(userPoints + score));
    publishRetain((lands_land + userLand + points_END), String(landPoints + score));
  }
}

void setup() {
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
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, access_key);
  
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(250);
  }

  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //MQTT
  //Setup client
  mqtt.setServer(broker_url, port);
  mqtt.setCallback(callback);
  
  //Connect to broker
  if(mqtt.connect(client_id, username, password)){
    Serial.println("Connected to MQTT broker");
  }else{
    Serial.println("Failed to connect to MQTT broker");
  }

  //Subscribe to isAvailable topic
  if(mqtt.subscribe((char*)games_droom_isAvailable.c_str())){
    Serial.print("Subscribed to ");
    Serial.println((char*)games_droom_isAvailable.c_str());
  }else{
    Serial.print("Failed to subscribe to ");
    Serial.println((char*)games_droom_isAvailable.c_str());
  }

  defaultStates();
  delay(2500);
}

void defaultStates(){
  score = 0;
  userPoints = 0;
  userLand = "";
  landPoints = 0;
  users_user = "esstelstrijd/users/";
  lands_land = "esstelstrijd/lands/";
  Serial.println("Reset everything");
}

void publishRetain(String topic, String message){
  byte Buffer[message.length() + 1];
  message.getBytes(Buffer, message.length() + 1);
  mqtt.publish((char*)topic.c_str(), Buffer, message.length(), true);
  Serial.print("Published message ");
  Serial.print(message);
  Serial.print(" to ");
  Serial.println(topic);
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
    mqtt.loop();
  }
}

void play(){
  if(lServoUp){
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
      }
    }
  }

  if(rServoUp){
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
      }
    }
  }
}

  updateLCD();
}

void loop(){
  long bufferTimer = millis();
  while(millis() < bufferTimer + 1000){
    mqtt.loop();
  }
  
  //Reset display
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan phone");
  lcd.setCursor(0, 1);
  lcd.print("to start game");

  //Wait for activation
  while(isAvailable){
     mqtt.loop();
  }

  defaultStates();
  mqtt.subscribe((char*)games_droom_currentUser.c_str());
  Serial.println("Subscribed to currentUser");
  mqtt.loop();
  mqtt.unsubscribe((char*)games_droom_currentUser.c_str());
  Serial.println("Unsubscribed to currentUser");
  mqtt.loop();

  countDown();
  updateLCD();

  Serial.println("Playing game...");
  playTimer = millis();
  while(millis() < playTimer + playTime){
    play();
    delay(frameTime);
    mqtt.loop();
  }
  Serial.println("Game finished");

  lServo.write(100);
  rServo.write(100);
  lServoUp = false;
  rServoUp = false;

  mqtt.subscribe((char*)(lands_land + userLand + points_END).c_str());
  Serial.print("Subscribed to land: ");
  Serial.println(userLand);
  mqtt.unsubscribe((char*)(lands_land + userLand + points_END).c_str());
  Serial.print("Unsubscribed to land: ");
  Serial.println(userLand);
  mqtt.loop();

  publishRetain(games_droom_isAvailable, "yes");
//  publishRetain(games_droom_currentUser, "default");

  addPoints();
}
