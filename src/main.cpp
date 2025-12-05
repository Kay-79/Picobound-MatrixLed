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
    ButtonState g_resetButton;
    uint8_t g_brightness = AppConfig::Brightness::DEFAULT_LEVEL;
    unsigned long g_lastFetchMs = 0;

    // Dynamic avatar state
    String g_currentCollection = "";
    int g_currentTokenId = -1;
    unsigned long g_lastAvatarPollMs = 0;
    String g_userWallet = "";

    void applyBrightness()
    {
        Serial.printf("Applying brightness: %u\n", g_brightness);
        displaySetBrightness(g_brightness);
    }

    void initializeButtons()
    {
        initButton(g_decButton, AppConfig::Buttons::DECREASE_PIN);
        initButton(g_incButton, AppConfig::Buttons::INCREASE_PIN);
        initButton(g_resetButton, AppConfig::Buttons::RESET_PIN);
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
    void handleInput(unsigned long now)
    {
        if (buttonReleased(g_decButton, AppConfig::Buttons::DECREASE_PIN, now, AppConfig::Buttons::DEBOUNCE_MS))
        {
            adjustBrightness(-AppConfig::Brightness::STEP);
        }

        if (buttonReleased(g_incButton, AppConfig::Buttons::INCREASE_PIN, now, AppConfig::Buttons::DEBOUNCE_MS))
        {
            adjustBrightness(AppConfig::Brightness::STEP);
        }

        if (buttonReleased(g_resetButton, AppConfig::Buttons::RESET_PIN, now, AppConfig::Buttons::DEBOUNCE_MS))
        {
            Serial.println("Reset button pressed. Clearing settings and restarting...");
            resetNetworkSettings();
            ESP.restart();
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

    bool shouldFetchImage(unsigned long now)
    {
        return g_lastFetchMs == 0 || (now - g_lastFetchMs) >= AppConfig::Timing::FETCH_INTERVAL_MS;
    }

    void fetchAndDisplayImage();

    void pollAvatar(unsigned long now)
    {
        if (now - g_lastAvatarPollMs < AppConfig::Timing::AVATAR_POLL_INTERVAL_MS)
        {
            return;
        }
        g_lastAvatarPollMs = now;

        if (WiFi.status() != WL_CONNECTED)
            return;
        Serial.println("Polling primary avatar...");
        Avatar avatar = getPrimaryAvatar(AppConfig::Blockchain::RPC_URL,
                                         AppConfig::Blockchain::RESOLVER_ADDRESS,
                                         g_userWallet.c_str());

        if (avatar.isValid)
            if (avatar.isValid)
            {
                // Check if changed
                if (!avatar.collection.equalsIgnoreCase(g_currentCollection) || avatar.tokenId != g_currentTokenId)
                {
                    Serial.printf("Avatar changed! New: %s #%d\n", avatar.collection.c_str(), avatar.tokenId);
                    g_currentCollection = avatar.collection;
                    g_currentTokenId = avatar.tokenId;

                    // Force fetch immediately
                    fetchAndDisplayImage();
                    g_lastFetchMs = now; // Reset periodic timer
                }
            }
    }

    void fetchAndDisplayImage()
    {
        if (g_currentCollection.length() == 0 || g_currentTokenId < 0)
        {
            Serial.println("No avatar selected to fetch.");
            return;
        }

        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("WiFi disconnected. Attempting to reconnect...");
            WiFi.disconnect();
            WiFi.reconnect();
            return;
        }

        Serial.printf("Fetching image for %s #%d...\n", g_currentCollection.c_str(), g_currentTokenId);
        String fetchedImage = getPicoboundImage(AppConfig::Blockchain::RPC_URL,
                                                g_currentCollection.c_str(),
                                                g_currentTokenId);

        if (fetchedImage.length() > 0 && isValidImage(fetchedImage))
        {
            Serial.printf("Image fetched successfully. Length: %d\n", fetchedImage.length());
            g_image = fetchedImage;
            drawPixelString(normalizePixelString(g_image));
        }
        else
        {
            Serial.println("Fetch failed or returned invalid image.");
            if (g_image.length() > 0 && isValidImage(g_image))
            {
                Serial.println("Keeping previous valid image.");
            }
            else
            {
                Serial.println("No valid previous image. Displaying test pattern.");
                showTestPattern();
            }
        }
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

    // Check if DECREASE button is held down at boot to reset settings
    // (Button 8 is a strapping pin, so we use Button 10 for boot-time reset)
    if (digitalRead(AppConfig::Buttons::DECREASE_PIN) == LOW)
    {
        Serial.println("Decrease button held. Resetting network settings...");
        displaySetBrightness(50); // Visual feedback
        resetNetworkSettings();
        Serial.println("Settings reset. Restarting...");
        delay(1000);
        ESP.restart();
    }

    g_userWallet = setupNetwork();
    Serial.printf("Using wallet: %s\n", g_userWallet.c_str());
}

void loop()
{
    unsigned long now = millis();
    handleInput(now);
    displayTick(now);

    pollAvatar(now);

    if (shouldFetchImage(now))
    {
        g_lastFetchMs = now;
        fetchAndDisplayImage();
    }

    delay(1);
}