/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-sound-sensor
 */

#include <Arduino.h>

#define SENSOR_PIN 26 // ESP32 pin GPIO18 connected to the OUT pin of the sound sensor
#define BUZZER_PIN 27 // ESP32 pin GPIO27 connected to the IN pin of the sound sensor

int lastState = HIGH;  // the previous state from the input pin
int currentState; 
int decibel;         // the current reading from the input pin

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // initialize the ESP32's pin as an input
  pinMode(SENSOR_PIN, INPUT);
  // initialize the ESP32's pin as an output
  pinMode(BUZZER_PIN, OUTPUT);
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
