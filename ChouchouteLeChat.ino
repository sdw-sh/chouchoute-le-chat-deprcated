#include <WiFi.h>
// https://randomnerdtutorials.com/esp32-ntp-client-date-time-arduino-ide/
// https://github.com/taranais/NTPClient
#include <NTPClient.h>
// https://www.upesy.com/blogs/tutorials/how-to-connect-wifi-acces-point-with-esp32
#include <WiFiUdp.h>

#include "NetworkCredentials.h"


const char* ssid = SSID2;
const char* password = PASSWORD2;


const int ledPin = 16;
const int relaisPin = 2;
const int resistorPin = 34;
int resistorValue = 0;
int lastValue = 0;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;

String get_wifi_status(int status){
  switch(status){
    case WL_IDLE_STATUS:
    return "WL_IDLE_STATUS";
    case WL_SCAN_COMPLETED:
    return "WL_SCAN_COMPLETED";
    case WL_NO_SSID_AVAIL:
    return "WL_NO_SSID_AVAIL";
    case WL_CONNECT_FAILED:
    return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST:
    return "WL_CONNECTION_LOST";
    case WL_CONNECTED:
    return "WL_CONNECTED";
    case WL_DISCONNECTED:
    return "WL_DISCONNECTED";
  }
}


void setup() {
  
  pinMode(relaisPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  Serial.begin(115200);
  delay(1000);

  Serial.println("====================================================================================");
  Serial.println("==                                    STARTING                                    ==");
  Serial.println("====================================================================================");
  Serial.println(__DATE__);
  Serial.println(__TIME__);
  Serial.println(__TIMESTAMP__);

  /*
  for (int i = 0; i < 5; i++) {
    digitalWrite(relaisPin, HIGH);
    delay(i * 50);
    digitalWrite(relaisPin, LOW); 
    delay(i * 50);
  }
  */

 //// NTP ////////////////////////////////////////////////////////////////////////////////////////////////

  // Scan for networks
  //while (true) {
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
      Serial.print(n);
      Serial.println(" networks found");
      for (int i = 0; i < n; ++i) {
        // Print SSID and RSSI for each network found
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(WiFi.SSID(i));
        Serial.print(" (");
        Serial.print(WiFi.RSSI(i));
        Serial.print(")");
        Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
        delay(10);
      }
    }
    delay(1000);
  //}

  Serial.println("--- SSID: ---");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  int status = WL_IDLE_STATUS;
  int timeout_counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    status = WiFi.status();
    delay(500);
    Serial.println(get_wifi_status(status));
    timeout_counter++;
    if(timeout_counter >= 300 || status == WL_NO_SSID_AVAIL){
      ESP.restart();
    }
    //Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  digitalWrite(ledPin, LOW);

  // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  // timeClient.setTimeOffset(3600);

  /////////////////////////////////////////////////////////////////////////////////////////////////////////
}
void loop() {

  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }

    // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
  Serial.println(formattedDate);

  // Extract date
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  Serial.print("DATE: ");
  Serial.println(dayStamp);
  // Extract time
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  Serial.print("HOUR: ");
  Serial.println(timeStamp);

  resistorValue = analogRead(resistorPin);
  if (resistorValue > 1000) { //lastValue > resistorValue + 50 || lastValue < resistorValue - 50 
    digitalWrite(relaisPin, HIGH); 
  } else {
    digitalWrite(relaisPin, LOW); 
  }
  lastValue = resistorValue;
  Serial.println(resistorValue);
  delay(1000);
}

// https://randomnerdtutorials.com/esp32-adc-analog-read-arduino-ide/
// https://learn.adafruit.com/force-sensitive-resistor-fsr/using-an-fsr