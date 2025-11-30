# Picobound Matrix LED

A Wi-Fi enabled, blockchain-powered 16x16 WS2812B LED matrix display, built on ESP32. Fetches pixel art and animations from both Ethereum smart contracts and the Picobound.xyz API, displaying them in real time.


## Demo

![Picobound Matrix LED Demo](/demo/demo.png)

*The device displays pixel art fetched from the blockchain, API of Picobound.xyz, matching the on-screen preview.*

![Picobound Matrix LED workflow](/demo/workflow.png)


## Features

- **ESP32-based**: Fast, reliable microcontroller with Wi-Fi support.
- **16x16 WS2812B Matrix**: Vivid, individually addressable RGB LEDs.
- **Blockchain & API Integration**: Fetches pixel art and animation data from both Ethereum (via RPC) and the Picobound.xyz API.
- **Modular Firmware**: Clean separation of networking, blockchain, display, and input logic.
- **Non-blocking Design**: Responsive button input and smooth animations.
- **Brightness Caching**: Efficient LED updates and power management.
- **Button Controls**: User interaction for cycling images, adjusting brightness, etc.

## Tech Stack

- C++ (PlatformIO)
- ESP32
- FastLED
- Ethereum JSON-RPC

## Directory Structure

```
lib/
  AppConfig/      # Centralized constants and configuration
  Blockchain/     # Ethereum RPC and image fetch logic
  Display/        # LED rendering and brightness management
  Input/          # Button debounce and input handling
  Network/        # Wi-Fi connection management
src/
  main.cpp        # Entry point, wires modules together
platformio.ini    # PlatformIO project configuration
```

## Getting Started

### Prerequisites

- [PlatformIO](https://platformio.org/) installed (VS Code recommended)
- ESP32 development board
- 16x16 WS2812B LED matrix
- (Optional) Button(s) for input

### Build & Flash

1. Clone this repository:
   ```sh
   git clone https://github.com/Kay-79/Picobound-MatrixLed.git
   cd Picobound-MatrixLed
   ```
2. Connect your ESP32 board.
3. Edit Wi-Fi and blockchain settings in `lib/AppConfig/AppConfig.h` as needed.
4. Build and upload firmware:
   ```sh
   pio run -t upload
   ```

### Usage

- On boot, the device connects to Wi-Fi and fetches pixel data from the blockchain.
- Use the button(s) to cycle images, adjust brightness, or trigger other actions as configured.

## Customization

- **Add new features**: Place new modules in `lib/` and expose headers for easy inclusion.
- **Change pin mappings or constants**: Edit `AppConfig.h`.
- **Update blockchain logic**: Modify `Blockchain/` module.

## Contributing

Pull requests and issues are welcome! Please document any new hardware pins or behaviors in `AppConfig.h` and update this README as needed.

## License

MIT
