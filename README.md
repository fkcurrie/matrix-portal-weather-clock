# Adafruit Matrix Portal M4 Weather Clock

This project uses an Adafruit Matrix Portal M4 with a 64x64 LED screen to display a real-time analog clock with live weather data for Toronto, ON. It is built using PlatformIO and C++.

## Features

*   **Analog Clock:** A real-time analog clock with hour, minute, and second hands. Time is fetched from an NTP server.
*   **Live Weather Data:** Weather information is fetched every minute from the free Open-Meteo API.
*   **Corner Display:**
    *   **Top-Left:** Current temperature (Green).
    *   **Top-Right:** 8x8 pixel icon representing the current weather conditions (e.g., Sunny, Cloudy, Rain).
    *   **Bottom-Left:** Daily low temperature (Blue).
    *   **Bottom-Right:** Daily high temperature (Red).
*   **Custom Fonts & Icons:** Uses the compact "TomThumb" font for weather text and custom-designed 8x8 pixel icons for conditions.

## Resources

*   **Adafruit MatrixPortal M4 Guide:** [https://learn.adafruit.com/adafruit-matrixportal-m4?view=all](https://learn.adafruit.com/adafruit-matrixportal-m4?view=all)

## Getting Started

This project is built using PlatformIO.

1.  **Install PlatformIO:** Follow the official instructions at [platformio.org](https://platformio.org/install).
2.  **Create `include/secrets.h`:** Before building, create a file named `secrets.h` inside the `include` directory with your WiFi credentials:
    ```cpp
    #define WIFI_SSID "your_ssid"
    #define WIFI_PASS "your_password"
    ```
3.  **Build and Upload:** Use the following PlatformIO commands:
    *   `pio run` to build the project.
    *   `pio run --target upload` to upload the compiled code to the board.

## Testing

Unit tests are located in the `test` directory and can be run with the `pio test` command. The tests cover the weather code conversion logic.

## Hardware Setup

This configuration is confirmed to work for a 64x64 matrix as of Sept 17, 2025.

### Power Supply

A separate 5V 4A (or greater) power supply connected to the screw terminals on the Matrix Portal is **required** for a 64x64 matrix. Powering it solely via USB-C will cause instability and display issues.

### Address E Jumper

For 64x64 matrices, the "Address E" jumper on the back of the board must be soldered. This connects the E address line required for matrices taller than 32 pixels.

### Pin Configuration

The `Adafruit_Protomatter` library is initialized with the following pin configuration in `src/main.cpp`:

*   **RGB Data Pins:** `7, 8, 9, 10, 11, 12`
*   **Address Pins:** `17, 18, 19, 20, 21`
*   **Clock Pin:** `14`
*   **Latch Pin:** `15`
*   **Output Enable Pin:** `16`

## Libraries Used

This project relies on the following libraries, which are managed by PlatformIO via the `platformio.ini` file:

*   `adafruit/Adafruit Protomatter`
*   `adafruit/Adafruit GFX Library`
*   `adafruit/WiFiNINA`
*   `arduino-libraries/NTPClient`
*   `bblanchon/ArduinoJson`
*   `arduino-libraries/ArduinoHttpClient`
