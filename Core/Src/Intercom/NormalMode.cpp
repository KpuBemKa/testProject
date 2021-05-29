#include "Intercom/NormalMode.h"

enum class Access
{
    None,
    Enter,
    Exit,
    Both
};

struct Key
{
    uint32_t keyID;
    Access access;
};

bool NormalMode::CheckKey(uint32_t key)
{
    Key keys[] =
        {
            {12563593, Access::Both},
            {12563594, Access::Exit}};

    for (unsigned int i = 0; i < sizeof(keys) / sizeof(keys[0]); i++)
    {
        if (key == keys[i].keyID)
        {
            if (keys[i].access == Access::Both)
            {
                return true;
            }
        }
    }

    return false;
}