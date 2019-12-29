#include <Arduino.h>

const int iRed = 5;
const int iBlue = 4;
const int iYellow = 2;
const int iGreen = 3;

const int oRed = 12;
const int oBlue = 11;
const int oYellow = 9;
const int oGreen = 10;

bool listenMode;
bool repeatMode;

const int MAX_LENGTH_OF_SEQUENCE = 99;

int seq[MAX_LENGTH_OF_SEQUENCE] = { };

int repeatEndCursor = 0;
int cursor = 0;

// 0 = not pressed
// 1 = down
// 2 = hold
uint8_t input[5] = { };

void resetGameState(){
  // Fill dirty part of sequence with zeros
  for (int i = 0; i < repeatEndCursor + 1; i++){
    seq[i] = 0;
  }
  cursor = 0;
  repeatEndCursor = 0;
  listenMode = true;
  repeatMode = false;
  // Fill click buffer with false
  for (int i = 0; i < 5; i++){
    input[i] = 0;
  }
  
}

void setup(){
  pinMode(oRed, OUTPUT);
  pinMode(oBlue, OUTPUT);
  pinMode(oYellow, OUTPUT);
  pinMode(oGreen, OUTPUT);

  pinMode(iRed, INPUT_PULLUP);
  pinMode(iBlue, INPUT_PULLUP);
  pinMode(iYellow, INPUT_PULLUP);
  pinMode(iGreen, INPUT_PULLUP);
  Serial.begin(9600);

  resetGameState();
}

int revertedDigitalRead(uint8_t pin){
  if(digitalRead(pin) == HIGH) return LOW;
  return HIGH;
}



int seqCodeToOutPin(int code){
  if(code == 1) return oRed;
  if(code == 2) return oBlue;
  if(code == 3) return oGreen;
  if(code == 4) return oYellow;
  // Notify somehow that something goes wrong
  return 8;
}

int seqCodeToInPin(int code){
  if(code == 1) return iRed;
  if(code == 2) return iBlue;
  if(code == 3) return iGreen;
  if(code == 4) return iYellow;
  // Notify somehow that something goes wrong
  return 8;
}

void blink(int code){
  Serial.print(code);
  digitalWrite(seqCodeToOutPin(code), HIGH);
  delay(500);
  digitalWrite(seqCodeToOutPin(code), LOW);
  delay(250);
}

void updateInputs(){
  for (uint8_t code = 0; code < 5; code++){
    // get actual pin index
    int pin = seqCodeToInPin(code);
    // get state of pin
    int state = revertedDigitalRead(pin);
    // get stored state
    uint8_t buffer = input[code];
    if(state == HIGH){
      if(buffer == 0){
        input[code] = 1;
        continue;
      }
      if(buffer == 1){
        input[code] = 2;
        continue;
      }
      // if(buffer == 2) do nothing
    } else {
      // if(buffer == 0) do nothing
      if(buffer == 1 || buffer == 2){
        input[code] = 0;
      }
    }
  }  
}

bool IsClick(int code){
  return input[code] == 1;
}

void gameOver(int code){
  digitalWrite(seqCodeToOutPin(1), HIGH);
  digitalWrite(seqCodeToOutPin(2), HIGH);
  digitalWrite(seqCodeToOutPin(3), HIGH);
  digitalWrite(seqCodeToOutPin(4), HIGH);
  delay(500);
  digitalWrite(seqCodeToOutPin(code), LOW);
  delay(250);
  digitalWrite(seqCodeToOutPin(code), HIGH);
  delay(250);
  digitalWrite(seqCodeToOutPin(code), LOW);
  delay(250);
  digitalWrite(seqCodeToOutPin(code), HIGH);
  delay(500);
  resetGameState();
  digitalWrite(seqCodeToOutPin(1), LOW);
  digitalWrite(seqCodeToOutPin(2), LOW);
  digitalWrite(seqCodeToOutPin(3), LOW);
  digitalWrite(seqCodeToOutPin(4), LOW);
  delay(1000);

}

void renderState(){
  for (uint8_t i = 0; i < 5; i++){
    digitalWrite(seqCodeToOutPin(i), input[i] ? HIGH : LOW);
  }    
}

void loop(){

  if(listenMode){
    Serial.print("Listen : ");
    // Blink LEDs according to sequence
    for (int i = 0; i < MAX_LENGTH_OF_SEQUENCE; i++){
      // append element to the end of sequence, 
      // blink and then exit the loop
      if(seq[i] == 0){
        //Generate next sequence element
        int code = random(1,5);
        seq[i] = code;
        blink(code);
        repeatEndCursor = i;
        Serial.println();
        Serial.print("Sequence length = ");    
        Serial.println(repeatEndCursor + 1); 
        break;
      }
      blink(seq[i]);
    }
    listenMode = false;
    repeatMode = true;
    cursor = 0;
  }

  updateInputs();
  renderState();

  if (repeatMode){
    for (int code = 1; code < 5; code++){
      if(IsClick(code)){
        int expectedCode = seq[cursor];
        if(code == expectedCode){
          Serial.println("Correct");
          // If end of generated sequence is reached then enable 
          // listen mode and return to get to the next iteration step
          if(cursor == repeatEndCursor){
            listenMode = false;
            repeatMode = false;
            Serial.println("Success sequence");
            return;
          }
          Serial.print(repeatEndCursor - cursor);
          Serial.println(" steps left");
          cursor++;
          break;
        } else {
          Serial.print("Incorrect! Expected : ");
          Serial.print(expectedCode);
          Serial.print(". Actual : ");
          Serial.println(code);
          gameOver(expectedCode);
          return;
        }
      }      
    }
    delay(10);
  }

  if(!listenMode && !repeatMode){
    // ignore move to listenMode until any of buttons is pressed
    if(input[1] != 0 ) return;
    if(input[2] != 0 ) return;
    if(input[3] != 0 ) return;
    if(input[4] != 0 ) return;
    delay(1000);
    listenMode = true;
    Serial.println();
  }
}