/*
 * Matrix Portal Weather Clock
 *
 * Copyright (c) 2025 Frank Currie <frank@sfle.ca>
 *
 * All rights reserved
 *
 * This software is the confidential and proprietary information of
 * Frank Currie. You shall not disclose such Confidential Information
 * and shall use it only in accordance with the terms of the license
 * agreement you entered into with Frank Currie.
 *
 * Created: 2025-09-17
 */

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
#include "weather_utils.h"

// --- WiFi Setup ---
#include "secrets.h"

// --- Time Setup ---
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
unsigned long last_time_update = 0;
long timezone_offset = 0;
int last_hour = -1, last_minute = -1, last_second = -1;

// --- Geolocation & Weather Setup ---
enum FetchState { IDLE, FETCHING, SUCCESS, ERROR };
FetchState geo_state = IDLE;
FetchState weather_state = IDLE;
const char* geo_host = "ip-api.com";
const char* geo_path = "/json/?fields=status,message,country,region,city,lat,lon,timezone,offset,query";
float latitude = 43.65; // Default to Toronto
float longitude = -79.38;
char public_ip[16] = "?.?.?.?";
char location_str[32] = "Toronto, ON";
const char* weather_host = "api.open-meteo.com";
char weather_path[256];
WiFiClient wifi;
HttpClient geo_client = HttpClient(wifi, geo_host, 80);
HttpClient weather_client = HttpClient(wifi, weather_host, 80);
char current_temp[8] = "--";
char max_temp[8] = "--";
char min_temp[8] = "--";
int weather_code = 0;
unsigned long last_weather_update = 0;
bool initial_setup_done = false;

// --- Matrix Setup ---
uint8_t rgbPins[] = {7, 8, 9, 10, 11, 12};
uint8_t addrPins[] = {17, 18, 19, 20, 21};
uint8_t clockPin = 14;
uint8_t latchPin = 15;
uint8_t oePin = 16;
Adafruit_Protomatter matrix(64, 6, 1, rgbPins, 5, addrPins, clockPin, latchPin, oePin, true);
int spinner_angle = 0;

