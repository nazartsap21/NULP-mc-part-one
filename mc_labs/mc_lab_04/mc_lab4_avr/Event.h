
#ifndef Event_h
#define Event_h

#include <Arduino.h>
#include <inttypes.h>

typedef void (*func_callback_t)(void*);

enum EventType
{
  EVENT_NONE,
  EVENT_EVERY,
  EVENT_OSCILLATE
};

class Event
{

public:
  Event(void)
  {
    eventType = EVENT_NONE;
  }

  void update(uint32_t now=millis())
  {
    if (now - lastEventTime >= period)
	{
		switch (eventType)
		{
			case EVENT_EVERY:
                if (callback != NULL)
                {
                    callback(void);
                }
				break;

			case EVENT_OSCILLATE:
				pinState = ! pinState;
				digitalWrite(pin, pinState);
				break;
		}
		lastEventTime = now;
		count++;
	}
	if (repeatCount > -1 && count >= repeatCount)
	{
		eventType = EVENT_NONE;
	}
  }

  int8_t eventType = EVENT_NONE;
  uint32_t period;
  int16_t repeatCount = -1;
  uint8_t pin;
  uint8_t pinState;
  func_callback_t callback = NULL;
  uint32_t lastEventTime;
  int16_t count;
};

#endif