#define BLYNK_PRINT Serial
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
#include "SPI.h"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

SSD1306  display(0x3c, D1, D2);


// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "";

// Your WiFi credentials.
// Set password to "" for open networks.

char ssid[] = "AP";
char pass[] = "12345678";

const int CLOCK_PIN = D5;
const int DATA_PIN = D6;
int BIT_COUNT = 29;

//BLYNK_READ(V0)
//{
//  unsigned long reading;
//  Blynk.virtualWrite(V0,reading);
//
//}

BLYNK_WRITE(V1)
{
  int pinValue = param.asInt();
  BIT_COUNT=pinValue;
}


void setup() {
  //setup our pins
  pinMode(DATA_PIN, INPUT);
  pinMode(CLOCK_PIN, OUTPUT);

  //give some default values
  digitalWrite(CLOCK_PIN, HIGH);
  Blynk.begin(auth, ssid, pass);

  Serial.begin(115200);
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
}


void loop() {
  Blynk.run();
  unsigned long reading = readPosition();

  Serial.print("Reading: ");
  Serial.println(reading);
  Blynk.virtualWrite(V0,reading);
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "KODEERING:" + String(BIT_COUNT));

  display.setFont(ArialMT_Plain_10);
  display.drawString(50, 52, String(reading));
  // write the buffer to the display
  display.display();
  delay(1);
}

//read the current angular position
int readPosition() {
  unsigned long graysample = shiftIn(DATA_PIN, CLOCK_PIN, BIT_COUNT);
  delayMicroseconds(100); // Clock must be high for 20 microseconds before a new sample can be taken
  unsigned long binarysample = grayToBinary32(graysample);
  return (binarysample);
}

//read in a byte of data from the digital input of the board.
unsigned long shiftIn(const int data_pin, const int clock_pin, const int bit_count) {
  unsigned long data = 0;

  for (int i=0; i<bit_count; i++) {
    data <<= 1;
    digitalWrite(clock_pin, LOW);
    delayMicroseconds(1);
    digitalWrite(clock_pin, HIGH);
    delayMicroseconds(1);

    data |= digitalRead(data_pin);
  }  
  return data;
}

unsigned int grayToBinary32(unsigned int num)
{
 num = num ^ (num >> 16);
 num = num ^ (num >> 8);
 num = num ^ (num >> 4);
 num = num ^ (num >> 2);
 num = num ^ (num >> 1);
 return num;
}
