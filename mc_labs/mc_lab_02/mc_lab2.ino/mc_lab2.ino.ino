
#include <Arduino.h>


#define GREEN_LED 4     // esp8266 D2  
#define YELLOW_LED 0    // esp8266 D3
#define RED_LED 2       // esp8266 D4 builtin led

#define BUTTON_BLACK 14 // esp8266 D5
#define BUTTON_WHITE 12 // esp8266 D6

#define ON_DELAY_TIMER 250
#define OFF_DELAY_TIMER 400

#include "wifi.h"

void setup() 
{
  Serial.begin(115200);

  // put your setup code here, to run once:
  pinMode(GREEN_LED, OUTPUT);
  digitalWrite(GREEN_LED, LOW);
  pinMode(YELLOW_LED,OUTPUT);
  digitalWrite(YELLOW_LED, LOW);
  pinMode(RED_LED,   OUTPUT);
  digitalWrite(RED_LED, LOW);

  setupWifi();

  pinMode(BUTTON_BLACK, INPUT);
  pinMode(BUTTON_WHITE, INPUT);
}

void clickHandler()
{

  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        ledState = !ledState;
        //todo for 3 leds
      }
    }
  }

  // set the LED:
  digitalWrite(output, ledState);

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;


  //easy and bad way 
  if (digitalRead(BUTTON_WHITE) == HIGH)
  {
    digitalWrite(GREEN_LED, HIGH);
    delay(ON_DELAY_TIMER);
    digitalWrite(GREEN_LED, LOW);
    delay(OFF_DELAY_TIMER);

    digitalWrite(YELLOW_LED, HIGH);
    delay(ON_DELAY_TIMER);
    digitalWrite(YELLOW_LED, LOW);
    delay(OFF_DELAY_TIMER);

    digitalWrite(RED_LED, HIGH);
    delay(ON_DELAY_TIMER);
    digitalWrite(RED_LED, LOW);
    delay(OFF_DELAY_TIMER);
  }

}

void loop() 
{
   clickHandler();
  
}
