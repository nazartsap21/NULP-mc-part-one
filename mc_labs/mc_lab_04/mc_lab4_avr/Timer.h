#ifndef Timer_h
#define Timer_h

#include "Event.h"

#define MAX_NUMBER_OF_EVENTS (10)

#define TIMER_NOT_AN_EVENT (-2)
#define NO_TIMER_AVAILABLE (-1)

class Timer
{

public:
    Timer(void)
    {
    }  

    int8_t every(uint32_t period, func_callback_t callback, int16_t repeatCount = -1)
    {
        int8_t i = findFreeEventIndex();
        if (i == -1)
            return -1;

        _events[i].eventType = EVENT_EVERY;
        _events[i].period = period;
        _events[i].repeatCount = repeatCount;
        if (callback != NULL)
        {
            _events[i].callback = callback;
        }
        _events[i].lastEventTime = millis();
        _events[i].count = 0;
        return i;
    }

    int8_t after(uint32_t duration, func_callback_t callback)
    {
        return every(duration, callback, 1);
    }

    int8_t oscillate(uint8_t pin, uint32_t period, uint8_t startingValue, int16_t repeatCount = -1)
    {
        int8_t i = findFreeEventIndex();
        if (i == NO_TIMER_AVAILABLE)
            return NO_TIMER_AVAILABLE;

        _events[i].eventType = EVENT_OSCILLATE;
        _events[i].pin = pin;
        _events[i].period = period;
        _events[i].pinState = startingValue;
        digitalWrite(pin, startingValue);
        _events[i].repeatCount = repeatCount * 2; // full cycles not transitions
        _events[i].lastEventTime = millis();
        _events[i].count = 0;
        return i;
    }

    /**
     * This method will generate a pulse of !startingValue, occuring period after the
     * call of this method and lasting for period. The Pin will be left in !startingValue.
     */
    int8_t pulse(uint8_t pin, uint32_t period, uint8_t startingValue)
    {
        return oscillate(pin, period, startingValue, 1); // once
    }

    /**
     * This method will generate a pulse of pulseValue, starting immediately and of
     * length period. The pin will be left in the !pulseValue state
     */
    int8_t pulseImmediate(uint8_t pin, uint32_t period, uint8_t pulseValue)
    {
        int8_t id(oscillate(pin, period, pulseValue, 1));
        // now fix the repeat count
        if (id >= 0 && id < MAX_NUMBER_OF_EVENTS)
        {
            _events[id].repeatCount = 1;
        }
        return id;
    }

    void stop(int8_t id)
    {
        if (id >= 0 && id < MAX_NUMBER_OF_EVENTS)
        {
            _events[id].eventType = EVENT_NONE;
        }
    }

    // void update(void)
    // {
    //     update(millis());
    // }

    void update(uint32_t now=millis())
    {
        for (int8_t i = 0; i < MAX_NUMBER_OF_EVENTS; i++)
        {
            if (_events[i].eventType != EVENT_NONE)
            {
                _events[i].update(now);
            }
        }
    }

protected:
    Event _events[MAX_NUMBER_OF_EVENTS];
    int8_t findFreeEventIndex(void)
    {
        for (int8_t i = 0; i < MAX_NUMBER_OF_EVENTS; i++)
        {
            if (_events[i].eventType == EVENT_NONE)
            {
                return i;
            }
        }
        return NO_TIMER_AVAILABLE;
    }
};

#endif