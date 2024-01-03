#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <WiFiManager.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

// Data wire is plugged into pin D1 on the ESP8266 12-E - GPIO 5
#define led  5         //D1: pin for the LED
#define sensor 13        //D7: input pin (for PIR sensor)

//const char* ssid = "AndroidAP5B5B"; // The name of the Wi-Fi network that will be created
//const char* password = "abbcc1223";   // The password required to connect to it, leave blank for an open network
ESP8266WebServer server(80);
WiFiManager wifiManager;
String header;
bool l_stat = false;
bool override = false;
int timer = 5 * 1000;
unsigned long start_time;
unsigned long start_scan = 0;
unsigned long scan_time = 2000;

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("Motion Sensor NodeMCU:\n");
  pinMode(led, OUTPUT);      // declare LED as output
  digitalWrite(led, HIGH);
  pinMode(D7, INPUT);     // declare sensor as input
  pinMode(D8, OUTPUT);     // declare sensor as input
  digitalWrite(D8, HIGH);

  // Start up the library
  wifiManager.autoConnect("AutoConnectAP");

  Serial.println("\nSuccessfully connected");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());

  String message = "{\"ip\":\"";
  message += WiFi.localIP().toString();
  message += "\"}\n";
  Serial.println(message);

  server.on("/stat", handleStat);
  server.on("/data", handleData);
  server.on("/timer", handleTimer);
  server.on("/override", handleOverride);
  server.on("/wifi", handleWiFi); //Associate the handler function to the path

  server.begin(); //Start the server
  Serial.println("Server listening\n");
  digitalWrite(led, LOW);
  message = "{\"stat\":\"0\"}\n";
}

void loop() {
  String message;
  server.handleClient(); //Handling of incoming requests
  //Serial.println(motionsen());
  if (!(override)) {
    if ((digitalRead(sensor) == HIGH) && ((millis() - start_scan) >= scan_time)) {
      Serial.println("motion detected");
      if (!(l_stat)) {
        digitalWrite(led, HIGH);
        message = "{\"stat\":\"1\"}\n";
        Serial.println(message);
        l_stat = true;
      }
      start_scan = millis();
      start_time = millis();
    }
    else if (millis() == (start_time + timer)) {
      digitalWrite(led, LOW);
      message = "{\"stat\":\"0\"}\n";
      Serial.println(message);
      l_stat = false;
      delay(1);
    }
  }
}

String motionsen() {
  String message;
  if (digitalRead(sensor) == HIGH) {
    message = "{\"motion\":\"true\"}\n";
  }
  else {
    message = "{\"motion\":\"false\"}\n";
  }
  return message;
}

void handleStat() {
  const size_t capacity = JSON_OBJECT_SIZE(1) + 10;
  DynamicJsonBuffer jsonBuffer(capacity);

  String payload = server.arg("plain");
  const char* json = payload.c_str();

  JsonObject& root = jsonBuffer.parseObject(json);

  const char* stat = root["stat"]; // "1"

  String message;
  if (strcmp(stat, "1") == 0) {
    digitalWrite(led, HIGH);
    l_stat = true;
  }
  if (strcmp(stat, "0") == 0) {
    digitalWrite(led, LOW);
    l_stat = false;
  }
  message = "{\"stat\":\"";
  message += stat;
  message += "\"}\n";
  Serial.println(message);
  server.send(200, "application/json", message);
}

void handleData() {
  String message = "{\"stat\":\"";
  message += l_stat;
  message += "\"}\n";
  server.send(200, "application/json", message);
  Serial.println(message);
}

void handleTimer() {
  const size_t capacity = JSON_OBJECT_SIZE(1) + 20;
  DynamicJsonBuffer jsonBuffer(capacity);

  String payload = server.arg("plain");
  const char* json = payload.c_str();

  JsonObject& root = jsonBuffer.parseObject(json);

  int timer = root["timer"]; // 100

  String message;
  message = "{\"timer\":";
  message += timer;
  message += "}\n";
  server.send(200, "application/json", message);
  Serial.println(message);
  timer = timer * 60 * 100;
}

void handleOverride() {
  const size_t capacity = JSON_OBJECT_SIZE(1) + 10;
  DynamicJsonBuffer jsonBuffer(capacity);

  String payload = server.arg("plain");
  const char* json = payload.c_str();

  JsonObject& root = jsonBuffer.parseObject(json);

  int override = root["timer"]; // 100

  String message;
  message = "{\"override\":";
  message += override;
  message += "}\n";
  server.send(200, "application/json", message);
  Serial.println(message);
  timer = timer * 60 * 100;
}

void handleWiFi() { //Handler for the body path
  if (server.hasArg("plain") == false) { //Check if body received
    server.send(200, "text/plain", "Body not received\n");
    return;
  }

  const size_t capacity = JSON_OBJECT_SIZE(3) + 30;
  DynamicJsonBuffer jsonBuffer(capacity);

  String payload = server.arg("plain");
  const char* json = payload.c_str();

  JsonObject& root = jsonBuffer.parseObject(json);

  const char* nname = root["nname"]; // "name";
  const char* pwd = root["pwd"]; // "1234";
  const char* wifi = root["wifi"]; // "on";

  String message = "{\"ip\":\"";
  message += WiFi.localIP().toString();
  message += "\"}\n";
  server.send(200, "application/json", message);
  Serial.println(message);
}
