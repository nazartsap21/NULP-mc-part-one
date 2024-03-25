#ifndef LCD
#define LCD

#define ROWS 2
#define COLS 16
#include <LiquidCrystal.h>

#define FAN_A OCR0A // 13
#define FAN_B OCR0B // 4

const int rs = 5, rw = 6, en = 7, d4 = 8, d5 = 9, d6 = 10, d7 = 11;
LiquidCrystal lcd(rs, rw, en, d4, d5, d6, d7);

void lcdInit()
{
    lcd.begin(COLS, ROWS);
    lcd.home();
    lcd.print("Start Client");
}

void lcdPrintFanA(bool currentMode)
{
    lcd.setCursor(0, 0);
    if (currentMode)
    {
        lcd.print("StartA ");
    }
    else
    {
        lcd.print("StopA  ");
    }
    lcd.print("OCRA=");
    lcd.print(FAN_A);
    lcd.print("   ");
}

void lcdPrintFanB(bool currentMode)
{
    lcd.setCursor(0, 1);
    if (currentMode)
    {
        lcd.print("StartB ");
    }
    else
    {
        lcd.print("StopB  ");
    }
    lcd.print("OCRB=");
    lcd.print(FAN_B);
    lcd.print("   ");
}

void lcdPrintFanCmdValueA()
{
    lcd.setCursor(7, 0);
    lcd.print("OCRA=");
    lcd.print(FAN_A);
    lcd.print("  ");
}

void lcdPrintFanCmdValueB()
{
    lcd.setCursor(7, 1);
    lcd.print("OCRB=");
    lcd.print(FAN_B);
    lcd.print("  ");
}

#endif