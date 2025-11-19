#include "Button.h"

void initButton(ButtonState &state, uint8_t pin)
{
    pinMode(pin, INPUT_PULLUP);
    state.reading = digitalRead(pin);
    state.stable = state.reading;
    state.lastDebounceMs = millis();
    state.pressActive = false;
}

bool buttonReleased(ButtonState &state, uint8_t pin, unsigned long now, unsigned long debounceMs)
{
    int reading = digitalRead(pin);
    if (reading != state.reading)
    {
        state.lastDebounceMs = now;
        state.reading = reading;
    }

    if ((now - state.lastDebounceMs) > debounceMs)
    {
        if (state.stable == HIGH && state.reading == LOW)
        {
            state.pressActive = true;
        }
        else if (state.stable == LOW && state.reading == HIGH)
        {
            bool wasPressed = state.pressActive;
            state.pressActive = false;
            state.stable = state.reading;
            return wasPressed;
        }
        state.stable = state.reading;
    }

    return false;
}
