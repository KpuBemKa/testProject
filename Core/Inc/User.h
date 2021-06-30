#pragma once

struct User
{
  uint32_t code;
  bool isFirstRangeAvailable;
  std::vector<uint16_t> accessTime;
};