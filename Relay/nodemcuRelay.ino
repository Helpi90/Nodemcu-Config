/*
   IoT - Remotely control 220-240V bulb with NodeMCU through MQTT broker
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define GPIO_PIN D0

// Update these with values suitable for your network.
const char* ssid = "CHANGE_TO_YOUR_SSID";
const char* password = "CHANGE_TO_YOUR_PASSWORD";
const char* mqtt_server = "iot.eclipse.org";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void setup()
{
  Serial.begin(115200);
  setup_wifi();
  mqttClient.setServer(mqtt_server, 1883);
  mqttClient.setCallback(callback);
  pinMode(GPIO_PIN, OUTPUT);
  digitalWrite(GPIO_PIN, LOW);
}

void loop()
{
  if (!mqttClient.connected()) 
  {
    connect();
  }

  mqttClient.loop();
}

void setup_wifi()
{
  delay(100);
  
  // We start by connecting to a WiFi network
  Serial.print("Connecting to: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Command from MQTT broker is : ");
  Serial.print(topic);
  char p = payload[0];

  // if MQTT comes a 0 turn OFF on D0
  if (p == '0')
  {
    digitalWrite(GPIO_PIN, HIGH);
    Serial.println(" Turn Off! ");
  }

  // if MQTT comes a 1, turn ON pin D0
  if (p == '1')
  {
    digitalWrite(GPIO_PIN, LOW);
    Serial.println(" Turn On! " );
  }

  Serial.println();
}

void connect()
{
  // Loop until we're connected
  while (!mqttClient.connected())
  {
    Serial.print("Attempting MQTT connection...");
    
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect
    // if you MQTT broker has clientID,username and password then change following line to
    // if (mqttClient.connect(clientId,userName,passWord))
    if (mqttClient.connect(clientId.c_str()))
    {
      Serial.println("connected");
      
      //once connected to MQTT broker, subscribe to the topic
      mqttClient.subscribe("/lamp/status/");
    } 
    else 
    {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      
      // Wait 6 seconds before retrying
      delay(6000);
    }
  }
}
