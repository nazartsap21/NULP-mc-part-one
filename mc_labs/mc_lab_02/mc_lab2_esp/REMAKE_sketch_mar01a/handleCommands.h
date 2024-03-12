#ifndef HANDLE_COMMANDS_H
#define HANDLE_COMMANDS_H

#include <Arduino.h>
#include "lcd.h"

enum CommandType
{
    NOT_DEFINED = 0,
    START_FAN_A = 0xA1,
    STOP_FAN_A = 0xA2,
    OCR_A_COMMAND = 0xA3,
    START_FAN_B = 0xB1,
    STOP_FAN_B = 0xB2,
    OCR_B_COMMAND = 0xB3
};

CommandType command = NOT_DEFINED;

bool isCommand = true;
bool modeFanA = false;
bool modeFanB = false;

unsigned char pwmValue_A = 0;
unsigned char pwmValue_B = 0;

void handleFanACommand(uint8_t inByte)
{
    switch (inByte)
    {
    case START_FAN_A:
        FAN_A = pwmValue_A;
        modeFanA = true;
        lcdPrintFanA(modeFanA);
        break;

    case STOP_FAN_A:
        FAN_A = 0;
        modeFanA = false;
        lcdPrintFanA(modeFanA);
        break;

    case OCR_A_COMMAND:
        isCommand = false;
        command = inByte; //(CommandType)
        break;

    default:
        break;
    }
}

void handleFanBCommand(int inByte)
{
    switch (inByte)
    {
    case START_FAN_B:
        FAN_B = pwmValue_B;
        modeFanB = true;
        lcdPrintFanB(modeFanB);
        break;

    case STOP_FAN_B:
        FAN_B = 0;
        modeFanB = false;
        lcdPrintFanB(modeFanB);
        break;

    case OCR_B_COMMAND:
        isCommand = false;
        command = inByte; //(CommandType)
        break;
        
    default:
        break;  
    }
}

void handleOCRCommand(uint8_t inByte)
{
    switch (command)
    {
    case OCR_A_COMMAND:
        isCommand = true;
        pwmValue_A = inByte;
        if (modeFanA)
        {
            FAN_A = pwmValue_A;
            lcdPrintFanCmdValueA();
        }
        break;

    case OCR_B_COMMAND:
        isCommand = true;
        pwmValue_B = inByte;
        if (modeFanB)
        {
            FAN_B = pwmValue_B;
            lcdPrintFanCmdValueB();
        }
        break;

    default:
        break;
    }
}

#endif