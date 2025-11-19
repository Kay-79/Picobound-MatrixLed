#include "Display.h"

#define DATA_PIN 10
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define BRIGHTNESS 5
#define SERPENTINE_LAYOUT true
#define FLIP_HORIZONTAL false
#define FLIP_VERTICAL true

CRGB leds[NUM_LEDS];

CRGB pico8_palette[16] = {
    CRGB(0, 0, 0), CRGB(29, 43, 83), CRGB(126, 37, 83), CRGB(0, 135, 81),
    CRGB(171, 82, 54), CRGB(95, 87, 79), CRGB(194, 195, 199), CRGB(255, 241, 232),
    CRGB(255, 0, 77), CRGB(255, 163, 0), CRGB(255, 236, 39), CRGB(0, 228, 54),
    CRGB(41, 173, 255), CRGB(131, 118, 156), CRGB(255, 119, 168), CRGB(255, 204, 170)};

namespace
{

    static inline uint16_t XY(uint8_t x, uint8_t y)
    {
        if (FLIP_HORIZONTAL)
            x = MATRIX_WIDTH - 1 - x;
        if (FLIP_VERTICAL)
            y = MATRIX_HEIGHT - 1 - y;
        uint16_t i = SERPENTINE_LAYOUT && (y & 1)
                         ? y * MATRIX_WIDTH + (MATRIX_WIDTH - 1 - x)
                         : y * MATRIX_WIDTH + x;
        return i < NUM_LEDS ? i : 0;
    }

    String g_lastFrame;

    void renderFrame(const String &pixelString)
    {
        for (uint8_t y = 0; y < MATRIX_HEIGHT; y++)
        {
            for (uint8_t x = 0; x < MATRIX_WIDTH; x++)
            {
                int pos = y * MATRIX_WIDTH + x;
                char hexChar = pos < pixelString.length() ? pixelString[pos] : '0';
                int colorIndex = (hexChar >= 'a') ? 10 + (hexChar - 'a') : (hexChar >= 'A') ? 10 + (hexChar - 'A')
                                                                                            : hexChar - '0';
                leds[XY(x, y)] = pico8_palette[colorIndex & 0x0F];
            }
        }
        FastLED.show();
    }
} // namespace

void displayInit()
{
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.clear();
    FastLED.show();
    g_lastFrame = "";
}

void showTestPattern()
{
    fill_rainbow(leds, NUM_LEDS, 0, 7);
    FastLED.show();
}

void drawPixelString(const String &pixelString)
{
    g_lastFrame = pixelString;
    renderFrame(pixelString);
}

bool displayRefreshLastFrame()
{
    if (g_lastFrame.length() >= NUM_LEDS)
    {
        renderFrame(g_lastFrame);
        return true;
    }
    return false;
}

void displaySetBrightness(uint8_t brightness)
{
    FastLED.setBrightness(brightness);
    if (!displayRefreshLastFrame())
    {
        FastLED.show();
    }
}
