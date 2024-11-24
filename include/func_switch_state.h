#ifndef FUNC_SWITCH_STATE_H
#define FUNC_SWITCH_STATE_H

#include <Arduino.h>

#include "enums.h"
#include "defines.h"

SwitchStateType SwitchStateGet()
{
    return digitalRead(kPinStateSwitch) == HIGH ? SwitchStateTypeAPMode : SwitchStateTypeWifiMode;
}

#endif
