#pragma once
#include <Arduino.h>

namespace AppConfig
{
    constexpr uint32_t SERIAL_BAUD = 115200;

    namespace Wifi
    {
        constexpr char AP_NAME[] = "Picobound-Setup";
        constexpr char AP_PASSWORD[] = "picobound";
        constexpr char DEFAULT_SSID[] = "24Kay";
        constexpr char DEFAULT_PASSWORD[] = "2424KAYY";
    }

    namespace Storage
    {
        constexpr char NAMESPACE[] = "picobound";
        constexpr char KEY_WALLET[] = "wallet";
    }

    namespace Blockchain
    {
        constexpr char RPC_URL[] = "https://ethereum-sepolia-rpc.publicnode.com";
        constexpr char RESOLVER_ADDRESS[] = "0x00000000f6bb2f2820d990631842fdf5a6ad6e66";
        // Default wallet if not configured
        constexpr char DEFAULT_WALLET[] = "0xD59FA9E2bD52e247a9c4AD2472C4131E097d0531";
        constexpr size_t PIXEL_HEX_CHARS_PER_FRAME = 256; // 16x16 matrix
        constexpr uint8_t MAX_ANIMATION_FRAMES = 4;
        constexpr size_t SINGLE_IMAGE_PAYLOAD_CHARS = PIXEL_HEX_CHARS_PER_FRAME + 2; // account for 0x prefix
        constexpr size_t MULTI_IMAGE_PAYLOAD_CHARS = PIXEL_HEX_CHARS_PER_FRAME * MAX_ANIMATION_FRAMES + 2;
    }

    namespace Buttons
    {
        constexpr uint8_t DECREASE_PIN = 10;
        constexpr uint8_t INCREASE_PIN = 21;
        constexpr uint8_t RESET_PIN = 8;
        constexpr uint8_t RESET_BRIGHTNESS_PIN = 5;
        constexpr unsigned long DEBOUNCE_MS = 30;
    }

    namespace Brightness
    {
        constexpr uint8_t MIN_LEVEL = 0;
        constexpr uint8_t MAX_LEVEL = 50;
        constexpr uint8_t STEP = 3;
        constexpr uint8_t DEFAULT_LEVEL = 5;
    }

    namespace Display
    {
        constexpr uint8_t DEFAULT_ANIMATION_SPEED = 3; // 1=slow, 2=medium, 3=fast
        constexpr uint16_t BASE_ANIMATION_INTERVAL_MS = 1000;
    }

    namespace Timing
    {
        constexpr uint32_t FETCH_INTERVAL_MS = 3600000UL;
        constexpr uint32_t AVATAR_POLL_INTERVAL_MS = 10000UL;
    }
} // namespace AppConfig
