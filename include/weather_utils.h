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

#pragma once
#include <Arduino.h>

String wmoCodeToString(int code) {
  if (code == 0) return "Clear";
  if (code == 1) return "M.Clear"; // Abbreviated
  if (code == 2) return "P.Cloudy"; // Abbreviated
  if (code == 3) return "Overcast";
  if (code >= 51 && code <= 55) return "Drizzle";
  if (code >= 61 && code <= 65) return "Rain";
  if (code >= 71 && code <= 75) return "Snow";
  if (code >= 80 && code <= 82) return "Showers";
  if (code >= 95 && code <= 99) return "T-Storm";
  return "---";
}
