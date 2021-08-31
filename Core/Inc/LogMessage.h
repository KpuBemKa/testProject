#include "stm32f1xx_hal.h"

struct LogMessage
{
    uint8_t type;
    uint8_t subType;
};

 

struct LogInfo {
  uint32_t time;
  uint8_t state;
  uint32_t beginEvent;
  uint8_t sizeOfMessage;
  uint8_t eventType;
  uint8_t mes1;
  uint32_t mes2;
};