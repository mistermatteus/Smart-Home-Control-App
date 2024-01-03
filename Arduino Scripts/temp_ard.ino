#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <WiFiManager.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into pin D7 on the ESP8266 12-E - GPIO 5
#define ONE_WIRE_BUS 13

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature DS18B20(&oneWire);

//const char* ssid = "AndroidAP5B5B"; // The name of the Wi-Fi network that will be created
//const char* password = "abbcc1223";   // The password required to connect to it, leave blank for an open network
ESP8266WebServer server(80);
WiFiManager wifiManager;
bool stat_ac = true;
bool stat_h = true;

char temperatureCString[7];
char temperatureFString[7];

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println('\n');
  pinMode(D0, OUTPUT);
  digitalWrite(D0, HIGH);
  pinMode(D1, OUTPUT);
  digitalWrite(D1, HIGH);
  pinMode(D2, OUTPUT);
  digitalWrite(D2, HIGH);
  pinMode(D8, OUTPUT);
  digitalWrite(D8, HIGH);

  Serial.println("Temperature Sensor NodeMCU:\n");

  // Start up the library
  DS18B20.begin();
  wifiManager.autoConnect("AutoConnectAP");

  Serial.println("\nSuccessfully connected");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());

  String message = "{\"ip\":\"";
  message += WiFi.localIP().toString();
  message += "\"}\n";
  Serial.println(message);
  
  server.on("/stat/h", handleStatH);
  server.on("/stat/ac", handleStatAC);
  server.on("/data", handleData);
  server.on("/wifi", handleWiFi); //Associate the handler function to the path

  server.begin(); //Start the server
  Serial.println("Server listening");
}

void loop() {
  server.handleClient(); //Handling of incoming requests
  if (stat_h) {
    digitalWrite(D0, HIGH);
    digitalWrite(D1, HIGH);
  }
  else {
    digitalWrite(D0, LOW);
  }
  if (stat_ac) {
    digitalWrite(D2, HIGH);
    digitalWrite(D1, HIGH);
  }
  else {
    digitalWrite(D2, LOW);
  }
  if (!(stat_h) && !(stat_ac)) {
    digitalWrite(D1, LOW);
  }
}

void getTemperature() {
  float tempC;
  float tempF;
  do {
    DS18B20.requestTemperatures();
    tempC = DS18B20.getTempCByIndex(0);
    dtostrf(tempC, 2, 2, temperatureCString);
    tempF = DS18B20.getTempFByIndex(0);
    dtostrf(tempF, 3, 2, temperatureFString);
    delay(100);
  } while (tempC == 85.0 || tempC == (-127.0));
}

void handleStatH() {
  const size_t capacity = JSON_OBJECT_SIZE(1) + 10;
  DynamicJsonBuffer jsonBuffer(capacity);

  String payload = server.arg("plain");
  const char* json = payload.c_str();

  JsonObject& root = jsonBuffer.parseObject(json);

  const char* h_stat = root["h_stat"]; // "1"

  if (strcmp(h_stat, "1") == 0) {
    stat_h = true;
  }
  if (strcmp(h_stat, "0") == 0) {
    stat_h = false;
  }

  String message = "{\"h_stat\":\"";
  message += h_stat;
  message += "\"}\n";
  Serial.println(message);
  server.send(200, "application/json", message);
}

void handleStatAC() {
  const size_t capacity = JSON_OBJECT_SIZE(1) + 20;
  DynamicJsonBuffer jsonBuffer(capacity);

  String payload = server.arg("plain");
  const char* json = payload.c_str();

  JsonObject& root = jsonBuffer.parseObject(json);

  const char* ac_stat = root["ac_stat"]; // "1"

  if (strcmp(ac_stat, "1") == 0) {
    stat_ac = true;
  }
  if (strcmp(ac_stat, "0") == 0) {
    stat_ac = false;
  }

  String message = "{\"ac_stat\":\"";
  message += ac_stat;
  message += "\"}\n";
  Serial.println(message);
  server.send(200, "application/json", message);
}

void handleData() {
  getTemperature();
  String message = "{\"temp\":\"";
  message += temperatureFString;
  message += "\"}\n";
  server.send(200, "application/json", message);
  Serial.println(message);
}

void handleWiFi() {
  String message = "{\"ip\":\"";
  message += WiFi.localIP().toString();
  message += "\"}\n";
  server.send(200, "application/json", message);
  Serial.println(message);
}
