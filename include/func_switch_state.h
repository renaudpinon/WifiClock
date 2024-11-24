/**
 * (c) Renaud PINON 2022.
 * 
 * Ce projet appartient à Renaud PINON. Le code peut être utilisé sans restriction à condition que l'appartenance du projet soit conservée et clairement indiquée.
 * */

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
