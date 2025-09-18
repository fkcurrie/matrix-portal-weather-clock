/*
 * Matrix Portal Weather Clock
 *
 * Copyright (c) 2025 Frank Currie <frank@sfle.ca>
 *
 * All rights reserved.
 *
 * This software is the confidential and proprietary information of
 * Frank Currie. You shall not disclose such Confidential Information
 * and shall use it only in accordance with the terms of the license
 * agreement you entered into with Frank Currie.
 *
 * Created: 2025-09-17
 */

// Weather Icons (8x8 pixels)

#pragma once
#include <Arduino.h>

// Sunny/Clear Icon (Yellow)
const unsigned char icon_clear_8x8[] PROGMEM = {
    0x18, 0x3c, 0x7e, 0xff, 0xff, 0x7e, 0x3c, 0x18,
};

// Cloudy Icon (White)
const unsigned char icon_cloudy_8x8[] PROGMEM = {
    0x00, 0x1c, 0x3e, 0x7f, 0xff, 0xff, 0x1c, 0x00,
};

// Rain Icon (Blue Drops)
const unsigned char icon_rain_cloud_8x8[] PROGMEM = {
    0x00, 0x1c, 0x3e, 0x7f, 0xff, 0x00, 0x00, 0x00,
};
const unsigned char icon_rain_drops_8x8[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x52, 0x00,
};

// Snow Icon (White Flakes)
const unsigned char icon_snow_cloud_8x8[] PROGMEM = {
    0x00, 0x1c, 0x3e, 0x7f, 0xff, 0x00, 0x00, 0x00,
};
const unsigned char icon_snow_flakes_8x8[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x2a, 0x55, 0x2a,
};

// Storm Icon (Yellow Bolt)
const unsigned char icon_storm_cloud_8x8[] PROGMEM = {
    0x00, 0x1c, 0x3e, 0x7f, 0xff, 0x00, 0x00, 0x00,
};
const unsigned char icon_storm_bolt_8x8[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x18, 0x30,
};