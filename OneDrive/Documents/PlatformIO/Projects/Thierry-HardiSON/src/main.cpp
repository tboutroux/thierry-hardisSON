#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "TETETOTO";
const char* password = "Tetetoto2101";
const char* url = "https://hook.eu2.make.com/7fiewyk971fohxgli1hcv171plr6w5ss";

#define SENSOR_PIN 26 // ESP32 pin GPIO18 connected to the OUT pin of the sound sensor
#define BUZZER_PIN 27 // ESP32 pin GPIO27 connected to the IN pin of the sound sensor

struct Data {
  int decibel;
  unsigned long timestamp;
};

Data dataBuffer[96]; // Tableau pour stocker les données des 24 dernières heures
int dataCounter = 0;
unsigned long lastWiFiConnectTime = 0; // Variable pour stocker le temps de dernière connexion au WiFi

void connectToWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting");

  int i = 0;
  while (i < 60) {
    if (WiFi.status() != WL_CONNECTED) {
      delay(100);
      Serial.print(".");
      i++;
    } else {
      break;
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nConnection failed");
  }

  lastWiFiConnectTime = millis();
}

void setup() {
  Serial.begin(9600);
  pinMode(SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Connect to WiFi
  connectToWiFi();

  // Store the time of WiFi connection
  lastWiFiConnectTime = millis();

}

void sendData(int decibel, unsigned long timestamp) {
  HTTPClient http;
  String urlWithParams = String(url) + "?niveau=" + String(decibel, 2) + "&timestamp=" + String(timestamp);
  http.begin(urlWithParams);

  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }

  int decibel = analogRead(SENSOR_PIN);

  if (decibel > 80) {
    Serial.println("The sound has appeared");
    Serial.println("Decibel: " + String(decibel));
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    Serial.println("The sound has appeared but it's too low");
  }

  // Store data in buffer
  if (dataCounter < 96) {
    unsigned long currentTime = millis() + lastWiFiConnectTime; // Calculate the current time since last WiFi connection
    Data newData = {decibel, currentTime};
    dataBuffer[dataCounter] = newData;

    // Display the data stored in the buffer
    Serial.println("Data stored in the buffer:");
    Serial.println("Decibel: " + String(dataBuffer[dataCounter].decibel));
    Serial.println("Timestamp: " + String(dataBuffer[dataCounter].timestamp));

    dataCounter++;
  }

  // Send data when connected to WiFi
  if (WiFi.status() == WL_CONNECTED) {

    if (dataCounter == 0) {
      Serial.println("No data to send");
    } else if (dataCounter == 1) {
      sendData(dataBuffer[0].decibel, dataBuffer[0].timestamp);

      // Remove the data sent from the buffer
      dataBuffer[0] = dataBuffer[1];
      dataCounter--;

    } else {
      for (int i = 0; i < dataCounter; i++) {
        sendData(dataBuffer[i].decibel, dataBuffer[i].timestamp);

        // Remove the data sent from the buffer
        dataBuffer[i] = dataBuffer[i + 1];
        dataCounter--;
      }
    }
    // Reset data counter and buffer after sending data
    dataCounter = 0;
  }

  delay(900000); // 15 minutes
}
