
#define AVR_ATmega2560
#include <Arduino.h>
#include "lcd.h"
#include "handleCommands.h"

#define UART_SPD 9600

const uint8_t ledPin = LED_BUILTIN;
const uint8_t buttonPin = 22;

void setupTimer()
{
   // Timer0  -- fast PWM
  TCCR0A = (1 << WGM01) | (1 << WGM00) | (1 << COM0A1) | (1 << COM0B1);
  //  TCCR0B = (0 << CS02) | (1 << CS01) | (1 << CS00); // prescaler=
  TCCR0B = (0 << CS02) | (1 << CS01) | (0 << CS00); // prescaler=
}

void setup()
{

  Serial.begin(UART_SPD);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  pinMode(buttonPin, INPUT_PULLUP);

  setupTimer();

  FAN_A = 0;
  FAN_B = 0;
  // OC_Pins
  DDRB |= 1 << 7;
  DDRG |= 1 << 5;
}


void loop()
{
  while (Serial.available())
  {
    uint8_t inByte = Serial.read();
    if (isCommand)
    {
      handleFanACommand(inByte);
      handleFanBCommand(inByte);
      handleOCRCommand(inByte);
    }
    else
    {
      handleOCRCommand(inByte);
    }
  }
}

#if 0
void loop()
{
  while(Serial.available())
  {
    uint8_t inByte = Serial.read();

    if (isCommand)
    {

      if (inByte == START_FAN_A)
      {
        FAN_A = pwmValue_A;
        isFanAStarted = true;
        lcd.setCursor(0, 0);
        lcd.print("StartA OCRA=");
        lcd.print(FAN_A);
        lcd.print("   ");
      }
  
      else if (inByte == STOP_FAN_A)
      {
        FAN_A = 0;
        isFanAStarted = false;
        lcd.setCursor(0, 0);
        lcd.print("StopA  OCRA=");
        lcd.print(FAN_A);
        lcd.print("   ");
      }
      // OCR_A command
      else if (inByte == OCR_A_COMMAND)
      {
        isCommand = false;
        command = inByte;
      }
     
      if (inByte == START_FAN_B)
      {
        FAN_B = pwmValue_B;
        isFanBStarted = true;
        lcd.setCursor(0, 1);
        lcd.print("StartB OCRB=");
        lcd.print(FAN_B);
        lcd.print("   ");
      }
      // Stop FAN_B
      else if (inByte == STOP_FAN_B)
      {
        FAN_B = 0;
        isFanBStarted = false;
        lcd.setCursor(0, 1);
        lcd.print("StopB  OCRB=");
        lcd.print(FAN_B);
        lcd.print("   ");
      }
      // OCR_B command
      else if (inByte == OCR_B_COMMAND)
      {
        isCommand = false;
        command = inByte;
      }
    }
    else
    {
      if (command == OCR_A_COMMAND)
      {
        
        isCommand = true;
        pwmValue_A = inByte;
        if (isFanAStarted)
        {
          
          FAN_A = pwmValue_A;
          lcd.setCursor(7, 0);
          lcd.print("OCRA=");
          lcd.print(FAN_A);
          lcd.print("  ");
        }
      }
      //---------------------------
      if (command == OCR_B_COMMAND)
      {
        isCommand = true;
        pwmValue_B = inByte;
        if (isFanBStarted)
        {
          FAN_B = pwmValue_B;
          lcd.setCursor(7, 1);
          lcd.print("OCRB=");
          lcd.print(FAN_B);
          lcd.print("  ");
        }
      }
    }
  }
}
#endif