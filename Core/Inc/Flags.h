#pragma once
#include "IntercomMode.h"
#include "WorkMode.h"

extern bool
    insideButtonPressed,
    outsideButtonPressed,
    doorSensorFirstTime;

extern IntercomMode intercomMode;
extern WorkMode workMode, previousWorkMode;