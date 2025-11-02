/*
 * CODE ĐIỀU KHIỂN LED MATRIX 16x16 NEOPIXEL BẰNG ESP32
 * Lấy dữ liệu ảnh PICO-8 trực tiếp từ blockchain (on-chain)
 * * Yêu cầu thư viện (Cài qua Library Manager):
 * 1. FastLED
 * 2. ArduinoJson (bởi Benoit Blanchon)
 */

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <FastLED.h>
#include <ArduinoJson.h>

const char *ssid = "Lock B";
const char *password = "0347979595";

const char *rpcUrl = "https://ethereum-sepolia-rpc.publicnode.com";

const char *contractAddress = "0x00000000610d8474af42d87b42f8c54efc05e023";
const int tokenIdToFetch = 3921;

#define DATA_PIN 18
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 256
#define BRIGHTNESS 50

#define MATRIX_WIDTH 16
#define MATRIX_HEIGHT 16
#define SERPENTINE_LAYOUT true
#define FLIP_HORIZONTAL false
#define FLIP_VERTICAL false

CRGB pico8_palette[16] = {
    CRGB(0, 0, 0),       // 0: black
    CRGB(29, 43, 83),    // 1: dark_blue
    CRGB(126, 37, 83),   // 2: dark_purple
    CRGB(0, 135, 81),    // 3: dark_green
    CRGB(171, 82, 54),   // 4: brown
    CRGB(95, 87, 79),    // 5: dark_gray
    CRGB(194, 195, 199), // 6: light_gray
    CRGB(255, 241, 232), // 7: white
    CRGB(255, 0, 77),    // 8: red
    CRGB(255, 163, 0),   // 9: orange
    CRGB(255, 236, 39),  // 10 ('A'): yellow
    CRGB(0, 228, 54),    // 11 ('B'): green
    CRGB(41, 173, 255),  // 12 ('C'): blue
    CRGB(131, 118, 156), // 13 ('D'): lavender
    CRGB(255, 119, 168), // 14 ('E'): pink
    CRGB(255, 204, 170)  // 15 ('F'): light_peach
};

int hexCharToPico8Index(char c)
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f')
    {
        return 10 + (c - 'a');
    }
    if (c >= 'A' && c <= 'F')
    {
        return 10 + (c - 'A');
    }
    return 0;
}

CRGB leds[NUM_LEDS];

static inline uint16_t XY(uint8_t x, uint8_t y)
{
    if (FLIP_HORIZONTAL)
        x = MATRIX_WIDTH - 1 - x;
    if (FLIP_VERTICAL)
        y = MATRIX_HEIGHT - 1 - y;

    uint16_t i;
    if (SERPENTINE_LAYOUT)
    {
        if (y & 1)
        {
            // hàng lẻ đi ngược
            i = y * MATRIX_WIDTH + (MATRIX_WIDTH - 1 - x);
        }
        else
        {
            i = y * MATRIX_WIDTH + x;
        }
    }
    else
    {
        i = y * MATRIX_WIDTH + x;
    }
    if (i >= NUM_LEDS)
        i = 0;
    return i;
}

void showTestPattern()
{
    for (uint8_t y = 0; y < MATRIX_HEIGHT; y++)
    {
        for (uint8_t x = 0; x < MATRIX_WIDTH; x++)
        {
            uint8_t idx = (((x >> 2) + (y >> 2)) & 1) ? 12 /*blue*/ : 8 /*red*/;
            leds[XY(x, y)] = pico8_palette[idx];
        }
    }
    FastLED.show();
}

String parseAbiEncodedString(String hexResult)
{
    if (hexResult.startsWith("0x"))
    {
        hexResult.remove(0, 2);
    }

    String lengthHex = hexResult.substring(64, 128);
    unsigned long length = strtoul(lengthHex.c_str(), NULL, 16);

    if (length == 0 || length > 512)
    {
        Serial.println("Fail: Invalid string length parsed from ABI data.");
        return "";
    }

    String dataHex = hexResult.substring(128, 128 + (length * 2));
    String asciiString = "";

    for (int i = 0; i < length * 2; i += 2)
    {
        String byteHex = dataHex.substring(i, i + 2);
        char c = (char)strtol(byteHex.c_str(), NULL, 16);

        if (c == 0)
            break;

        asciiString += c;
    }

    return asciiString;
}

String getPicoboundImage(int tokenId)
{
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    http.setTimeout(15000);
    http.begin(client, rpcUrl);
    http.addHeader("Content-Type", "application/json");

    DynamicJsonDocument docRequest(768);
    docRequest["jsonrpc"] = "2.0";
    docRequest["id"] = 1;
    docRequest["method"] = "eth_call";

    JsonArray params = docRequest.createNestedArray("params");
    JsonObject callObj = params.createNestedObject();
    callObj["to"] = contractAddress;

    char tokenIdHex[65];
    sprintf(tokenIdHex, "%064x", tokenId);
    String dataPayload = "0xb4b3f359" + String(tokenIdHex);

    callObj["data"] = dataPayload;
    params.add("latest");

    String requestBody;
    serializeJson(docRequest, requestBody);

    Serial.println("Sending HTTP request to fetch image data...");

    int httpCode = http.POST(requestBody);

    if (httpCode == HTTP_CODE_OK)
    {
        DynamicJsonDocument docResponse(8192);
        DeserializationError error = deserializeJson(docResponse, http.getString());

        if (error)
        {
            Serial.print("deserializeJson() thất bại: ");
            Serial.println(error.c_str());
            http.end();
            return "";
        }

        String hexResult = docResponse["result"] | "";

        if (hexResult.length() > 130)
        {
            http.end();
            return parseAbiEncodedString(hexResult);
        }
        else
        {
            Serial.println("Fail: Result string is too short or invalid.");
            http.end();
            return "";
        }
    }
    else
    {
        Serial.printf("HTTP request failed, error code: %d\n", httpCode);
        http.end();
        return "";
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting Picobound LED Matrix Display...");

    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 1200);

    FastLED.clear();
    FastLED.show();

    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    int wifi_retries = 20;
    while (WiFi.status() != WL_CONNECTED && wifi_retries > 0)
    {
        delay(500);
        Serial.print(".");
        wifi_retries--;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.print("Connected to WiFi!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println("\nFailed to connect to WiFi. Restarting...");
        ESP.restart();
    }
}

void loop()
{
    Serial.println("Starting new cycle...");
    Serial.printf("Fetching image for Token ID: %d\n", tokenIdToFetch);

    String pixelString = getPicoboundImage(tokenIdToFetch);

    if (pixelString.length() == 256)
    {
        Serial.println("Received 256 characters. Drawing on LED...");

        for (uint8_t y = 0; y < MATRIX_HEIGHT; y++)
        {
            for (uint8_t x = 0; x < MATRIX_WIDTH; x++)
            {
                int pos = y * MATRIX_WIDTH + x;
                char hexChar = pixelString[pos];
                int colorIndex = hexCharToPico8Index(hexChar);
                leds[XY(x, y)] = pico8_palette[colorIndex];
            }
        }

        FastLED.show();
        Serial.println("Display complete!");
    }
    else
    {
        Serial.printf("Fail: Invalid pixel string received (length %d), expected 256.\n", pixelString.length());
        showTestPattern();
    }

    Serial.println("Waiting 30 seconds before updating...");
    delay(30000);
}