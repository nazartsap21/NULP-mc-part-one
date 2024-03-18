#ifndef BTN_LEDS_HANDLE_H
#define BTN_LEDS_HANDLE_H

#include <Arduino.h>
// Set to true to define Relay as Normally Open (NO)
#define LED_MODE 1

// Assign GPIO to the relay
const int ledPin = D4; // Replace with the desired GPIO pin
const int btnPin = D5; // Replace with the desired GPIO pin

bool btnChangedFlag;
bool btnPressed;
bool btnReleased;

bool serialInFlag = false;

IRAM_ATTR void btnIsrHigh()
{
    btnChangedFlag = true;
    btnPressed = true;
}

IRAM_ATTR void btnIsrLow()
{
    btnChangedFlag = true;
    btnReleased = true;
}

void btnInit()
{
    // todo: make two functions for: leds and button
    pinMode(btnPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(btnPin), btnIsrHigh, RISING);
    attachInterrupt(digitalPinToInterrupt(btnPin), btnIsrLow, FALLING);
    btnChangedFlag = false;
    btnPressed = false;
    btnReleased = false;
}

void ledsInit()
{
    pinMode(ledPin, OUTPUT);
#if (LED_MODE == 1)
    digitalWrite(ledPin, HIGH);
#else
    digitalWrite(ledPin, LOW);
#endif
}

void btnLedsHandle()
{
    if (btnChangedFlag)
    {
        if (btnPressed)
        {
            digitalWrite(ledGPIO, LOW);
            if (!serialInFlag)
            {
                Serial.write(0xB0);
            }else{
                serialInFlag = false;
            }
            
            btnPressed = false;
        }
        if (btnReleased)
        {
            digitalWrite(ledGPIO, HIGH);
            if (!serialInFlag)
            {
                Serial.write(0xB1);
            }else
            {
                serialInFlag = false;
            }
            
            btnReleased = false;
        }
        btnChangedFlag = false;
    }
}

#endif //* BTN_LEDS_HANDLE_H