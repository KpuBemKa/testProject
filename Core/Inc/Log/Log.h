#pragma once
#include <stdio.h>
#include <string.h>
#include "stm32f1xx_hal.h"
#include "IntercomMode.h"

enum class RecievedMessage : uint8_t
{
    ChangeID = 1,
    NetworkSettings = 2,
    ServerAdress = 3,
    EnableDHCP = 4,
    EntranceTime = 5,
    LockOpenTime = 6,
    RelayWorkMode = 7,
    sd = 8,
    LockImpulseTime = 9,
    SirenType = 10,
    SirenTimeout = 11,
    ChangeUser = 12,
    ChangeUserSchedule = 13,
    ChangeIntercomSchedule = 14,
    DeleteUser = 15,
    DoorUnlock = 16,
    IntercomModeChange = 17,
    SirenStop = 18,
    SendMemoryStatus = 19
};

enum class EventType : uint8_t
{
    AccessRequest = 1,
    AccessGranted = 2,
    AccessDenied = 3,
    DoorOpened = 4,
    DoorClosed = 5,
    AlarmStart = 6,
    AlarmEnd = 7,
    SirenStart = 8,
    SirenEnd = 9,
    IntercomModeChange = 10,
    SystemOn = 11,
    ExecutionResult = 200
};

enum class Side : uint8_t
{
    In = 1,
    Out = 2
};

enum class DenyReason : uint8_t
{
    InvalidIntercomMode = 1,
    InvalidKey = 2,
    InvalidDirection = 3,
    InvalidSchedule = 4
};

enum class OpenedDoorReason : uint8_t
{
    AllowedOpening = 1,
    DeniedOpening = 2
};

enum class EntranceAllowedReason : uint8_t
{
    EntranceByKey = 1,
    OpenMode = 2,
    ServerGrant = 3
};

enum class AlarmStartReason : uint8_t
{
    LongPassage = 1
};

enum class AlarmEndReason : uint8_t
{
    ClosedDoor = 1,
    AllowedPasage = 2,
    RemoteEnd = 3
};

enum class SirenStartReason : uint8_t
{
    ForbiddenPassage = 1
};

enum class SirenEndReason : uint8_t
{
    EndTime = 1,
};

enum class ModeChangeReason : uint8_t
{
    BySchedule = 1,
    ByServer = 2,
    ByKey = 3,
    ByServerWithBlocking = 4
};

struct Log
{
private:
    EventType eventType;

protected:
    Log(EventType eventType)
    {
        this->eventType = eventType;
    }

public:
    uint8_t GetSize()
    {
        switch (this->eventType)
        {
        case EventType::AccessRequest:
            return 6;
            break;
        default:
            return 1;
            break;
        }
    }
};

/* -==< Event Types >==- */

struct AccessRequest : Log
{
    Side side;
    uint32_t keyID;

    AccessRequest() : Log(EventType::AccessRequest)
    {
    }
};

struct AccessGranted : Log
{
    AccessGranted() : Log(EventType::AccessGranted)
    {
    }
};

struct AccessDenied : Log
{
    DenyReason denyReason;

    AccessDenied() : Log(EventType::AccessDenied)
    {
    }
};

struct DoorOpened : Log
{
    OpenedDoorReason openedDoorReason;
    EntranceAllowedReason entranceAllowedReason;

    DoorOpened() : Log(EventType::DoorOpened)
    {
    }
};

struct DoorClosed : Log
{
    DoorClosed() : Log(EventType::DoorClosed)
    {
    }
};

struct AlarmStart : Log
{
    AlarmStartReason alarmstartReason;

    AlarmStart() : Log(EventType::AlarmStart)
    {
    }
};

struct AlarmEnd : Log
{
    AlarmEndReason alarmEndReason;

    AlarmEnd() : Log(EventType::AlarmEnd)
    {
    }
};

struct SirenStart : Log
{
    SirenStartReason sirenStartReason;

    SirenStart() : Log(EventType::SirenStart)
    {
    }
};

struct SirenEnd : Log
{
    SirenEndReason sirenEndReason;

    SirenEnd() : Log(EventType::SirenEnd)
    {
    }
};

struct IntercomModeChange : Log
{
    Side side;
    IntercomMode intercomMode;
    ModeChangeReason modeChangeReason;

    IntercomModeChange() : Log(EventType::IntercomModeChange)
    {
    }
};

struct SystemOn : Log
{
    SystemOn() : Log(EventType::SystemOn)
    {
    }
};

struct ExecutionResult : Log
{
    RecievedMessage recievedMessage;
    bool isAttemptSuccessful;
};

void Logging(Log log)
{
    uint8_t array[6];
    memcpy(&array, &log, log.GetSize());
}

void test()
{
    AccessRequest accessRequest;
    accessRequest.side = Side::In;
    accessRequest.keyID = 108734;
}