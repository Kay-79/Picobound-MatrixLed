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
        constexpr int TOKEN_ID = 6328;
        constexpr size_t EXPECTED_IMAGE_CHARS = 258; // includes 0x prefix
    }

    namespace Buttons
    {
        constexpr uint8_t DECREASE_PIN = 7;
        constexpr uint8_t INCREASE_PIN = 6;
        constexpr unsigned long DEBOUNCE_MS = 30;
    }

    namespace Brightness
    {
        constexpr uint8_t MIN_LEVEL = 0;
        constexpr uint8_t MAX_LEVEL = 255;
        constexpr uint8_t STEP = 5;
        constexpr uint8_t DEFAULT_LEVEL = 10;
    }

    namespace Timing
    {
        constexpr uint32_t FETCH_INTERVAL_MS = 3600000UL;
    }
} // namespace AppConfig
