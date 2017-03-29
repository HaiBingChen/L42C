#include "nrf.h"
#include "bsp.h"
#include "nrf51.h"
#include "nrf_soc.h"
#include "nrf_gpio.h"
#include "pstorage.h"
#include "ble_ancs_c.h"
#include "ble_gap.h"
#include "ble_hci.h"
#include "ble_advertising.h"
#include "nrf_drv_timer.h"
#include "stdio.h"
#include "string.h"
#include "stdio.h"
#include "stdint.h"
#include <stdlib.h>
#include <stdbool.h>
#include "Typedef.h"
#include "app_error.h"
#include "app_timer.h"
#include "clockdriver.h"
#include "Osal.h"
#include "mem.h"
#include "OsalMessage.h"
#include "OsalMemory.h"
#include "OsalClock.h"
#include "OsalTaskEvent.h"
#include "OsalTimer.h"
#include "OsalPort.h"
#include "common.h"
#include <heap_4_config.h>
#include "app_util_platform.h"
#include "app_uart_fifo.h"
#include "Ble_nus.h"
#include "simpleGATTprofile.h"
#include "HwTask.h"
#include "ClockTask.h"
#include "StepTask.h"
#include "SleepTask.h"
#include "BleTask.h"
#include "TouchKeyTask.h"
#include "AppHandler.h"
#include "DisplayTask.h"
#include "Macrodef.h"
#include "AlgInterface.h"
#include "UserInfo.h"
#include "Utils.h"
#include "circbuf.h"
#include "Motor.h"
#include "ExtSpiFlash.h"
#include "StandbyHandler.h"
#include "BlueToothHandler.h"
#include "BatteryStatusHandler.h"
#include "VolumeHandler.h"
#include "RingModeHandler.h"
#include "SleepHandler.h"
#include "AlarmHandler.h"
#include "PhoneHandler.h"
#include "SportHandler.h"
#include "AncsRemove.h"
#include "AxisI2c.h"
#include "Bma2xx.h"
#include "Reminder.h"
#include "BC5CmdPackage.h"
#include "NeuL18CmdPackage.h"
#include "Ui.h"
#include "Res.h"
#include "DataStoreInside.h"
#include "FontDisplay.h"
#include "FontLib.h"
#include "HardwareTest.h"
#include "IQS263_Handler.h"
#include "lower_level_xxx.h"
#include "upper_level_xxx.h"
#include "CypressI2c.h"
#include "lcd.h" 
#include "spim.h"
#include "protocol.h"
#include "SmardCardHandler.h"
#include "ble_ccids.h"
#include "scanner_beacon.h"
#include "advertiser_beacon.h"
#include "DevicesComHandler.h"
#include "Secondstimer.h"
#include "bl_private.h"
#include "bl_protocol.h"
#include "port_me.h"
#include "bl_config.h"
#include "code2str.h"
#include "ble_media.h"
#include "scanner_parse.h"
#include "KL17CmdPackage.h"
#include "Gesture_Recog.h"
#include "AS7000.h"
#include "AS7000_I2c.h"
#include "AS7000_update.h"

#include "heartRateTask.h"
#include "heartRateHandler.h"
#include "ble_hrs.h"
#include "usr_ble_hrs.h"
#include "math.h"

#include "lis3dh.h"

#include "pwm.h"
#include "AncsCrc.h"





