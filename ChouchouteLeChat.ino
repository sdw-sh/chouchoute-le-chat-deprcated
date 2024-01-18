

int ledPin = 2;
const int resistorPin = 34;
int resistorValue = 0;
int lastValue = 0;

void setup() {
 pinMode(ledPin, OUTPUT);
 Serial.begin(115200);
 delay(1000);
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
 //digitalWrite(ledPin, HIGH);
 //delay(200);
 //digitalWrite(ledPin, LOW);
 //delay(200);
}

// https://randomnerdtutorials.com/esp32-adc-analog-read-arduino-ide/
// https://learn.adafruit.com/force-sensitive-resistor-fsr/using-an-fsr