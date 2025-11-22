#include <Arduino.h>
#include "AppConfig.h"
#include "Blockchain.h"
#include "Display.h"
#include "Button.h"
#include "Network.h"

namespace
{
    String g_image;
    ButtonState g_decButton;
    ButtonState g_incButton;
    uint8_t g_brightness = AppConfig::Brightness::DEFAULT_LEVEL;
    unsigned long g_lastFetchMs = 0;

    void applyBrightness()
    {
        Serial.printf("Applying brightness: %u\n", g_brightness);
        displaySetBrightness(g_brightness);
    }

    void initializeButtons()
    {
        initButton(g_decButton, AppConfig::Buttons::DECREASE_PIN);
        initButton(g_incButton, AppConfig::Buttons::INCREASE_PIN);
    }

    void adjustBrightness(int delta)
    {
        int next = static_cast<int>(g_brightness) + delta;
        if (next < AppConfig::Brightness::MIN_LEVEL)
        {
            next = AppConfig::Brightness::MIN_LEVEL;
        }
        else if (next > AppConfig::Brightness::MAX_LEVEL)
        {
            next = AppConfig::Brightness::MAX_LEVEL;
        }

        if (next != g_brightness)
        {
            g_brightness = static_cast<uint8_t>(next);
            applyBrightness();
        }
    }

    void handleBrightnessInput(unsigned long now)
    {
        if (buttonReleased(g_decButton, AppConfig::Buttons::DECREASE_PIN, now, AppConfig::Buttons::DEBOUNCE_MS))
        {
            adjustBrightness(-AppConfig::Brightness::STEP);
        }

        if (buttonReleased(g_incButton, AppConfig::Buttons::INCREASE_PIN, now, AppConfig::Buttons::DEBOUNCE_MS))
        {
            adjustBrightness(AppConfig::Brightness::STEP);
        }
    }

    bool isValidImage(const String &image)
    {
        size_t payloadLength = image.length();
        if (image.startsWith("0x") && payloadLength >= 2)
        {
            payloadLength -= 2;
        }

        return payloadLength == AppConfig::Blockchain::PIXEL_HEX_CHARS_PER_FRAME ||
               payloadLength == AppConfig::Blockchain::PIXEL_HEX_CHARS_PER_FRAME * AppConfig::Blockchain::MAX_ANIMATION_FRAMES;
    }

    String normalizePixelString(const String &image)
    {
        String sanitized = image;
        if (sanitized.startsWith("0x"))
        {
            sanitized.remove(0, 2);
        }
        return sanitized;
    }

    void renderImageOrFallback(const String &image)
    {
        if (isValidImage(image))
        {
            drawPixelString(normalizePixelString(image));
        }
        else
        {
            Serial.println("Image payload invalid, displaying test pattern.");
            showTestPattern();
        }
    }

    bool shouldFetchImage(unsigned long now)
    {
        return g_lastFetchMs == 0 || (now - g_lastFetchMs) >= AppConfig::Timing::FETCH_INTERVAL_MS;
    }

    void fetchAndDisplayImage()
    {
        Serial.println("Fetching image...");
        g_image = getPicoboundImage(AppConfig::Blockchain::RPC_URL,
                                    AppConfig::Blockchain::CONTRACT_ADDRESS,
                                    AppConfig::Blockchain::TOKEN_ID);
        Serial.printf("Image data length: %d\n", g_image.length());
        if (g_image.length() > 0)
        {
            Serial.println(g_image);
        }
        renderImageOrFallback(g_image);
    }
} // namespace

void setup()
{
    Serial.begin(AppConfig::SERIAL_BAUD);
    Serial.println("Starting Picobound LED Matrix...");

    displayInit();
    showTestPattern();
    applyBrightness();
    initializeButtons();

    if (!connectWiFi(AppConfig::Wifi::SSID, AppConfig::Wifi::PASSWORD))
    {
        ESP.restart();
    }
}

void loop()
{
    unsigned long now = millis();
    handleBrightnessInput(now);
    displayTick(now);

    if (shouldFetchImage(now))
    {
        g_lastFetchMs = now;
        fetchAndDisplayImage();
    }

    delay(1);
}
