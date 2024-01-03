#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <WiFiManager.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

// Data wire is plugged into pin D1 on the ESP8266 12-E - GPIO 5
#define led  5         //D1: pin for the LED

//const char* ssid = "AndroidAP5B5B"; // The name of the Wi-Fi network that will be created
//const char* password = "abbcc1223";   // The password required to connect to it, leave blank for an open network
ESP8266WebServer server(80);
WiFiManager wifiManager;
String header;
bool l_stat = false;
int timer = 10 * 1000;
unsigned long start_scan = 0;
unsigned long scan_time = 2000;
bool stat = false;

#define soilPower 13//Variable for Soil moisture Power
double val = 0.0; //value for storing moisture value
int soilPin = A0;//Declare a variable for the soil moisture sensor
double maxVal = 883.0;
int thresholdUp = 400;
int thresholdDown = 250;
String DisplayWords;

//Rather than powering the sensor through the 3.3V or 5V pins,
//we'll use a digital pin to power the sensor. This will
//prevent corrosion of the sensor as it sits in the soil.

void setup() {
  Serial.begin(115200);   // open serial over USB
  delay(10);
  Serial.println("Moisture Sensor NodeMCU:\n");

  pinMode(led, OUTPUT);//Set D1 as an OUTPUT
  pinMode(soilPin, INPUT);//Set A0 as an INPUT
  pinMode(soilPower, OUTPUT);//Set D7 as an OUTPUT
  digitalWrite(soilPower, LOW);//Set to LOW so no power is flowing through the sensor

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
  server.on("/wifi", handleWiFi); //Associate the handler function to the path

  server.begin(); //Start the server
  Serial.println("Server listening");
}

void loop() {
  server.handleClient(); //Handling of incoming requests
  if ((millis() - start_scan) >= scan_time) {
    int sensorValue;
    sensorValue = readSoil();

    //Write what we want to desplay on the screen:
    Serial.print("Water Level: ");
    Serial.println(sensorValue);

    if (sensorValue <= thresholdDown) {
      DisplayWords = "{\"stat\":1}\n";
      Serial.println(DisplayWords);
      digitalWrite(led, HIGH);
      stat = true;
    } else if (sensorValue >= thresholdUp) {
      DisplayWords = "{\"stat\":0}\n";
      Serial.println(DisplayWords);
      digitalWrite(led, LOW);
      stat = false;
    } else {
      Serial.println(DisplayWords);
    }
    start_scan = millis();
  }
}
//This is a function used to get the soil moisture content
int readSoil() {
  digitalWrite(soilPower, HIGH);//turn D7 "On"
  delay(10);//wait 10 milliseconds
  val = analogRead(soilPin);//Read the SIG value form sensor
  digitalWrite(soilPower, LOW);//turn D7 "Off"
  return val;//send current moisture value
}

void handleStat() {
  const size_t capacity = JSON_OBJECT_SIZE(1) + 20;
  DynamicJsonBuffer jsonBuffer(capacity);

  String payload = server.arg("plain");
  const char* json = payload.c_str();

  JsonObject& root = jsonBuffer.parseObject(json);

  const int m_stat = root["stat"]; // "1"

  if (m_stat) {
    stat = true;
  }
  else {
    stat = false;
  }

  String message = "{\"stat\":\"";
  message += m_stat;
  message += "\"}\n";
  Serial.println(message);
  server.send(200, "application/json", message);
}

void handleData() {
  String message = "{\"moisture\":\"";
  message += readSoil();
  message += "\",\n\"stat\":\"";
  message += stat;
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
