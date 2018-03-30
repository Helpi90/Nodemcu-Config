#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_BME280.h>
#include <Wire.h>
#include <ArduinoJson.h> 
   
#include "settings.h"

const int baudrate = 115200;
#define SDA D3
#define SDC D4

// MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// BME280, Luftdruck-Sensor
Adafruit_BME280 bme280;

struct {
  bool read;
  bool valid;
  int ppm;
} zh18_result = { .read = true };

struct {
  bool read;
  bool valid;
  float t; // Temperature
  float h; // Humidity
  float p; // Pressure
} bme280_result = { .read = true };


char hostString[20] = {0};
char url[128] = {0};
bool shouldSaveConfig = false;

// All timestamps/periods in milliseconds
unsigned long uptime = 0;
unsigned long last_send = 0;
const unsigned long sending_interval = 10*1000;


void debugf(char *fmt, ... ){
  char buf[128]; // resulting string limited to 128 chars
  va_list args;
  va_start (args, fmt );
  vsnprintf(buf, 128, fmt, args);
  va_end (args);
  Serial.print(buf);
}

void debugf_float(char *fmt, float val){
  char buf[9];
  dtostrf(val, 6, 2, buf);
  debugf(fmt, buf);
}

String Float2String(const float value) {
  // Convert a float to String with two decimals.
  char temp[12];
  String s;

  dtostrf(value, 8, 2, temp);
  s = String(temp);
  s.trim();
  return s;
}

bool initBME280(char addr) {
  debugf("[bme280] Trying on 0x%02X ... ", addr);

  if (bme280.begin(addr)) {
    debugf("found\n");
    return true;
  } else {
    debugf("not found\n");
    return false;
  }
}

void readBME280() {
  bme280_result.t = bme280.readTemperature();
  bme280_result.h = bme280.readHumidity();
  bme280_result.p = bme280.readPressure();
  bme280_result.valid = !isnan(bme280_result.t) && !isnan(bme280_result.h) && !isnan(bme280_result.p);

  if (bme280_result.valid) {
    debugf_float("Temperature: %s C\n",   bme280_result.t);
    debugf_float("Humidity:    %s %%\n",  bme280_result.h);
    debugf_float("Pressure:    %s hPa\n", bme280_result.p/100);
  } else {
    debugf("[bme280] reading failed\n");
  }
}

void readZH18()
{
  zh18_result.valid = false;

  // command to ask for data
  const byte cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
  char response[9]; // for answer

  Serial.flush();
  delay(100);
  Serial.begin(9600);
  Serial.swap();

  Serial.write(cmd, sizeof(cmd));
  int read = Serial.readBytes(response, sizeof(response));

  Serial.swap();
  Serial.begin(baudrate);

  if (read != sizeof(response)) {
    if (!read){
      Serial.println("[zh18] no bytes received");
    } else {
      Serial.print("[zh18] received ");
      Serial.print(read);
      Serial.println(" bytes");
    }
    return;
  }

  if (response[0] != 0xFF)
  {
    Serial.println("[zh18] Wrong starting byte received");
    return;
  }

  if (response[1] != 0x86)
  {
    Serial.println("[zh18] Wrong command received");
    return;
  }

  char checksum = 0;
  for (char i = 1; i < 8; i++)
  {
    checksum += response[i];
  }
  checksum = 0xff - checksum + 1;

  if (checksum != response[8])
  {
    Serial.printf("[zh18] Checksum invalid: expected=%02x is=%02x\n", checksum, response[8]);
    return;
  }

  bool preheating = uptime < 1000*60*3; // up to three minutes preheat time

  zh18_result.ppm   = response[3] | response[2] << 8;
  zh18_result.valid = !preheating || (preheating && zh18_result.ppm != 400);
  debugf("CO2:           %04d ppm\n", zh18_result.ppm);
}

void sendData() {
  String data, url;

  // Build data
  data = "";
  data += F("sensors,host=");
  data += hostString;
  data += F(",location=");
  data += location;
  data += " ";

  if (bme280_result.valid) {
    data += F("temperature=");
    data += Float2String(bme280_result.t);
    data += F(",humidity=");
    data += Float2String(bme280_result.h);
    data += F(",pressure=");
    data += Float2String(bme280_result.p);
    data += F(",");
  }

  if (zh18_result.valid) {
    data += F("co2=");
    data += Float2String(zh18_result.ppm);
    data += F(",");
  }

  if (!data.endsWith(",")){
    debugf("No data available\n");
    return;
  }

  data += F("uptime=");
  data += String(uptime/1000);

  // Build URL
  url = String("http://");
  url += influx_server;
  url += F(":");
  url += influx_port;
  url += F("/");
  url += F("write?precision=s&db=");
  url += influx_database;

  // Send data to InfluxDB
  HTTPClient http;
  http.begin(url);
  http.setAuthorization(influx_user, influx_pass);
  http.addHeader("Content-Type", "text/plain");
  int status = http.POST(data);
  if (status != 204) {
    Serial.printf("[http] unexpected status code: %d\n", status);
    http.writeToStream(&Serial);
  }
  http.end();

}

