#include "headfile.h"

static ble_hrs_t                             m_hrs;                                     /**< Structure used to identify the heart rate service. */


void usr_ble_hrs_on_ble_evt(ble_evt_t * p_ble_evt)
{
    ble_hrs_on_ble_evt(&m_hrs, p_ble_evt);
}


void usr_ble_hrs_service_init(void)
{
    uint32_t       err_code;
    ble_hrs_init_t hrs_init;
    uint8_t        body_sensor_location;
    
    // Initialize Heart Rate Service.
    body_sensor_location = BLE_HRS_BODY_SENSOR_LOCATION_FINGER;

    memset(&hrs_init, 0, sizeof(hrs_init));

    hrs_init.evt_handler                 = NULL;
    hrs_init.is_sensor_contact_supported = true;
    hrs_init.p_body_sensor_location      = &body_sensor_location;

    // Here the sec level for the Heart Rate Service can be changed/increased.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&hrs_init.hrs_hrm_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&hrs_init.hrs_hrm_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&hrs_init.hrs_hrm_attr_md.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&hrs_init.hrs_bsl_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&hrs_init.hrs_bsl_attr_md.write_perm);

    err_code = ble_hrs_init(&m_hrs, &hrs_init);
    APP_ERROR_CHECK(err_code);
}


void usr_ble_hrs_refresh_hr_val(uint16_t hr_val)
{
    ble_hrs_heart_rate_measurement_send(&m_hrs, hr_val);
}



