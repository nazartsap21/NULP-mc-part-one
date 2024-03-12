
#include <Arduino.h>
#include <LiquidCrystal.h>

#include "btn_handle.h"
#include "wifi_config.h"

#define SERIAL_SPD 115200


void setup()
{
  // initialize UART0
  Serial.begin(SERIAL_SPD);

  ledsInit();
  btnInit();
  initWiFi();

}

void loop()
{
  while (Serial.available())
  {
    uint8_t inByte = Serial.read();
    if (inByte == 0xB0) //176
    {
      btnChangedFlag = true;
      btnPressed = true;
      serialInFlag = true;
    }
    else
    {
      //could be other command or junk data 
    }
  }
  
  btnLedsHandle();
}
