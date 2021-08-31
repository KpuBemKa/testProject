#pragma once
#include "stm32f1xx_hal.h"

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
    Text = 100,
    ExecutionResult = 200
};

enum class Direction : uint8_t
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

enum class OpenedDoorReason
{
    AllowedOpening = 1,
    DeniedOpening = 2
};

enum class EntranceAllowedReason
{
    EntranceByKey = 1,
    OpenMode = 2,
    ServerGrant = 3
};

enum class AlarmStartReason
{
    ForbiddenPassage = 1,
    LongPassage = 2
};

enum class AlarmEndReason
{
    ClosedDoor = 1,
    AllowedPasage = 2,
    RemoteEnd = 3
};

enum class SirenEndReason
{
    EndTime = 1,

};

/* -==< Event Types >==- */

struct AccessRequest
{
    Direction direction;
    uint32_t keyID;
};

struct AccessGranted
{
};

struct AccessDenied
{
    DenyReason denyReason;
};

struct OpenedDoor
{
};

struct DoorClosed
{
};

struct AlarmStart
{
};

struct AlarmEnd
{
};

struct SirenStart
{
};

struct SirenEnd
{
};

struct IntercomModeChange
{
};

struct SystemOn
{
};

struct Text
{
};

struct ExecutionResult
{
};

union LogMessage
{
    AccessRequest accessRequest;
    AccessGranted accessGranted;
    AccessDenied accessDenied;
    OpenedDoor openedDoor;
    DoorClosed doorClosed;
    AlarmStart alarmStart;
    AlarmEnd alarmEnd;
    SirenStart sirenStart;
    SirenEnd sirenEnd;
    IntercomModeChange intercomModeChange;
    SystemOn systemOn;
    Text text;
    ExecutionResult executionResult;

    uint8_t logMessage[5];
};

struct Log
{
    EventType eventType;
    LogMessage logMessage;
};

void Logging()
{
    Log log;
    log.eventType = EventType::AccessRequest;
    log.logMessage.accessRequest.direction = Direction::In;
}