void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void mountSpiffs() {
  Serial.println("mounted file system");
  if (SPIFFS.exists("/config.json")) {
    //file exists, reading and loading
    Serial.println("reading config file");
    File configFile = SPIFFS.open("/config.json", "r");
    if (configFile) {
      Serial.println("opened config file");
      size_t size = configFile.size();
      // Allocate a buffer to store contents of the file.
      std::unique_ptr<char[]> buf(new char[size]);
      
      configFile.readBytes(buf.get(), size);
      DynamicJsonBuffer jsonBuffer;
      JsonObject& json = jsonBuffer.parseObject(buf.get());
      if (json.success()) {
        Serial.println("parsed json");
        strcpy(influx_server, json["influx_server"]);
        strcpy(influx_port, json["influx_port"]);
        strcpy(influx_database, json["influx_database"]);
        strcpy(influx_measurement, json["influx_measurement"]);
        strcpy(influx_user, json["influx_user"]);
        strcpy(influx_pass, json["influx_pass"]);
        strcpy(location, json["location"]);
      } else {
        Serial.println("failed to load json config");
      }
    }
  } else {
    Serial.println("no config found. writing defaults.");
    writeToConfig();
  }
}

void writeToConfig() {
  Serial.println("saving config");
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["influx_server"] = influx_server;
  json["influx_port"] = influx_port;
  json["influx_database"] = influx_database;
  json["influx_measurement"] = influx_measurement;
  json["influx_user"] = influx_user;
  json["influx_pass"] = influx_pass;
  json["location"] = location;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("failed to open config file for writing");
  }

  json.printTo(configFile);
  configFile.close();
  //end save
}

void showConfig() {
  Serial.printf("Influx server: %s\n", influx_server);
  Serial.printf("Influx port: %s\n", influx_port);
  Serial.printf("Influx database: %s\n", influx_database);
  Serial.printf("Influx measurement: %s\n", influx_measurement);
  Serial.printf("Influx user: %s\n", influx_user);
}

void setup() {
  WiFiManager wifiManager;
  wifiManager.setDebugOutput(false);
  
  Serial.begin(baudrate);

  Wire.pins(SDC, SDA);
  Wire.begin(SDC, SDA);

  sprintf(hostString, "Airsensor-%06X", ESP.getChipId());
  Serial.printf("\nHostname: %s\n", hostString);

  // Serial.setDebugOutput(true);

  if (SPIFFS.begin()) {
    mountSpiffs();
  } else {
    Serial.println("failed to mount FS");
  }

  WiFiManagerParameter custom_influx_server("ifserver", "influx server", influx_server, 40);
  WiFiManagerParameter custom_influx_port("ifport", "influx port", influx_port, 6);
  WiFiManagerParameter custom_influx_db("ifdb", "influx database", influx_database, 16);
  WiFiManagerParameter custom_influx_measure("ifmeasure", "influx measurement", influx_measurement, 16);
  WiFiManagerParameter custom_influx_user("ifuser", "influx user", influx_user, 16);
  WiFiManagerParameter custom_influx_pass("ifpass", "influx pass", influx_pass, 16);
  WiFiManagerParameter custom_location("location", "location", location, 16);
  
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  wifiManager.addParameter(&custom_influx_server);
  wifiManager.addParameter(&custom_influx_port);
  wifiManager.addParameter(&custom_influx_db);
  wifiManager.addParameter(&custom_influx_measure);
  wifiManager.addParameter(&custom_influx_user);
  wifiManager.addParameter(&custom_influx_pass);
  wifiManager.addParameter(&custom_location);

  if (!wifiManager.autoConnect(hostString, "fortytwo")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  if (shouldSaveConfig) {
    Serial.println("saving config");
    writeToConfig();
  }

  // BME280 initialisieren
  if (bme280_result.read) {
    if (!initBME280(0x76) && !initBME280(0x77)) {
      debugf("Check BME280 wiring\n");
      bme280_result.read = false;
    }
  }

  showConfig();

  delay(2500);
}


void loop() {
  uptime = millis();

  // uptime restarted at zero? (overflow after 50 days)
  if (last_send > uptime)
    last_send = 0;

  // Sending now?
  if (last_send == 0 || last_send + sending_interval < uptime){
    debugf("\nreading sensors ...\n");
    debugf("Uptime:        %04d\n", uptime/1000);

    if (bme280_result.read) readBME280();
    if (zh18_result.read)   readZH18();


    sendData();

    last_send = uptime;
    debugf("done\n");
  }
}