uint16_t colorWheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return matrix.color565(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return matrix.color565(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return matrix.color565(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void drawRainbowBorder(float brightness) {
    int w = matrix.width();
    int h = matrix.height();
    for (int i = 0; i < w; i++) {
        uint16_t color = colorWheel((i * 256 / w) & 255);
        uint8_t r = (uint8_t)((((color >> 11) & 0x1F) * 527) * brightness / 255);
        uint8_t g = (uint8_t)((((color >> 5) & 0x3F) * 259) * brightness / 255);
        uint8_t b = (uint8_t)(((color & 0x1F) * 527) * brightness / 255);
        matrix.drawPixel(i, 0, matrix.color565(r,g,b));
        matrix.drawPixel(i, h - 1, matrix.color565(r,g,b));
    }
    for (int i = 0; i < h; i++) {
        uint16_t color = colorWheel((i * 256 / h) & 255);
        uint8_t r = (uint8_t)((((color >> 11) & 0x1F) * 527) * brightness / 255);
        uint8_t g = (uint8_t)((((color >> 5) & 0x3F) * 259) * brightness / 255);
        uint8_t b = (uint8_t)(((color & 0x1F) * 527) * brightness / 255);
        matrix.drawPixel(0, i, matrix.color565(r,g,b));
        matrix.drawPixel(w - 1, i, matrix.color565(r,g,b));
    }
}

void drawSpinner() {
    matrix.fillScreen(0);
    int centerX = matrix.width() / 2;
    int centerY = matrix.height() / 2;
    int length = 15;
    int x = centerX + length * cos(radians((float)spinner_angle));
    int y = centerY + length * sin(radians((float)spinner_angle));
    matrix.drawLine(centerX, centerY, x, y, matrix.color565(100, 100, 100));
    spinner_angle = (spinner_angle + 10) % 360;
    matrix.show();
}

void displayBootInfo(const char* utc_time, const char* local_time, const char* pub_ip, const char* priv_ip, const char* loc) {
    matrix.fillScreen(0);

    for (int i = 0; i < 256; i++) {
        drawRainbowBorder(i);
        matrix.show();
        delay(2);
    }

    int16_t x1, y1;
    uint16_t w, h;
    
    const int line_padding = 2;
    int num_lines = 5;
    
    char pub_ip_str[32];
    snprintf(pub_ip_str, sizeof(pub_ip_str), "PubIP: %s", pub_ip);
    matrix.getTextBounds(pub_ip_str, 0, 0, &x1, &y1, &w, &h);
    bool split_pub_ip = (w > matrix.width() - 4);
    if (split_pub_ip) {
        num_lines++;
    }

    int font_height = h;
    int total_text_height = (num_lines * font_height) + ((num_lines - 1) * line_padding);
    int y_pos = ((matrix.height() - 2 - total_text_height) / 2) + 1 + font_height;

    if (split_pub_ip) {
        const char* pub_ip_label = "PubIP:";
        matrix.getTextBounds(pub_ip_label, 0, 0, &x1, &y1, &w, &h);
        matrix.setCursor((matrix.width() - w) / 2, y_pos);
        matrix.setTextColor(matrix.color565(255, 0, 0)); // Red
        matrix.print(pub_ip_label);
        y_pos += font_height + line_padding;

        matrix.getTextBounds(pub_ip, 0, 0, &x1, &y1, &w, &h);
        matrix.setCursor((matrix.width() - w) / 2, y_pos);
        matrix.print(pub_ip);
        y_pos += font_height + line_padding;
    } else {
        matrix.getTextBounds(pub_ip_str, 0, 0, &x1, &y1, &w, &h);
        matrix.setCursor((matrix.width() - w) / 2, y_pos);
        matrix.setTextColor(matrix.color565(255, 0, 0)); // Red
        matrix.print(pub_ip_str);
        y_pos += font_height + line_padding;
    }

    char priv_ip_str[32];
    snprintf(priv_ip_str, sizeof(priv_ip_str), "PrivIP: %s", priv_ip);
    matrix.getTextBounds(priv_ip_str, 0, 0, &x1, &y1, &w, &h);
    matrix.setCursor((matrix.width() - w) / 2, y_pos);
    matrix.setTextColor(matrix.color565(255, 165, 0)); // Orange
    matrix.print(priv_ip_str);
    y_pos += font_height + line_padding;

    matrix.getTextBounds(loc, 0, 0, &x1, &y1, &w, &h);
    matrix.setCursor((matrix.width() - w) / 2, y_pos);
    matrix.setTextColor(matrix.color565(255, 255, 0)); // Yellow
    matrix.print(loc);
    y_pos += font_height + line_padding;

    char local_str[32];
    snprintf(local_str, sizeof(local_str), "Local: %s", local_time);
    matrix.getTextBounds(local_str, 0, 0, &x1, &y1, &w, &h);
    matrix.setCursor((matrix.width() - w) / 2, y_pos);
    matrix.setTextColor(matrix.color565(0, 0, 255)); // Blue
    matrix.print(local_str);
    y_pos += font_height + line_padding;

    char utc_str[32];
    snprintf(utc_str, sizeof(utc_str), "UTC: %s", utc_time);
    matrix.getTextBounds(utc_str, 0, 0, &x1, &y1, &w, &h);
    matrix.setCursor((matrix.width() - w) / 2, y_pos);
    matrix.setTextColor(matrix.color565(0, 255, 0)); // Green
    matrix.print(utc_str);

    matrix.show();

    delay(5000);

    for (int i = 255; i >= 0; i--) {
        drawRainbowBorder(i);
        matrix.show();
        delay(2);
    }
}

void startGeoLocationFetch() {
    Serial.println("Starting geolocation data fetch...");
    geo_client.get(geo_path);
    geo_state = FETCHING;
}

void updateGeoLocation() {
    if (geo_state == FETCHING) {
        if (geo_client.available()) {
            int statusCode = geo_client.responseStatusCode();
            String response = geo_client.responseBody();

            if (statusCode == 200) {
                JsonDocument doc;
                deserializeJson(doc, response);
                latitude = doc["lat"];
                longitude = doc["lon"];
                strlcpy(public_ip, doc["query"], sizeof(public_ip));
                const char* city = doc["city"];
                const char* region = doc["region"];
                snprintf(location_str, sizeof(location_str), "%s, %s", city, region);
                timezone_offset = doc["offset"];
                timeClient.setTimeOffset(timezone_offset);
                Serial.println("Geolocation update successful.");
                geo_state = SUCCESS;
            } else {
                Serial.print("Geolocation update failed with code: ");
                Serial.println(statusCode);
                geo_state = ERROR;
            }
        }
    }
}

void startWeatherDataFetch() {
  Serial.println("Starting weather data fetch...");
  snprintf(weather_path, sizeof(weather_path),
           "/v1/forecast?latitude=%.2f&longitude=%.2f&current_weather=true&daily=weathercode,temperature_2m_max,temperature_2m_min&timezone=auto",
           latitude, longitude);
  weather_client.get(weather_path);
  weather_state = FETCHING;
}

void updateWeatherData() {
    if (weather_state == FETCHING) {
        if (weather_client.available()) {
            int statusCode = weather_client.responseStatusCode();
            String response = weather_client.responseBody();

            if (statusCode == 200) {
                JsonDocument doc;
                deserializeJson(doc, response);
                snprintf(current_temp, sizeof(current_temp), "%dC", (int)round(doc["current_weather"]["temperature"].as<float>()));
                snprintf(max_temp, sizeof(max_temp), "%dC", (int)round(doc["daily"]["temperature_2m_max"][0].as<float>()));
                snprintf(min_temp, sizeof(min_temp), "%dC", (int)round(doc["daily"]["temperature_2m_min"][0].as<float>()));
                weather_code = doc["current_weather"]["weathercode"].as<int>();
                Serial.println("Weather update successful.");
                weather_state = SUCCESS;
            } else {
                Serial.print("Weather update failed with code: ");
                Serial.println(statusCode);
                weather_state = ERROR;
            }
        }
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
  matrix.setFont(&TomThumb);
  
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    drawSpinner();
  }

  timeClient.begin();
  while(!timeClient.update()) {
    drawSpinner();
    timeClient.forceUpdate();
  }
  
  startGeoLocationFetch();
}

void loop() {
  unsigned long current_millis = millis();

  updateGeoLocation();
  updateWeatherData();

  if (!initial_setup_done) {
    if (geo_state == SUCCESS) {
      timeClient.update();
      String local_time_str = timeClient.getFormattedTime();
      
      timeClient.setTimeOffset(0);
      timeClient.update();
      String utc_time_str = timeClient.getFormattedTime();
      timeClient.setTimeOffset(timezone_offset);

      IPAddress localIP = WiFi.localIP();
      char localIPStr[16];
      snprintf(localIPStr, sizeof(localIPStr), "%d.%d.%d.%d", localIP[0], localIP[1], localIP[2], localIP[3]);

      displayBootInfo(utc_time_str.c_str(), local_time_str.c_str(), public_ip, localIPStr, location_str);
      
      matrix.fillScreen(0);

      startWeatherDataFetch();
      last_weather_update = millis();
      initial_setup_done = true;
    } else {
      drawSpinner();
    }
  }

  if (current_millis - last_weather_update > 3600000) { // Update weather every hour
    startWeatherDataFetch();
    last_weather_update = current_millis;
  }

  if (initial_setup_done && current_millis - last_time_update > 1000) {
    last_time_update = current_millis;
    
    timeClient.update();
    int hours = timeClient.getHours();
    int minutes = timeClient.getMinutes();
    int seconds = timeClient.getSeconds();

    if (seconds != last_second) {
      
      if (minutes != last_minute) {
        matrix.fillScreen(0);
        drawClockFace();

        int16_t x1, y1;
        uint16_t w, h;

        matrix.setTextColor(matrix.color565(0, 255, 0));
        matrix.setCursor(1, 5);
        matrix.print(current_temp);

        drawWeatherIcon(weather_code);

        matrix.setTextColor(matrix.color565(0, 0, 255));
        matrix.setCursor(1, matrix.height() - 1);
        matrix.print(min_temp);

        matrix.setTextColor(matrix.color565(255, 0, 0));
        matrix.getTextBounds(max_temp, 0, 0, &x1, &y1, &w, &h);
        matrix.setCursor(matrix.width() - w - 1, matrix.height() - 1);
        matrix.print(max_temp);
      } else {
        float lastSecondAngle = last_second * 6.0;
        drawHand(lastSecondAngle, 28, 0);
      }

      float hourAngle = (hours % 12 + minutes / 60.0) * 30.0;
      drawHand(hourAngle, 16, matrix.color565(255, 0, 0));
      float minuteAngle = minutes * 6.0;
      drawHand(minuteAngle, 24, matrix.color565(0, 255, 0));
      float secondAngle = seconds * 6.0;
      drawHand(secondAngle, 28, matrix.color565(0, 0, 255));
      
      last_hour = hours;
      last_minute = minutes;
      last_second = seconds;
      
      matrix.show();
    }
  }
}