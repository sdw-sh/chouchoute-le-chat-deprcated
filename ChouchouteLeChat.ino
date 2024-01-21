#include <WiFi.h>
//https://randomnerdtutorials.com/esp32-ntp-client-date-time-arduino-ide/
// https://github.com/taranais/NTPClient
#include <NTPClient.h>
#include <WiFiUdp.h>

#include "NetworkCredentials.h"


const char* ssid = SSID;
const char* networkPwd = NETWORKPWD;



int ledPin = 2;
const int resistorPin = 34;
int resistorValue = 0;
int lastValue = 0;

void setup() {
 pinMode(ledPin, OUTPUT);
 Serial.begin(115200);
 for (int i = 0; i < 5; i++) {
  digitalWrite(ledPin, HIGH);
  delay(i * 50);
  digitalWrite(ledPin, LOW); 
  delay(i * 50);
 }
}
void loop() {

  resistorValue = analogRead(resistorPin);
  if (resistorValue > 1000) { //lastValue > resistorValue + 50 || lastValue < resistorValue - 50 
    digitalWrite(ledPin, HIGH); 
  } else {
    digitalWrite(ledPin, LOW); 
  }
  lastValue = resistorValue;
  Serial.println(resistorValue);
  delay(500);
}

// https://randomnerdtutorials.com/esp32-adc-analog-read-arduino-ide/
// https://learn.adafruit.com/force-sensitive-resistor-fsr/using-an-fsr