#include <FastLED.h>

// --- CẤU HÌNH LED (Giữ nguyên như code chính) ---
#define DATA_PIN 18
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 256
#define BRIGHTNESS 50
#define FRAMES_PER_SECOND 60

CRGB leds[NUM_LEDS];
uint8_t gHue = 0; // animated hue

void setup()
{
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
}

void loop()
{
  // Hiệu ứng cầu vồng (animated)
  fill_rainbow(leds, NUM_LEDS, gHue, 7);
  FastLED.show();

  // advance hue and control frame rate
  EVERY_N_MILLISECONDS(20) { gHue++; }
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}