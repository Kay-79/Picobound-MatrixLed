# Picobound Matrix LED – Copilot Instructions

## System Overview

-   ESP32-based 16x16 WS2812B matrix driven through FastLED.
-   Core modules: `Network` (Wi-Fi bootstrap), `Blockchain` (Ethereum RPC fetch), `Display` (render + brightness cache), `Input` (button debounce), `AppConfig` (central constants).
-   `src/main.cpp` wires the modules via `setup()` and `loop()` only; new logic should live in libraries whenever possible.

## Coding Guardrails

1. Keep hardware-facing code non-blocking—prefer short polling over long delays.
2. Always pull shared constants from `AppConfig.h`; do not scatter literals.
3. Reuse `ButtonState` helpers instead of reimplementing debounce logic.
4. Refresh LEDs through `drawPixelString()` or `displayRefreshLastFrame()` so brightness caching stays valid.
5. Avoid dynamic allocation in the loop; reuse global `String` buffers or stack arrays.
6. Whenever you add or modify a feature, capture the behavior changes and new expectations in this file so future contributors stay aligned.

## Feature Workflow

-   Networking: call `connectWiFi()` during boot; if retries are needed, expose them via a helper rather than editing `main.cpp` directly.
-   Image fetch: build RPC payloads with `Blockchain::getPicoboundImage(...)` and validate against `AppConfig::Blockchain::EXPECTED_IMAGE_CHARS` before rendering.
-   Display: update visuals through `drawPixelString()`; when only brightness changes, call `displaySetBrightness()`.
-   Buttons: initialize via `initButton()` and drive interactions via `buttonReleased(...)` with the global debounce constant.

## Testing & Deployment

-   Use `pio run -t upload` for firmware flashes; ensure the board is connected on the `esp32dev` environment defined in `platformio.ini`.
-   When adding new libraries, place them under `lib/<Name>/` and expose headers at the root so they can be included with quotes.
-   Document any new hardware pins or behaviors in `AppConfig.h` and mirror them in this file when relevant.
