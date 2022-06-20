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
long playTimer;

bool isAvailable;
int landPoints;
int userPoints;
String userLand;

//Wifi
const char* ssid = "MQTT Network";
const char* access_key = "00177013";

//Client IP
const char* client_id = "arduino_johan_en_de_eenhoorn";

//MQTT broker
const char* broker_url = "broker.hivemq.com";
const int   port       = 1883;
const char* username   = "";
const char* password   = "";

//MQTT topics
const String games_jede_isAvailable   = "esstelstrijd/games/jede/isAvailable";
const String games_jede_currentUser   = "esstelstrijd/games/jede/currentUser";
String users_user                     = "esstelstrijd/users/";
String lands_land                     = "esstelstrijd/lands/";
const String points_END               = "/points";
const String users_user_land_END      = "/land";

//Create Wifi and MQTT variables
WiFiClient wifi;
PubSubClient mqtt(wifi);

//LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);
byte line[] = {B00100,B00100,B00100,B00100,B00100,B00100,B00100,B00100};
byte arrow[] = {B00100,B00100,B00100,B00100,B10101,B11111,B01110,B00100};
byte attack[] = {B00100,B00100,B00100,B00100,B00100,B00100,B01110,B00100};
byte defend[] = {B00000,B11111,B11111,B11111,B11111,B01110,B00100,B00000};

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
  if(!strcmp(topic, (char*)games_jede_isAvailable.c_str())){ // Topic filter
    if(message.equals("no") && isAvailable != false){ // Message filter
      isAvailable = false; // Action
      Serial.println("Set isAvailable_local to false");
    }else if(message.equals("yes") && isAvailable != true){
      isAvailable = true;
      Serial.println("Set isAvailable_local to true");
    }
  }else if(!strcmp(topic, (char*)games_jede_currentUser.c_str())){
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
  if(mqtt.subscribe((char*)games_jede_isAvailable.c_str())){
    Serial.print("Subscribed to ");
    Serial.println((char*)games_jede_isAvailable.c_str());
  }else{
    Serial.print("Failed to subscribe to ");
    Serial.println((char*)games_jede_isAvailable.c_str());
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

  //Draw game elements
  lcd.setCursor(6, 0);
  lcd.write(1);
  lcd.setCursor(7, 0);
  lcd.write(0);
  lcd.setCursor(8, 0);
  lcd.write(1);
  lcd.setCursor(7, 1);
  lcd.write(0);
  if(score > 9){
    lcd.setCursor(14, 0);
  }else{
    lcd.setCursor(15, 0);
  }
  lcd.print(score);
  
  
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
    mqtt.loop();
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
  long bufferTimer = millis();
  while(millis() < bufferTimer + 2500){
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
  mqtt.subscribe((char*)games_jede_currentUser.c_str());
  Serial.println("Subscribed to currentUser");
  mqtt.loop();
  mqtt.unsubscribe((char*)games_jede_currentUser.c_str());
  Serial.println("Unsubscribed to currentUser");
  mqtt.loop();
  
  countDown();

  Serial.println("Playing game...");
  //Play game for however long playTime is set to
  playTimer = millis();
  while(millis() < playTimer + playTime){
    play();
    delay(frameTime);
    mqtt.loop();
  }
  Serial.println("Game finished");

  mqtt.subscribe((char*)(lands_land + userLand + points_END).c_str());
  Serial.print("Subscribed to land: ");
  Serial.println(userLand);
  mqtt.unsubscribe((char*)(lands_land + userLand + points_END).c_str());
  Serial.print("Unsubscribed to land: ");
  Serial.println(userLand);
  mqtt.loop();

  publishRetain(games_jede_isAvailable, "yes");
//  publishRetain(games_jede_currentUser, "default");

  addPoints();
}
