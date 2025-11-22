#include "Display.h"
#include "AppConfig.h"

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

    struct AnimationState
    {
        bool active = false;
        uint8_t frameCount = 0;
        uint8_t currentIndex = 0;
        unsigned long nextSwitchMs = 0;
        uint16_t intervalMs = AppConfig::Display::BASE_ANIMATION_INTERVAL_MS;
        String frames[AppConfig::Blockchain::MAX_ANIMATION_FRAMES];
    };

    String g_lastFrame;
    AnimationState g_animation;
    uint8_t g_animationSpeed = AppConfig::Display::DEFAULT_ANIMATION_SPEED;

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

    bool populateQuadrantFrames(const String &pixelString)
    {
        constexpr uint8_t SOURCE_DIM = MATRIX_WIDTH * 2;
        const size_t expectedLength = NUM_LEDS * AppConfig::Blockchain::MAX_ANIMATION_FRAMES;
        if (pixelString.length() != expectedLength)
        {
            return false;
        }

        for (uint8_t i = 0; i < AppConfig::Blockchain::MAX_ANIMATION_FRAMES; ++i)
        {
            g_animation.frames[i].remove(0);
            g_animation.frames[i].reserve(NUM_LEDS);
        }

        size_t idx = 0;
        for (uint8_t y = 0; y < SOURCE_DIM; ++y)
        {
            for (uint8_t x = 0; x < SOURCE_DIM; ++x)
            {
                char pixel = pixelString[idx++];
                uint8_t quadrant = (y >= MATRIX_HEIGHT ? 2 : 0) + (x >= MATRIX_WIDTH ? 1 : 0);
                g_animation.frames[quadrant] += pixel;
            }
        }

        for (uint8_t i = 0; i < AppConfig::Blockchain::MAX_ANIMATION_FRAMES; ++i)
        {
            if (g_animation.frames[i].length() != NUM_LEDS)
            {
                return false;
            }
        }
        return true;
    }

    void resetAnimation()
    {
        g_animation.active = false;
        g_animation.frameCount = 0;
        g_animation.currentIndex = 0;
        g_animation.nextSwitchMs = 0;
    }

    void showCurrentFrame()
    {
        if (g_animation.frameCount == 0)
        {
            return;
        }

        const String &frame = g_animation.frames[g_animation.currentIndex];
        g_lastFrame = frame;
        renderFrame(frame);
    }
} // namespace

void displayInit()
{
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.clear();
    FastLED.show();
    resetAnimation();
    displaySetAnimationSpeed(AppConfig::Display::DEFAULT_ANIMATION_SPEED);
    g_lastFrame = "";
}

void showTestPattern()
{
    drawPixelString("7700000000000077770000000000007700000000000000000009999999999000009990999909990000999009900999000099990000999900009999000099990000999009900999000099909999099900770999999999907777700099990007777770009999000777770ccc0000ccc07700cccccccccccc0000cccccccccccc00");
    FastLED.show();
}

void drawPixelString(const String &pixelString)
{
    resetAnimation();
    size_t payloadLength = pixelString.length();

    if (payloadLength == NUM_LEDS)
    {
        g_animation.frameCount = 1;
        g_animation.frames[0] = pixelString;
        g_animation.currentIndex = 0;
        showCurrentFrame();
        return;
    }

    if (payloadLength == NUM_LEDS * AppConfig::Blockchain::MAX_ANIMATION_FRAMES)
    {
        if (populateQuadrantFrames(pixelString))
        {
            g_animation.active = true;
            g_animation.frameCount = AppConfig::Blockchain::MAX_ANIMATION_FRAMES;
            g_animation.currentIndex = 0;
            showCurrentFrame();
            g_animation.nextSwitchMs = millis() + g_animation.intervalMs;
            return;
        }
    }

    g_animation.frameCount = 1;
    if (payloadLength >= NUM_LEDS)
    {
        g_animation.frames[0] = pixelString.substring(0, NUM_LEDS);
    }
    else
    {
        g_animation.frames[0] = pixelString;
    }
    g_animation.currentIndex = 0;
    showCurrentFrame();
}

void displayTick(unsigned long now)
{
    if (!g_animation.active || g_animation.frameCount <= 1)
    {
        return;
    }

    if (g_animation.nextSwitchMs == 0)
    {
        g_animation.nextSwitchMs = now + g_animation.intervalMs;
        return;
    }

    if (now >= g_animation.nextSwitchMs)
    {
        g_animation.currentIndex = (g_animation.currentIndex + 1) % g_animation.frameCount;
        showCurrentFrame();
        g_animation.nextSwitchMs = now + g_animation.intervalMs;
    }
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

void displaySetAnimationSpeed(uint8_t speed)
{
    if (speed == 0)
    {
        speed = 1;
    }

    g_animationSpeed = speed;
    uint16_t interval = AppConfig::Display::BASE_ANIMATION_INTERVAL_MS / speed;
    if (interval == 0)
    {
        interval = 1;
    }
    g_animation.intervalMs = interval;

    if (g_animation.active)
    {
        g_animation.nextSwitchMs = millis() + g_animation.intervalMs;
    }
}
