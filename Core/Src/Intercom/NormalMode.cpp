#include "Intercom/NormalMode.h"

/*--------> Defines <--------*/
#define Max_Users 256

/*--------> Structures <--------*/
struct Key
{
    uint32_t keyID;
    Access access;
};

struct User
{
  uint32_t keyID;
  bool isFirstRangeAvailable;
  uint16_t enterAccessTime[64];
  uint16_t exitAccessTime[64];
};

struct Day
{
    uint16_t minute;
    uint8_t week;
    uint16_t year;
};

Key keys[] =
    {
        {12563593, Access::Both},
        {12563594, Access::Exit}};

User users[Max_Users];


/*--------> Code <--------*/
NormalMode::NormalMode()
{
    // read from flash memory but not today
    users[0] = {
        12563593, false, {600, 960}, {600, 960}
    };
}

bool NormalMode::CheckKey(uint32_t key, Side side)
{
    for (unsigned int i = 0; i < sizeof(users) / sizeof(users[0]); i++)
    {
        if (side == Side::Iniside)
        {
            if (key == users[i].keyID && (keys[i].access == Access::Both || keys[i].access == Access::Exit))
            {
                return true;
            }
        }

        if (side == Side::Outside)
        {
            if (key == keys[i].keyID && (keys[i].access == Access::Both || keys[i].access == Access::Exit))
            {
                return true;
            }
        }
    }

    return false;
}

bool isTimeAllowing(Day day)
{
    
}