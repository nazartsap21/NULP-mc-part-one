
#define __AVR_ATmega2560__
#include <Arduino.h>

const int buttonPin1 =  53;
const int buttonPin2 =  51;
int inByte;

void setup() {
  DDRA = 0xFF;
  PORTA = 0;
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  Serial.begin(9600);  //UART0
}
void loop() {
  if (Serial.available()) {
    inByte = Serial.read();
    if (inByte == 0xA1) { 
      //Algorithm 1
      PORTA = 1;
      while (PORTA) {
        delay(1000);
        PORTA = PORTA << 1;
      }
    } else if (inByte == 0xA2) {
      //Algorithm 2
      PORTA = B10000001;
      delay(1000);
      PORTA = B01000010;
      delay(1000);
      PORTA = B00100100;
      delay(1000);
      PORTA = B00011000;
      delay(1000);
      PORTA = 0b11111111;
    }
  }

  if (digitalRead(buttonPin1) == LOW) {
    Serial.write(0xB1);
    delay(500);
  }
  if (digitalRead(buttonPin2) == LOW) {
    Serial.write(0xB2);
    delay(500);
  }
}
