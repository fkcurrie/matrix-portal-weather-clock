# Changelog

## [1.0.0] - 2025-09-19

### Added

-   **Analog Clock:** Real-time clock with hour, minute, and second hands.
-   **Live Weather:** Fetches and displays current temperature, daily high/low, and a weather icon.
-   **Animated Boot Sequence:**
    -   Spinner animation during startup.
    -   Fading rainbow border.
    -   Dynamically centered display of IP addresses, location, and time.
-   **Automatic Timezone:** Geolocation is used to set the correct timezone.
-   **Stability and Performance:**
    -   Refactored to use `char` arrays instead of `String` objects to prevent memory fragmentation.
    -   Implemented non-blocking network calls to ensure smooth operation.

### Changed

-   Pivoted from CircuitPython to a stable and feature-rich PlatformIO C++ implementation.

## [Unreleased]