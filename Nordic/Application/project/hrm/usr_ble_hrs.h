#ifndef _USR_BLE_HRS_H
#define _USR_BLE_HRS_H


//extern export
void usr_ble_hrs_on_ble_evt(ble_evt_t * p_ble_evt);
void usr_ble_hrs_service_init(void);
void usr_ble_hrs_refresh_hr_val(uint16_t hr_val);

#endif //_USR_BLE_HRS_H

