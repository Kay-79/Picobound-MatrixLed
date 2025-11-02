#pragma once
#include <FastLED.h>

#define MATRIX_WIDTH 16
#define MATRIX_HEIGHT 16
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)

extern CRGB leds[NUM_LEDS];

void displayInit();
void showTestPattern();
void drawPixelString(const String &pixelString);
