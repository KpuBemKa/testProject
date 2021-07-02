/* #pragma once

#include "stm32f1xx_hal.h"
#include <vector>

*
 * uint32_t code
 * bool isFirstRangeAvailable
 * std::vector<uint16_t> enterAccessTime
 * std::vector<uint16_t> exitAccessTime

struct User
{
  uint32_t code;
  bool isFirstIntervalAvailable;
  uint16_t enterIntervals[64];
  uint16_t exitIntervals[64];
}; */