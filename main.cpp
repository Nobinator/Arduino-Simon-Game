#include <Arduino.h>

const int iRed = 5;
const int iBlue = 4;
const int iYellow = 2;
const int iGreen = 3;

const int oRed = 12;
const int oBlue = 11;
const int oYellow = 9;
const int oGreen = 10;
void setup(){
  pinMode(oRed, OUTPUT);
  pinMode(oBlue, OUTPUT);
  pinMode(oYellow, OUTPUT);
  pinMode(oGreen, OUTPUT);

  pinMode(iRed, INPUT_PULLUP);
  pinMode(iBlue, INPUT_PULLUP);
  pinMode(iYellow, INPUT_PULLUP);
  pinMode(iGreen, INPUT_PULLUP);
  //Serial.begin(9600);
}

int revertedDigitalRead(uint8_t pin){
  if(digitalRead(pin) == HIGH) return LOW;
  return HIGH;
}

void loop(){
  digitalWrite(oRed, revertedDigitalRead(iRed));
  digitalWrite(oBlue, revertedDigitalRead(iBlue));
  digitalWrite(oYellow, revertedDigitalRead(iYellow));
  digitalWrite(oGreen, revertedDigitalRead(iGreen));
  delay(100);
}