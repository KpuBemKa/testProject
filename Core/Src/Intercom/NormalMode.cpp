#include "Intercom/NormalMode.h"

struct Key
{
    uint32_t keyID;
    Access access;
};

Key keys[] =
    {
        {12563593, Access::Both},
        {12563594, Access::Exit}};

bool NormalMode::CheckKey(uint32_t key, Side side)
{
    for (unsigned int i = 0; i < sizeof(keys) / sizeof(keys[0]); i++)
    {
        if (side == Side::Iniside)
        {
            if (key == keys[i].keyID && (keys[i].access == Access::Both || keys[i].access == Access::Exit))
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