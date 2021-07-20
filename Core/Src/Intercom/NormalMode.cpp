#include "Intercom/NormalMode.h"

/*--------> Defines <--------*/
#define Max_Users 256

/*--------> Structures <--------*/

struct User
{
    uint32_t keyID;
    bool isFirstEnterRangeAvailable;
    bool isFirstExitRangeAvailable;
    uint16_t enterAccessTime[64];
    uint16_t exitAccessTime[64];
};

/**
 * @brief Find the biggest number smaller than maxNumber
*/
uint16_t findMaxIndex(uint16_t *array, uint8_t size, uint32_t maxNumber);
uint32_t currentWeekNumber(RTC_DateTypeDef currentDate);
bool isLeapYear(uint16_t year);
bool isTimeAllowing(RTC_TimeTypeDef currentTime, RTC_DateTypeDef currentDate, User user, Side side);

User __attribute__((section(".users_data"))) users[] = {
    {12563593, false, false, {600, 1080, 2040, 2520, 3480, 3960, 4920, 5400, 6360, 6840, 7800, 8280, 9240, 9720, 10680, 11160, 12120, 12600, 13560, 14040, 15000, 15480, 16440, 16920, 17880, 18360, 19320, 19800}, {600, 1080, 2040, 2520, 3480, 3960, 4920, 5400, 6360, 6840, 7800, 8280, 9240, 9720, 10680, 11160, 12120, 12600, 13560, 14040, 15000, 15480, 16440, 16920, 17880, 18360, 19320, 19800}},
    {12563594, false, false, {600, 1080, 2040, 2520, 3480, 3960, 4920, 5400, 6360, 6840, 7800, 8280, 9240, 9720, 10680, 11160, 12120, 12600, 13560, 14040, 15000, 15480, 16440, 16920, 17880, 18360, 19320, 19800}, {600, 1080, 2040, 2520, 3480, 3960, 4920, 5400, 6360, 6840, 7800, 8280, 9240, 9720, 10680, 11160, 12120, 12600, 13560, 14040, 15000, 15480, 16440, 16920, 17880, 18360, 19320, 19800}}};

/*--------> Code <--------*/
NormalMode::NormalMode()
{
}

bool NormalMode::CheckKey(uint32_t key, Side side, RTC_TimeTypeDef currentTime, RTC_DateTypeDef currentDate)
{
    for (unsigned int i = 0; i < sizeof(users) / sizeof(users[0]); i++)
    {
        if (key == users[i].keyID)
        {
            if (side == Side::Inside)
            {
                return isTimeAllowing(currentTime, currentDate, users[i], Side::Inside);
            }
            else
            {
                return isTimeAllowing(currentTime, currentDate, users[i], Side::Outside);
            }
        }
    }

    return false;
}

bool isTimeAllowing(RTC_TimeTypeDef currentTime, RTC_DateTypeDef currentDate, User user, Side side)
{
    uint32_t weeks = currentWeekNumber(currentDate);
    uint32_t currentMinute = 0;

    if (weeks % 2 == 0)
    {
        currentMinute = 10080; //7 days * 24 hours * 60 minutes
    }

    currentMinute += ((currentDate.WeekDay - 1) * 24 + currentTime.Hours) * 60 + currentTime.Minutes;

    if (side == Side::Outside)
    {
        uint8_t minMinuteIndex = findMaxIndex(user.enterAccessTime, 64, currentMinute);

        if (minMinuteIndex % 2 == 0)
        {
            return !user.isFirstEnterRangeAvailable;
        }
        else
        {
            return user.isFirstEnterRangeAvailable;
        }
    }
    else
    {
        uint8_t minMinuteIndex = findMaxIndex(user.exitAccessTime, 64, currentMinute);

        if (minMinuteIndex % 2 == 0)
        {
            return !user.isFirstExitRangeAvailable;
        }
        else
        {
            return user.isFirstExitRangeAvailable;
        }
    }
}

/**
 * @brief Find the biggest number smaller than maxNumber
*/
uint16_t findMaxIndex(uint16_t *array, uint8_t size, uint32_t maxNumber)
{
    uint16_t minNumberIndex = 0;
    uint16_t minNumber = 0;

    for (int i = 0; i < size; i++)
    {
        if (array[i] > minNumber && array[i] < maxNumber)
        {
            minNumber = array[i];
            minNumberIndex = i;
        }
    }

    return minNumberIndex;
}

uint32_t currentWeekNumber(RTC_DateTypeDef currentDate)
{
    uint32_t days = 0;
    uint16_t currentYear = currentDate.Year + 2000;

    for (int year = 2018; year < currentYear; year++)
    {
        if (isLeapYear(year))
        {
            days += 366;
        }
        else
        {
            days += 365;
        }
    }

    for (int i = 1; i < currentDate.Month; i++)
    {
        if (i <= 7)
        {
            if (i == 2 && isLeapYear(currentDate.Year))
            {
                days += 29;
                continue;
            }
            else if (i == 2 && !isLeapYear(currentDate.Year))
            {
                days += 28;
                continue;
            }

            if (i % 2 == 0 && i != 2)
            {
                days += 30;
            }
            else
            {
                days += 31;
            }
        }
        else
        {
            if (i % 2 == 0)
            {
                days += 31;
            }
            else
            {
                days += 30;
            }
        }
    }

    uint32_t n = (days + currentDate.Date - 1) / 7 - 52 * (currentDate.Year - 18);
    return n;
}

bool isLeapYear(uint16_t year)
{
    year += 2000;
    bool isLeap = false;

    if (year % 4 == 0)
        isLeap = true;

    if (year % 100 == 0)
        isLeap = false;

    if (year % 400 == 0)
        isLeap = true;

    return isLeap;
}