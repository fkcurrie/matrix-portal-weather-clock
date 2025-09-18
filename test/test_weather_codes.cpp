#include <Arduino.h>
#include <unity.h>
#include "weather_utils.h"

void test_wmoCodeToString_clear() {
    TEST_ASSERT_EQUAL_STRING("Clear", wmoCodeToString(0).c_str());
    TEST_ASSERT_EQUAL_STRING("M.Clear", wmoCodeToString(1).c_str());
}

void test_wmoCodeToString_cloudy() {
    TEST_ASSERT_EQUAL_STRING("P.Cloudy", wmoCodeToString(2).c_str());
    TEST_ASSERT_EQUAL_STRING("Overcast", wmoCodeToString(3).c_str());
}

void test_wmoCodeToString_drizzle() {
    TEST_ASSERT_EQUAL_STRING("Drizzle", wmoCodeToString(51).c_str());
    TEST_ASSERT_EQUAL_STRING("Drizzle", wmoCodeToString(53).c_str());
    TEST_ASSERT_EQUAL_STRING("Drizzle", wmoCodeToString(55).c_str());
}

void test_wmoCodeToString_rain() {
    TEST_ASSERT_EQUAL_STRING("Rain", wmoCodeToString(61).c_str());
    TEST_ASSERT_EQUAL_STRING("Rain", wmoCodeToString(63).c_str());
    TEST_ASSERT_EQUAL_STRING("Rain", wmoCodeToString(65).c_str());
}

void test_wmoCodeToString_snow() {
    TEST_ASSERT_EQUAL_STRING("Snow", wmoCodeToString(71).c_str());
    TEST_ASSERT_EQUAL_STRING("Snow", wmoCodeToString(73).c_str());
    TEST_ASSERT_EQUAL_STRING("Snow", wmoCodeToString(75).c_str());
}

void test_wmoCodeToString_showers() {
    TEST_ASSERT_EQUAL_STRING("Showers", wmoCodeToString(80).c_str());
    TEST_ASSERT_EQUAL_STRING("Showers", wmoCodeToString(81).c_str());
    TEST_ASSERT_EQUAL_STRING("Showers", wmoCodeToString(82).c_str());
}

void test_wmoCodeToString_thunderstorm() {
    TEST_ASSERT_EQUAL_STRING("T-Storm", wmoCodeToString(95).c_str());
    TEST_ASSERT_EQUAL_STRING("T-Storm", wmoCodeToString(99).c_str());
}

void test_wmoCodeToString_unknown() {
    TEST_ASSERT_EQUAL_STRING("---", wmoCodeToString(100).c_str());
    TEST_ASSERT_EQUAL_STRING("---", wmoCodeToString(-1).c_str());
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_wmoCodeToString_clear);
    RUN_TEST(test_wmoCodeToString_cloudy);
    RUN_TEST(test_wmoCodeToString_drizzle);
    RUN_TEST(test_wmoCodeToString_rain);
    RUN_TEST(test_wmoCodeToString_snow);
    RUN_TEST(test_wmoCodeToString_showers);
    RUN_TEST(test_wmoCodeToString_thunderstorm);
    RUN_TEST(test_wmoCodeToString_unknown);
    UNITY_END();
}

void loop() {
    // Nothing to do here
}
