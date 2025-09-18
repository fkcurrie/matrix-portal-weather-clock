#include <Arduino.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Adafruit_Protomatter.h>
#include <Adafruit_GFX.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include <Fonts/TomThumb.h>
#include "weather_icons_8x8.h"

// --- WiFi Setup ---
#include "secrets.h"

// --- Time Setup ---
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
const long gmtOffset_sec = -14400; // Toronto is UTC-4 for EDT

// --- Weather Setup ---
const char* weather_host = "api.open-meteo.com";
const String weather_path = "/v1/forecast?latitude=43.65&longitude=-79.38&current_weather=true&daily=weathercode,temperature_2m_max,temperature_2m_min&timezone=America%2FNew_York";
WiFiClient wifi;
HttpClient client = HttpClient(wifi, weather_host, 80);
String current_temp = "--";
String max_temp = "--";
String min_temp = "--";
int weather_code = 0;
unsigned long last_weather_update = 0;

// --- Matrix Setup ---
uint8_t rgbPins[] = {7, 8, 9, 10, 11, 12};
uint8_t addrPins[] = {17, 18, 19, 20, 21};
uint8_t clockPin = 14;
uint8_t latchPin = 15;
uint8_t oePin = 16;
Adafruit_Protomatter matrix(64, 6, 1, rgbPins, 5, addrPins, clockPin, latchPin, oePin, true);

void fetchWeatherData() {
  Serial.println("Fetching weather data...");
  client.get(weather_path);
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  if (statusCode == 200) {
    JsonDocument doc;
    deserializeJson(doc, response);
    current_temp = String((int)round(doc["current_weather"]["temperature"].as<float>())) + "C";
    max_temp = String((int)round(doc["daily"]["temperature_2m_max"][0].as<float>())) + "C";
    min_temp = String((int)round(doc["daily"]["temperature_2m_min"][0].as<float>())) + "C";
    weather_code = doc["daily"]["weathercode"][0].as<int>();
    Serial.println("Weather update successful.");
  } else {
    Serial.print("Weather update failed with code: ");
    Serial.println(statusCode);
  }
}

void drawWeatherIcon(int code) {
  int x = matrix.width() - 9;
  int y = 1;

  if (code == 0 || code == 1) { // Clear/Sunny
    matrix.drawBitmap(x, y, icon_clear_8x8, 8, 8, matrix.color565(255, 255, 0)); // Yellow
  } else if (code == 2 || code == 3) { // Cloudy
    matrix.drawBitmap(x, y, icon_cloudy_8x8, 8, 8, matrix.color565(255, 255, 255)); // White
  } else if (code >= 51 && code <= 65) { // Rain
    matrix.drawBitmap(x, y, icon_rain_cloud_8x8, 8, 8, matrix.color565(255, 255, 255)); // White cloud
    matrix.drawBitmap(x, y, icon_rain_drops_8x8, 8, 8, matrix.color565(0, 0, 255));   // Blue drops
  } else if (code >= 71 && code <= 75) { // Snow
    matrix.drawBitmap(x, y, icon_snow_cloud_8x8, 8, 8, matrix.color565(255, 255, 255)); // White cloud
    matrix.drawBitmap(x, y, icon_snow_flakes_8x8, 8, 8, matrix.color565(200, 200, 200)); // Light gray flakes
  } else if (code >= 95 && code <= 99) { // Storm
    matrix.drawBitmap(x, y, icon_storm_cloud_8x8, 8, 8, matrix.color565(128, 128, 128)); // Gray cloud
    matrix.drawBitmap(x, y, icon_storm_bolt_8x8, 8, 8, matrix.color565(255, 255, 0));   // Yellow bolt
  } else { // Default
    matrix.drawBitmap(x, y, icon_cloudy_8x8, 8, 8, matrix.color565(255, 255, 255));
  }
}

void drawHand(float angle, int length, uint16_t color) {
  int centerX = matrix.width() / 2;
  int centerY = matrix.height() / 2;
  int x = centerX + length * cos(radians(angle - 90));
  int y = centerY + length * sin(radians(angle - 90));
  matrix.drawLine(centerX, centerY, x, y, color);
}

void drawClockFace() {
  int centerX = matrix.width() / 2;
  int centerY = matrix.height() / 2;
  matrix.drawCircle(centerX, centerY, 31, matrix.color565(100, 100, 100));
}

void setup() {
  Serial.begin(115200);
  matrix.begin();
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    delay(500);
  }
  timeClient.begin();
  timeClient.setTimeOffset(gmtOffset_sec);
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  fetchWeatherData();
  last_weather_update = millis();
}

void loop() {
  if (millis() - last_weather_update > 60000) {
    fetchWeatherData();
    last_weather_update = millis();
  }

  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();
  int hours = (epochTime % 86400L) / 3600;
  int minutes = (epochTime % 3600) / 60;
  int seconds = epochTime % 60;

  matrix.fillScreen(0);
  drawClockFace();

  // --- Draw Weather ---
  matrix.setFont(&TomThumb);
  int16_t x1, y1;
  uint16_t w, h;

  // Top-left: Current Temp (Green)
  matrix.setTextColor(matrix.color565(0, 255, 0));
  matrix.setCursor(1, 5);
  matrix.print(current_temp);

  // Top-right: Icon
  drawWeatherIcon(weather_code);

  // Bottom-left: Min Temp (Blue)
  matrix.setTextColor(matrix.color565(0, 0, 255));
  matrix.setCursor(1, matrix.height() - 1);
  matrix.print(min_temp);

  // Bottom-right: Max Temp (Red)
  matrix.setTextColor(matrix.color565(255, 0, 0));
  matrix.getTextBounds(max_temp.c_str(), 0, 0, &x1, &y1, &w, &h);
  matrix.setCursor(matrix.width() - w - 1, matrix.height() - 1);
  matrix.print(max_temp);

  matrix.setFont(nullptr); // Revert to default font

  // --- Draw Hands ---
  float hourAngle = (hours % 12 + minutes / 60.0) * 30.0;
  float minuteAngle = minutes * 6.0;
  float secondAngle = seconds * 6.0;
  drawHand(hourAngle, 16, matrix.color565(255, 0, 0));
  drawHand(minuteAngle, 24, matrix.color565(0, 255, 0));
  drawHand(secondAngle, 28, matrix.color565(0, 0, 255));

  matrix.show();
  delay(1000);
}
