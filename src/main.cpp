#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

uint32_t delayMS;

const char* ssid = "WIFI_LABO";
const char* password = "EpsiWis2018!";
const char* url = "https://hook.eu2.make.com/7fiewyk971fohxgli1hcv171plr6w5ss";

unsigned long previousMillis = 0;
const long interval = 600000; // 30 minutes

#define SENSOR_PIN 26 // ESP32 pin GPIO18 connected to the OUT pin of the sound sensor
#define BUZZER_PIN 27 // ESP32 pin GPIO27 connected to the IN pin of the sound sensor

int lastState = HIGH;  // the previous state from the input pin
int currentState; 
int decibel;         // the current reading from the input pin

void setup(){
    // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // initialize the ESP32's pin as an input
  pinMode(SENSOR_PIN, INPUT);
  // initialize the ESP32's pin as an output
  pinMode(BUZZER_PIN, OUTPUT);

  // Connect to the WiFi network
  WiFi.mode(WIFI_STA); //Optional
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting");

  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(100);
  }

    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
} 

void loop() {
  // read the state of the the ESP32's input pin
  decibel = analogRead(SENSOR_PIN);

  if (decibel > 80) {
    Serial.println("The sound has appeared");
    Serial.println("Decibel: " + String(decibel));
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    Serial.println("The sound has appeared but it's too low");
  }

  delay(2000);
}

void sendData(int decibel) {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  HTTPClient http;
  String urlWithParams = String(url) + "?temp=" + String(decibel, 2);
  http.begin(urlWithParams);

  int httpResponseCode = http.GET();
  if(httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}