#include "unix_timestamp.h"

#include <ESP32Time.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SPIFFS

#include "FS.h"
#include "SPIFFS.h"

/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me-no-dev/arduino-esp32fs-plugin */
#define FORMAT_SPIFFS_IF_FAILED true

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

bool fileExists(fs::FS &fs, const char * path){
    Serial.printf("Checking if file %s exists\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("  It does not");
        return false;
    }
    Serial.println("  It does");
    return true;
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("- failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("- message appended");
    } else {
        Serial.println("- append failed");
    }
    file.close();
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("- file deleted");
    } else {
        Serial.println("- delete failed");
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ESP32Time rtc(60 * 60);

const int ledPin = 33;
const int relaisPin = 32;
const int resistorPin = 34;
int resistorValue = 0;
bool lastDetection = false;

// Define NTP Client to get time


// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;


void setup() {
  Serial.begin(115200);
  Serial.println("====================================================================================");
  Serial.println("==                                    STARTING                                    ==");
  Serial.println("====================================================================================");

  rtc.setTime(__DATE_TIME_UNIX__ - 60 * 60); // it seems __DATE_TIME_UNIX__ is naive, not considering the offset, so I have to correct it 
  Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S"));

  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
      Serial.println("SPIFFS Mount Failed");
      return;
  }

  if (!fileExists(SPIFFS, "/leChat.log")) {
    writeFile(SPIFFS, "/leChat.log", "Start of log\r\n");
  }

  listDir(SPIFFS, "/", 0);

  Serial.println("------------------------------------------------------------------------------------");
  readFile(SPIFFS, "/leChat.log");
  Serial.println("------------------------------------------------------------------------------------");

  appendFile(SPIFFS, "/leChat.log", rtc.getTime("%A, %B %d %Y %H:%M:%S").c_str());
  appendFile(SPIFFS, "/leChat.log", "  |  Appareil démarré.\r\n");

  pinMode(relaisPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);

  delay(5000);

  digitalWrite(ledPin, LOW);
}

void loop() {
  resistorValue = analogRead(resistorPin);
  if (resistorValue > 1000) { 
    if (!lastDetection) {
      appendFile(SPIFFS, "/leChat.log", rtc.getTime("%A, %B %d %Y %H:%M:%S").c_str());
      appendFile(SPIFFS, "/leChat.log", "  |  Le chat est arrivé.\r\n");
      lastDetection = true;
    }
    digitalWrite(relaisPin, HIGH); 
    digitalWrite(ledPin, HIGH); 
  } else {
    if (lastDetection) {
      appendFile(SPIFFS, "/leChat.log", rtc.getTime("%A, %B %d %Y %H:%M:%S").c_str());
      appendFile(SPIFFS, "/leChat.log", "  |  Le chat est parti.\r\n");
      lastDetection = false;
    }
    digitalWrite(relaisPin, LOW); 
    digitalWrite(ledPin, LOW); 
  }
  Serial.println(resistorValue);
  delay(5000);
}

// https://randomnerdtutorials.com/esp32-adc-analog-read-arduino-ide/
// https://learn.adafruit.com/force-sensitive-resistor-fsr/using-an-fsr