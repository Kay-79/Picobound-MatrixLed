#pragma once
#include <Arduino.h>

namespace AppConfig
{
    constexpr uint32_t SERIAL_BAUD = 115200;

    namespace Wifi
    {
        constexpr char SSID[] = "Lock B";
        constexpr char PASSWORD[] = "0347979595";
    }

    namespace Blockchain
    {
        constexpr char RPC_URL[] = "https://ethereum-sepolia-rpc.publicnode.com";
        constexpr char CONTRACT_ADDRESS[] = "0x96a241b3da246d30d4d4b0a79436ab4ee26585c9";
        constexpr int TOKEN_ID = 9223;
        constexpr size_t PIXEL_HEX_CHARS_PER_FRAME = 256; // 16x16 matrix
        constexpr uint8_t MAX_ANIMATION_FRAMES = 4;
        constexpr size_t SINGLE_IMAGE_PAYLOAD_CHARS = PIXEL_HEX_CHARS_PER_FRAME + 2; // account for 0x prefix
        constexpr size_t MULTI_IMAGE_PAYLOAD_CHARS = PIXEL_HEX_CHARS_PER_FRAME * MAX_ANIMATION_FRAMES + 2;
    }

    namespace Buttons
    {
        constexpr uint8_t DECREASE_PIN = 10;
        constexpr uint8_t INCREASE_PIN = 21;
        constexpr unsigned long DEBOUNCE_MS = 30;
    }

    namespace Brightness
    {
        constexpr uint8_t MIN_LEVEL = 0;
        constexpr uint8_t MAX_LEVEL = 255;
        constexpr uint8_t STEP = 5;
        constexpr uint8_t DEFAULT_LEVEL = 10;
    }

    namespace Display
    {
        constexpr uint8_t DEFAULT_ANIMATION_SPEED = 3; // 1=slow, 2=medium, 3=fast
        constexpr uint16_t BASE_ANIMATION_INTERVAL_MS = 1000;
    }

    namespace Timing
    {
        constexpr uint32_t FETCH_INTERVAL_MS = 3600000UL;
    }
} // namespace AppConfig
