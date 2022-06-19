#include <WiFi.h>
#include <PubSubClient.h>

//Hardware pins

//Loop variables
bool isAvailable;
int landPoints;
int userPoints;
String userLand;

//Wifi
const char* ssid = "MQTT Test Network";
const char* access_key = "00177013";

//Client ID
const char* client_id = "TestArduino";

//MQTT broker
const char* broker_url = "broker.hivemq.com";
const int   port       = 1883;
const char* username   = "";
const char* password   = "";

//MQTT topics
const String games_fest_isAvailable   = "esstelstrijd/games/fest/isAvailable";
const String games_fest_currentUser   = "esstelstrijd/games/fest/currentUser";
String users_user                     = "esstelstrijd/users/";
String lands_land                     = "esstelstrijd/lands/";
const String points_END               = "/points";
const String users_user_land_END      = "/land";

//Create Wifi and MQTT variables
WiFiClient wifi;
PubSubClient mqtt(wifi);

//Setup callback
void mqttCallback(char* topic, byte* payload, unsigned int length){
  //Logging
  Serial.println("-------------------------------");
  Serial.print("MQTT callback called for topic: ");
  Serial.println(topic);

  char messageBuffer[30]; //Buffer to hold string
  memcpy(messageBuffer, payload, length); //Copy payload to buffer
  messageBuffer[length] = '\0'; // Terminate array to transform buffer into string
  String message = messageBuffer; // Write to permanent variable

  //Action
  if(!strcmp(topic, (char*)games_fest_isAvailable.c_str())){ // Topic filter
    if(message.equals("no") && isAvailable != false){ // Message filter
      isAvailable = false; // Action
      Serial.println("Set isAvailable_local to false");
    }else if(message.equals("yes") && isAvailable != true){
      isAvailable = true;
      Serial.println("Set isAvailable_local to true");
    }
  }else if(!strcmp(topic, (char*)games_fest_currentUser.c_str())){
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

    String newUserPoints = String(userPoints + 50);
    byte userPointsBuffer[newUserPoints.length() + 1];
    newUserPoints.getBytes(userPointsBuffer, newUserPoints.length() + 1);
    mqtt.publish((char*)(users_user + points_END).c_str(), userPointsBuffer, newUserPoints.length(), true);
    Serial.print("Published new points to user: ");
    Serial.println(newUserPoints);

    String newLandPoints = String(landPoints + 50);
    byte landPointsBuffer[newLandPoints.length() + 1];
    newLandPoints.getBytes(landPointsBuffer, newLandPoints.length() + 1);
    mqtt.publish((char*)(lands_land + userLand + points_END).c_str(), landPointsBuffer, newLandPoints.length(), true);
    Serial.print("Published new points to land: ");
    Serial.println(newLandPoints);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Test start");

  //WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, access_key);
  
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //MQTT
  //Setup client
  mqtt.setServer(broker_url, port);
  mqtt.setCallback(mqttCallback);

  //Connect to broker
  if(mqtt.connect(client_id, username, password)){
    Serial.println("Connected to MQTT broker");
  }else{
    Serial.println("Failed to connect to MQTT broker");
  }

  //Subscribe to isAvailable topic
  if(mqtt.subscribe((char*)games_fest_isAvailable.c_str())){
    Serial.print("Subscribed to ");
    Serial.println((char*)games_fest_isAvailable.c_str());
  }else{
    Serial.print("Failed to subscribe to ");
    Serial.println((char*)games_fest_isAvailable.c_str());
  }

  defaultStates();
  delay(2500);
}

void defaultStates(){
  userPoints = 0;
  userLand = "";
  landPoints = 0;
  users_user = "esstelstrijd/users/";
  lands_land = "esstelstrijd/lands/";
  Serial.println("Reset everything");
}

void loop() {
  int timer2 = millis();
  while(millis() < timer2 + 1000){
    mqtt.loop();
  }
  
  while(isAvailable){
    mqtt.loop();
  }

  defaultStates();
  mqtt.subscribe((char*)games_fest_currentUser.c_str());
  Serial.println("Subscribed to currentUser");
  mqtt.loop();
  mqtt.unsubscribe((char*)games_fest_currentUser.c_str());
  Serial.println("Unsubscribed to currentUser");
  mqtt.loop();

  Serial.println("Playing game...");
  int timer = millis();
  while(millis() < timer + 10000){
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

  String newAvailability = "yes";
  byte availabilityBuffer[newAvailability.length() + 1];
  newAvailability.getBytes(availabilityBuffer, newAvailability.length() + 1);
  mqtt.publish((char*)games_fest_isAvailable.c_str(), availabilityBuffer, newAvailability.length(), true);
}
