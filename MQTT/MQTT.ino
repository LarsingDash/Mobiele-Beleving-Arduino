#include <WiFi.h>
#include <PubSubClient.h>

//Hardware pins
const int button = 34;
const int led = 27;

//Loop variables
bool buttonPressedPrev = false;

//Wifi
const char* WLAN_ssid = "MQTT Test Network";
const char* WLAN_access_key = "00177013";

//Client ID
const char* MQTT_client_id = "TestArduino";

//MQTT broker
const char* MQTT_broker_url = "broker.hivemq.com";
const int   MQTT_port       = 1883;
const char* MQTT_username   = "";
const char* MQTT_password   = "";

//MQTT topics
const char* MQTT_topic_test = "esstelstrijd/test";

//MQTT quality of service
const int MQTT_qos = 0;

//Create Wifi and MQTT variables
WiFiClient wifi;
PubSubClient mqtt(wifi);


//Setup callback
void mqttCallback(char* topic, byte* payload, unsigned int length){
  //Logging
  Serial.println("MQTT callback called for topic: ");
  Serial.println(topic);
  Serial.println("Message: ");
  for (int i = 0; i < length; i++){
    Serial.println((char)payload[i]);
  }

  //Check if changed topic is the one we want to listen to
  if(strcmp(topic, MQTT_topic_test) == 0){
    Serial.println("Message received on esstelstrijd/test");
  }
}

void setup() {
  pinMode(button, INPUT_PULLUP);
  pinMode(led, OUTPUT);

  Serial.begin(115200);
  Serial.println("Test start");

  //WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  Serial.println("Connecting to ");
  Serial.println(WLAN_ssid);
  WiFi.begin(WLAN_ssid, WLAN_access_key);
  
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
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
  if(mqtt.subscribe(MQTT_topic_test, MQTT_qos)){
    Serial.println("Subscribed to ");
    Serial.println(MQTT_topic_test);
  }else{
    Serial.println("Failed to subscribe to ");
    Serial.println(MQTT_topic_test);
  }

  mqtt.publish(MQTT_topic_test, "Hi there!");
}

void loop() {
  mqtt.loop();
}
