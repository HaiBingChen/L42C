#include "Typedef.h"
#include "AppHandler.h"

#ifndef __BatteryStatusHandler_H
#define __BatteryStatusHandler_H

typedef enum
{
        BATTERY_RESERVE_STATE,
        BATTERY_LOW_WARNING,
        BATTERY_CHARGE_NOTICE
}BatteryType;


typedef enum
{
        BATTERY_0_SENCOND = 0,
        BATTERY_5_SENCOND = 5,
        BATTERY_10_SENCOND = 10,
        BATTERY_30_SENCOND = 30,
        BATTERY_60_SENCOND = 60
}BatteryChargeTime;


typedef struct 
{
        int        MyId;                     
        uint8    DisplayTime;
        uint8    BatteryCharging;
        uint8    BatteryDispalyType;
} BatteryStateTAG;




extern uint16 ChargeTime;
extern uint16 BATTERY_FULL_SENCOND;



uint16 BatteryStatusHandler(MsgType msg, int iParam, void *pContext);
void UpdateBatteryStatus(bool SizeFlag,bool OneTime);
void ChargeDisplayStateChange(bool flag);
extern void SimulatePowerOffKey(uint8 time1, uint16 time2);
#endif
