#include <Event.h>
#include <Timer.h>

#define D5 (14)
#define D6 (12)
#define D7 (13)

#define PUMP_PIN D7

#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Timer.h>
#include <SoftwareSerial.h>


Adafruit_SSD1306 display(128, 64, &Wire, -1);
SoftwareSerial mySerial(D5, D6);
Timer t;

WiFiClient MaraxScreen;
PubSubClient client(MaraxScreen);

// Config
const char* SSID = "";
const char* PSK = "";
const char* MQTT_BROKER = "";
const char* MQTT_USER = "";
const char* MQTT_PASSWORD = "";

// State
// set to true/false when using another type of reed sensor
bool reedOpenSensor = true;
int timerCount = 0;
int prevTimerCount = 0;
bool timerStarted = false;
long timerStartMillis = 0;
long timerStopMillis = 0;
long serialUpdateMillis = 0;
int pumpInValue = 0;

String machineState;
bool machineHeating;
bool machineHeatingBoost;
int hxTemperature = NULL;
int steamTemperature = NULL;

const byte numChars = 32;
char receivedChars[numChars];

void setup() {
  Serial.begin(9600);
  
  setupTimer();
  setupSerial();
  setupDisplay();
  
  t.every(100, render);

  // Set timeout of the wifi connection to 240 seconds
  unsigned long timeout = 240000; 

  // Start timer
  unsigned long startTime = millis(); 

  while (millis() - startTime < timeout) {
    if (SSID != "" && PSK != "" && MQTT_BROKER != "") {
      setupWifi();
      break;
    }
  }

  // Turn off WiFi if connection cannot be established within timeout period
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.mode(WIFI_OFF);
  }
}

void loop() {
  t.update();
  updateTimer();
  updateSerial();
  
  if (SSID != "" && PSK != "" && MQTT_BROKER != "") {
    updateWifi();
  }
}
