#include <Arduino.h>
#include "Network.h"
#include "Blockchain.h"
#include "Display.h"

const char *ssid = "Lock B";
const char *password = "0347979595";
// const char *ssid = "Bamos Coffee";
// const char *password = "bamosxinchao";
const char *rpcUrl = "https://ethereum-sepolia-rpc.publicnode.com";
const char *contractAddress = "0x00000000610d8474af42d87b42f8c54efc05e023";
const int tokenId = 3675;

// Brightness control (IO2)
const int BUTTON_PIN = 0;
const uint8_t BRIGHTNESS_MIN = 0;
const uint8_t BRIGHTNESS_MAX = 255;
const uint8_t BRIGHTNESS_STEP = 5;
const uint8_t DEFAULT_BRIGHTNESS = 10;

uint8_t g_brightness = DEFAULT_BRIGHTNESS;
String img;

// Debounce & press type detection
bool btnReading = HIGH;
bool btnStable = HIGH;
unsigned long lastDebounceMs = 0;
const unsigned long debounceMs = 30;
unsigned long pressStartMs = 0;
bool pressActive = false;
const unsigned long longPressMs = 600;

// Optional brightness setter from Display (won't break if missing)
extern void displaySetBrightness(uint8_t) __attribute__((weak));
static inline void applyBrightness()
{
    Serial.printf("Applying brightness: %d\n", g_brightness);
    FastLED.setBrightness(g_brightness);
    drawPixelString(img);
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting Picobound LED Matrix...");

    displayInit();
    applyBrightness();

    pinMode(BUTTON_PIN, INPUT_PULLUP);

    if (!connectWiFi(ssid, password))
    {
        ESP.restart();
    }
}

void loop()
{
    static unsigned long lastFetchMs = 0;
    const unsigned long fetchIntervalMs = 3600000UL;
    unsigned long now = millis();

    int reading = digitalRead(BUTTON_PIN);
    if (reading != btnReading)
    {
        lastDebounceMs = now;
        btnReading = reading;
    }
    if ((now - lastDebounceMs) > debounceMs)
    {
        if (btnStable == HIGH && btnReading == LOW)
        {
            pressActive = true;
            pressStartMs = now;
        }
        if (btnStable == LOW && btnReading == HIGH)
        {
            if (pressActive)
            {
                unsigned long held = now - pressStartMs;
                if (held >= longPressMs)
                {
                    g_brightness = (g_brightness >= BRIGHTNESS_STEP)
                                       ? (uint8_t)(g_brightness - BRIGHTNESS_STEP)
                                       : BRIGHTNESS_MIN;
                }
                else
                {
                    g_brightness = (g_brightness <= (BRIGHTNESS_MAX - BRIGHTNESS_STEP))
                                       ? (uint8_t)(g_brightness + BRIGHTNESS_STEP)
                                       : BRIGHTNESS_MAX;
                }
                applyBrightness();
                pressActive = false;
            }
        }
        btnStable = btnReading;
    }

    if (lastFetchMs == 0 || (now - lastFetchMs) >= fetchIntervalMs)
    {
        lastFetchMs = now;

        Serial.println("Fetching image...");
        img = getPicoboundImage(rpcUrl, contractAddress, tokenId);
        Serial.printf("Image data length: %d\n", img.length());
        Serial.println(img);

        if (img.length() == 258)
        {
            img.replace("0x", "");
            drawPixelString(img);
        }
        else
        {
            showTestPattern();
        }
    }

    delay(1);
}
