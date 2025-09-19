# Adafruit Matrix Portal M4 Weather Clock

This project uses an Adafruit Matrix Portal M4 with a 64x64 LED screen to display a real-time analog clock with live weather data. It is built using PlatformIO and C++ and is optimized for long-term stability.

## Features

*   **Animated Boot Sequence:**
    *   Displays a spinner animation while connecting to WiFi and fetching initial data.
    *   Shows a dynamic boot screen with a fading rainbow border.
    *   Displays Public IP, Private IP, Geolocation, and current UTC/Local time on colored, dynamically centered lines.
*   **Analog Clock:**
    *   A real-time analog clock with hour, minute, and second hands.
    *   Time is fetched from an NTP server and automatically adjusted for the correct timezone based on geolocation.
*   **Live Weather Data:**
    *   Weather information is fetched from the free Open-Meteo API.
    *   **Top-Left:** Current temperature.
    *   **Top-Right:** 8x8 pixel icon representing the current weather conditions.
    *   **Bottom-Left:** Daily low temperature.
    *   **Bottom-Right:** Daily high temperature.
*   **Stability:**
    *   Code has been refactored to use `char` arrays instead of `String` objects to prevent memory fragmentation.
    *   All network requests are non-blocking to ensure the clock remains smooth and responsive.

## Getting Started

1.  **Install PlatformIO:** Follow the official instructions at [platformio.org](https://platformio.org/install).
2.  **Create `include/secrets.h`:** Before building, create a file named `secrets.h` inside the `include` directory with your WiFi credentials:
    ```cpp
    #define WIFI_SSID "your_ssid"
    #define WIFI_PASS "your_password"
    ```
3.  **Build and Upload:** Use the following PlatformIO commands:
    *   `pio run` to build the project.
    *   `pio run --target upload` to upload the compiled code to the board.

## Hardware Setup

*   **Power Supply:** A separate 5V 4A (or greater) power supply connected to the screw terminals on the Matrix Portal is **required** for a 64x64 matrix.
*   **Address E Jumper:** For 64x64 matrices, the "Address E" jumper on the back of the board must be soldered.