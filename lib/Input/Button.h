#pragma once
#include <Arduino.h>

struct ButtonState
{
    bool reading = HIGH;
    bool stable = HIGH;
    unsigned long lastDebounceMs = 0;
    bool pressActive = false;
};

void initButton(ButtonState &state, uint8_t pin);
bool buttonReleased(ButtonState &state, uint8_t pin, unsigned long now, unsigned long debounceMs);
