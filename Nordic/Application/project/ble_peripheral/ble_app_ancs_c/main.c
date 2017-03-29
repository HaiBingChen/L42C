/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 */

/** @file
 *
 * @defgroup ble_sdk_apple_notification_main main.c
 * @{
 * @ingroup ble_sdk_app_apple_notification
 * @brief Apple Notification Client Sample Application main file. Disclaimer: 
 * This client implementation of the Apple Notification Center Service can and 
 * will be changed at any time by Nordic Semiconductor ASA.
 *
 * Server implementations such as the ones found in iOS can be changed at any 
 * time by Apple and may cause this client implementation to stop working.
 *
 * This file contains the source code for a sample application using the Apple 
 * Notification Center Service Client.
 */
#include "headfile.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "Osal.h"
#include "ble_db_discovery.h"
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble_hci.h"
#include "ble_gap.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "nrf_gpio.h"
#include "ble_srv_common.h"
#include "ble_conn_params.h"
#include "nrf51_bitfields.h"
#include "device_manager.h"
#include "app_button.h"
#include "app_timer.h"
#include "pstorage.h"
#include "nrf_soc.h"
#include "bsp.h"
#include "bsp_btn_ble.h"
#include "softdevice_handler.h"
#include "app_trace.h"
#include "nrf_delay.h"
#include "app_scheduler.h"
#include "app_timer_appsh.h"
#include "nrf_timer.h"

#if BUTTONS_NUMBER < 2
#error "Not enough resources on board"
#endif

#define UART_TX_BUF_SIZE                1024                                        /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                1                                           /**< UART RX buffer size. */

#define ATTR_DATA_SIZE                  BLE_ANCS_ATTR_DATA_MAX                      /**< Allocated size for attribute data. */

#define DISPLAY_MESSAGE_BUTTON_ID       1                                           /**< Button used to request notification attributes. */

#define APP_ADV_FAST_INTERVAL           MSEC_TO_UNITS(400, UNIT_0_625_MS)                                         /**< The advertising interval (in units of 0.625 ms). The default value corresponds to 25 ms. */
#define APP_ADV_SLOW_INTERVAL         MSEC_TO_UNITS(2000, UNIT_0_625_MS)                                       /**< Slow advertising interval (in units of 0.625 ms). The default value corresponds to 2 seconds. */
#define APP_ADV_FAST_TIMEOUT            180                                         /**< The advertising time-out in units of seconds. */
#define APP_ADV_SLOW_TIMEOUT          180                                         /**< The advertising time-out in units of seconds. */
#define ADV_INTERVAL_FAST_PERIOD     30                                          /**< The duration of the fast advertising period (in seconds). */

#define APP_TIMER_PRESCALER               0                                           /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_MAX_TIMERS            4                 /**< Maximum number of simultaneously created timers. */
#define APP_TIMER_OP_QUEUE_SIZE     4                                           /**< Size of timer operation queues. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(120, UNIT_1_25_MS)            /**< Minimum acceptable connection interval (0.5 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(180, UNIT_1_25_MS)           /**< Maximum acceptable connection interval (1 second). */
#define SLAVE_LATENCY                         2                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(6000, UNIT_10_MS)             /**< Connection supervisory time-out (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(10, APP_TIMER_PRESCALER)  /**< Time from initiating an event (connect or start of notification) to the first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER) /**< Time between each call to sd_ble_gap_conn_param_update after the first (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    6                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define MESSAGE_BUFFER_SIZE             18                                          /**< Size of buffer holding optional messages in notifications. */

#define SEC_PARAM_BOND                  1                                           /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                           /**< Man In The Middle protection not required. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                        /**< No I/O capabilities. */
#define SEC_PARAM_OOB                   0                                           /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                           /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                          /**< Maximum encryption key size. */

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump. Can be used to identify stack location on stack unwind. */

uint8 DEVICE_NAME[12]="VIBE #00000";                                      /**< Name of the device. Will be included in the advertising data. */

static ble_ancs_c_t              m_ancs_c;                                 /**< Structure used to identify the Apple Notification Service Client. */

uint16_t                  m_conn_handle = BLE_CONN_HANDLE_INVALID;  /**< Handle of the current connection. */

static ble_db_discovery_t        m_ble_db_discovery;                       /**< Structure used to identify the DB Discovery module. */

static uint8_t                   m_ancs_uuid_type;                         /**< Store ANCS UUID. */
static dm_application_instance_t m_app_handle;                             /**< Application identifier allocated by the Device Manager. */
dm_handle_t               m_peer_handle;                            /**< Identifies the peer that is currently connected. */
static ble_gap_sec_params_t      m_sec_param;                              /**< Security parameter for use in security requests. */

ble_ancs_c_evt_notif_t m_notification_latest;                       /**< Local copy to keep track of the newest arriving notifications. */

static uint8_t m_attr_title[ATTR_DATA_SIZE];                               /**< Buffer to store attribute data. */
static uint8_t m_attr_subtitle[ATTR_DATA_SIZE];                            /**< Buffer to store attribute data. */
static uint8_t m_attr_message[ATTR_DATA_SIZE];                             /**< Buffer to store attribute data. */
static uint8_t m_attr_message_size[ATTR_DATA_SIZE];                        /**< Buffer to store attribute data. */
static uint8_t m_attr_date[ATTR_DATA_SIZE];                                /**< Buffer to store attribute data. */
static uint8_t m_attr_posaction[ATTR_DATA_SIZE];                           /**< Buffer to store attribute data. */
static uint8_t m_attr_negaction[ATTR_DATA_SIZE];                           /**< Buffer to store attribute data. */

//static ble_uuid_t m_adv_uuids[] = {{BLE_UUID_NUS_SERVICE,BLE_UUID_TYPE_BLE}};  /**< Universally unique service identifiers. */
static ble_uuid_t m_adv_uuids[] = {{BLE_UUID_NUS_SERVICE, BLE_UUID_TYPE_BLE},{BLE_UUID_HEART_RATE_SERVICE, BLE_UUID_TYPE_BLE}};  /**< Universally unique service identifiers. */

extern uint8 ClearCRCFlag(void);
extern int32_t do_update(void);

/**@brief Callback function for handling asserts in the SoftDevice.
 *
 * @details This function is called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. 
 *          You must analyze how your product should react to asserts.
 * @warning On assert from the SoftDevice, the system can recover only on reset.
 *
 * @param[in] line_num   Line number of the failing ASSERT call.
 * @param[in] file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


void uart_error_handle(app_uart_evt_t * p_event)
{
    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_communication);
    }
    else if (p_event->evt_type == APP_UART_FIFO_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_code);
    }
}


/**@brief Function for handling the security request timer time-out.
 *
 * @details This function is called each time the security request timer expires.
 *
 * @param[in] p_context  Pointer used for passing context information from the
 *                       app_start_timer() call to the time-out handler.
 */
void sec_req_timeout_handler(void * p_context)
{
    uint32_t             err_code;
    dm_security_status_t status;

    if (m_conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        err_code = dm_security_status_req(&m_peer_handle, &status);
        APP_ERROR_CHECK(err_code);

        // If the link is still not secured by the peer, initiate security procedure.
        if (status == NOT_ENCRYPTED)
        {
                osal_start_timerEx(GetBleTaskId(), BLE_PAIRED_EVENT, 1000);
        }
        else
                osal_start_timerEx( GetBleTaskId(), BLE_KEEP_LINK_EVENT, 3000 );
    }
}


/**@brief Function for setting up GATTC notifications from the Notification Provider.
 *
 * @details This function is called when a successful connection has been established.
 */
void apple_notification_setup(void)
{
    uint32_t             err_code;

    if (m_conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        err_code = ble_ancs_c_notif_source_notif_enable(&m_ancs_c);
        APP_ERROR_CHECK(err_code);

        err_code = ble_ancs_c_data_source_notif_enable(&m_ancs_c);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for printing an iOS notification.
 *
 * @param[in] p_notif  Pointer to the iOS notification.
 */
static void notif_print(ble_ancs_c_evt_notif_t * p_notif)
{
	BleAncsAppMsg(p_notif);
}


/**@brief Function for printing iOS notification attribute data.
 * 
 * @param[in] p_attr         Pointer to an iOS notification attribute.
 * @param[in] ancs_attr_list Pointer to a list of attributes. Each entry in the list stores 
                             a pointer to its attribute data, which is to be printed.
 */
static void notif_attr_print(ble_ancs_c_evt_notif_attr_t * p_attr,
                             ble_ancs_c_attr_list_t      * ancs_attr_list)
{
        if((ancs_attr_list[p_attr->attr_id].p_attr_data != NULL)&&(p_attr->attr_len != 0))
        {
                GetNotifAttributesTitleInf(ancs_attr_list[p_attr->attr_id].p_attr_data,p_attr->attr_len);
        }
	 else
	 {
		  Send_AncsConnectedGetAllInfo();
	 }		
}


/**@brief Function for initializing the timer module.
 */
static void timers_init(void)
{

    // Initialize timer module, making it use the scheduler.
    APP_TIMER_APPSH_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, false);

}


/**@brief Function for handling the Apple Notification Service client.
 *
 * @details This function is called for all events in the Apple Notification client that
 *          are passed to the application.
 *
 * @param[in] p_evt  Event received from the Apple Notification Service client.
 */
static void on_ancs_c_evt(ble_ancs_c_evt_t * p_evt)
{

    switch (p_evt->evt_type)
    {
        case BLE_ANCS_C_EVT_DISCOVER_COMPLETE:
            osal_start_timerEx( GetBleTaskId(), BLE_CONNECTEING_EVENT, 2000 );
            break;

        case BLE_ANCS_C_EVT_NOTIF:
            m_notification_latest = p_evt->notif;
            notif_print(&m_notification_latest);
            break;

        case BLE_ANCS_C_EVT_NOTIF_ATTRIBUTE:
            notif_attr_print(&p_evt->attr, p_evt->ancs_attr_list);
            break;

        case BLE_ANCS_C_EVT_DISCOVER_FAILED:
            break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for initializing the GAP.
 *
 * @details Use this function to set up all necessary GAP (Generic Access Profile)
 *          parameters of the device. It also sets the permissions and appearance.
 */
static void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen((char *)DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the Apple Notification Service client errors.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void apple_notification_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = true;
    cp_init.evt_handler                    = NULL;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the Device Manager events.
 *
 * @param[in] p_evt  Data associated to the Device Manager event.
 */
static uint32_t device_manager_evt_handler(dm_handle_t const * p_handle,
                                           dm_event_t const  * p_evt,
                                           ret_code_t          event_result)
{
        if(event_result == NRF_SUCCESS)
        {
                ble_ancs_c_on_device_manager_evt(&m_ancs_c, p_handle, p_evt);

                switch (p_evt->event_id)
                {
                    case DM_EVT_CONNECTION:
                        m_peer_handle = (*p_handle);
                        break;

                    case DM_EVT_LINK_SECURED:
                        osal_start_timerEx( GetBleTaskId(), BLE_KEEP_LINK_EVENT, 3000 );
                        break;

                    default:
                        break;

                }
        }
        else if (event_result == DM_DEVICE_CONTEXT_FULL)
        {
                dm_device_delete_all(&m_app_handle);                                      
        }
        else if(event_result == BLE_GAP_SEC_STATUS_PASSKEY_ENTRY_FAILED)
        {
                dm_device_delete_all(&m_app_handle);
        } 
        else if(event_result == BLE_GAP_SEC_STATUS_UNSPECIFIED) //if get this error, delet bonding info, and disconnect
        {
                dm_device_delete_all(&m_app_handle);
        } 

        return NRF_SUCCESS;
}


/**@brief Function for initializing the Device Manager.
 *
 * @param[in] erase_bonds  Indicates whether bonding information should be cleared from
 *                         persistent storage during initialization of the Device Manager.
 */
static void device_manager_init(bool erase_bonds)
{
    uint32_t               err_code;
    dm_init_param_t        init_param = {.clear_persistent_data = erase_bonds};
    dm_application_param_t register_param;

    // Initialize persistent storage module.
    err_code = pstorage_init();
    APP_ERROR_CHECK(err_code);

    err_code = dm_init(&init_param);
    APP_ERROR_CHECK(err_code);

    memset(&register_param.sec_param, 0, sizeof(ble_gap_sec_params_t));

    register_param.sec_param.bond         = SEC_PARAM_BOND;
    register_param.sec_param.mitm         = SEC_PARAM_MITM;
    register_param.sec_param.io_caps      = SEC_PARAM_IO_CAPABILITIES;
    register_param.sec_param.oob          = SEC_PARAM_OOB;
    register_param.sec_param.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
    register_param.sec_param.max_key_size = SEC_PARAM_MAX_KEY_SIZE;
    register_param.evt_handler            = device_manager_evt_handler;
    register_param.service_type           = DM_PROTOCOL_CNTXT_GATT_SRVR_ID;

    memcpy(&m_sec_param, &register_param.sec_param, sizeof(ble_gap_sec_params_t));

    err_code = dm_register(&m_app_handle, &register_param);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling advertising events.
 *
 * @details This function is called for advertising events that are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    uint32_t err_code;

    switch (ble_adv_evt)
    {
    case BLE_ADV_EVT_DIRECTED:
        break;

    case BLE_ADV_EVT_FAST:
        break;

    case BLE_ADV_EVT_FAST_WHITELIST:
        break;

    case BLE_ADV_EVT_SLOW:
        break;

    case BLE_ADV_EVT_IDLE:
        ble_advertising_start(BLE_ADV_MODE_SLOW);

        break;

    case BLE_ADV_EVT_WHITELIST_REQUEST:
    {
        ble_gap_whitelist_t whitelist;
        ble_gap_addr_t     *p_whitelist_addr[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];
        ble_gap_irk_t      *p_whitelist_irk[BLE_GAP_WHITELIST_IRK_MAX_COUNT];

        whitelist.addr_count = BLE_GAP_WHITELIST_ADDR_MAX_COUNT;
        whitelist.irk_count  = BLE_GAP_WHITELIST_IRK_MAX_COUNT;
        whitelist.pp_addrs   = p_whitelist_addr;
        whitelist.pp_irks    = p_whitelist_irk;

        err_code = dm_whitelist_create(&m_app_handle, &whitelist);
        APP_ERROR_CHECK(err_code);

        err_code = ble_advertising_whitelist_reply(&whitelist);
        APP_ERROR_CHECK(err_code);
        break;
    }

    default:
        break;
    }
}


/**@brief Function for handling the application's BLE stack events.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void on_ble_evt(ble_evt_t * p_ble_evt)
{
    uint32_t err_code = NRF_SUCCESS;

    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;

        //连接上发现服务
		osal_start_timerEx(GetBleTaskId(), BLE_ANCS_SERVICE_FIND_EVENT,2000);


        break;

    case BLE_GAP_EVT_DISCONNECTED:
	osal_stop_timerEx(GetBleTaskId(), BLE_CONNECTEING_EVENT);
        osal_stop_timerEx(GetBleTaskId(), BLE_PAIRED_EVENT);
        osal_stop_timerEx(GetBleTaskId(), BLE_KEEP_LINK_EVENT);
        break;

    case BLE_GATTC_EVT_TIMEOUT:
    case BLE_GATTS_EVT_TIMEOUT:
        // Disconnect on GATT Server and Client time-out events.
        err_code = sd_ble_gap_disconnect(m_conn_handle,
                                         BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);

        APP_ERROR_CHECK(err_code);
        break;

    default:
        // No implementation needed.
        break;
    }
    APP_ERROR_CHECK(err_code);
}


void Ancs_Discovery_Restart(void)
{
	//连接上发现服务
	ble_db_discovery_start(&m_ble_db_discovery, m_conn_handle);
}


/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the BLE Stack event interrupt handler after a BLE stack
 *          event has been received.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    dm_ble_evt_handler(p_ble_evt);
    ble_db_discovery_on_ble_evt(&m_ble_db_discovery, p_ble_evt);
    ble_conn_params_on_ble_evt(p_ble_evt);
    usr_ble_hrs_on_ble_evt(p_ble_evt);
    ble_nus_on_ble_evt(&m_nus, p_ble_evt);
    ble_ancs_c_on_ble_evt(&m_ancs_c, p_ble_evt);
    bsp_btn_ble_on_ble_evt(p_ble_evt);
    on_ble_evt(p_ble_evt);
    ble_ccids_on_ble_evt(p_ble_evt);
    ble_media_on_ble_evt(p_ble_evt);	
    ble_advertising_on_ble_evt(p_ble_evt);
}


/**@brief Function for dispatching a system event to interested modules.
 *
 * @details This function is called from the system event interrupt handler after a system
 *          event has been received.
 *
 * @param[in] sys_evt  System stack event.
 */
static void sys_evt_dispatch(uint32_t sys_evt)
{
    pstorage_sys_event_handler(sys_evt);
    //app_beacon_scanner_sd_evt_signal_handler(sys_evt);
    //app_beacon_sd_evt_signal_handler(sys_evt);
    ble_advertising_on_sys_evt(sys_evt);
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    uint32_t err_code;

    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM, NULL);

    // Enable BLE stack.
    ble_enable_params_t ble_enable_params;
    memset(&ble_enable_params, 0, sizeof(ble_enable_params));
#ifdef S130
    ble_enable_params.gatts_enable_params.attr_tab_size   = BLE_GATTS_ATTR_TAB_SIZE_DEFAULT;
#endif
    err_code = sd_ble_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);

    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);

    // Register with the SoftDevice handler module for System events.
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the Apple Notification Center Service.
*/
static void service_init(void)
{
    ble_ancs_c_init_t ancs_init_obj;
    ble_uuid_t        service_uuid;
    ble_nus_init_t      nus_init;
    uint32_t          err_code;

    err_code = sd_ble_uuid_vs_add(&ble_ancs_base_uuid128, &m_ancs_uuid_type);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_uuid_vs_add(&ble_ancs_cp_base_uuid128, &service_uuid.type);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_uuid_vs_add(&ble_ancs_ns_base_uuid128, &service_uuid.type);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_uuid_vs_add(&ble_ancs_ds_base_uuid128, &service_uuid.type);
    APP_ERROR_CHECK(err_code);

    memset(&ancs_init_obj, 0, sizeof(ancs_init_obj));

    err_code = ble_ancs_c_attr_add(BLE_ANCS_NOTIF_ATTR_ID_TITLE, m_attr_title, ATTR_DATA_SIZE);
    APP_ERROR_CHECK(err_code);
    
    err_code = ble_ancs_c_attr_add(BLE_ANCS_NOTIF_ATTR_ID_SUBTITLE,
                                   m_attr_subtitle,
                                   ATTR_DATA_SIZE);
    APP_ERROR_CHECK(err_code);

    err_code = ble_ancs_c_attr_add(BLE_ANCS_NOTIF_ATTR_ID_MESSAGE, m_attr_message, ATTR_DATA_SIZE);
    APP_ERROR_CHECK(err_code);

    err_code = ble_ancs_c_attr_add(BLE_ANCS_NOTIF_ATTR_ID_MESSAGE_SIZE,
                                   m_attr_message_size,
                                   ATTR_DATA_SIZE);
    APP_ERROR_CHECK(err_code);

    err_code = ble_ancs_c_attr_add(BLE_ANCS_NOTIF_ATTR_ID_DATE, m_attr_date, ATTR_DATA_SIZE);
    APP_ERROR_CHECK(err_code);

    err_code = ble_ancs_c_attr_add(BLE_ANCS_NOTIF_ATTR_ID_POSITIVE_ACTION_LABEL,
                                   m_attr_posaction,
                                   ATTR_DATA_SIZE);
    APP_ERROR_CHECK(err_code);

    err_code = ble_ancs_c_attr_add(BLE_ANCS_NOTIF_ATTR_ID_APP_IDENTIFIER,
                                   m_attr_negaction,
                                   ATTR_DATA_SIZE);
    APP_ERROR_CHECK(err_code);

    ancs_init_obj.evt_handler   = on_ancs_c_evt;
    ancs_init_obj.error_handler = apple_notification_error_handler;

    err_code = ble_ancs_c_init(&m_ancs_c, &ancs_init_obj);
    APP_ERROR_CHECK(err_code);


    /*    BLE  数据通信加载*/
    
    m_nus.uuid_type = BLE_UUID_TYPE_BLE;
    nus_init.data_handler = nus_data_handler;

    err_code = ble_nus_init(&m_nus, &nus_init);
    APP_ERROR_CHECK(err_code);

    err_code = ble_ccids_init();
    APP_ERROR_CHECK(err_code);
	
    err_code = ble_media_init();
    APP_ERROR_CHECK(err_code);

    usr_ble_hrs_service_init();
		
}


/**@brief Function for initializing the advertising functionality.
 */
static void advertising_init(void)
{
    uint32_t      err_code;
    ble_advdata_t advdata;

     // Build and set advertising data
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type                = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance       = false;
    advdata.flags                    = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    advdata.uuids_complete.p_uuids  = m_adv_uuids;

    ble_adv_modes_config_t options = {0};
    options.ble_adv_whitelist_enabled = BLE_ADV_WHITELIST_DISABLED;
    options.ble_adv_fast_enabled      = BLE_ADV_FAST_ENABLED;
    options.ble_adv_fast_interval     = APP_ADV_FAST_INTERVAL;
    options.ble_adv_fast_timeout      = APP_ADV_FAST_TIMEOUT;
    options.ble_adv_slow_enabled      = BLE_ADV_SLOW_ENABLED;
    options.ble_adv_slow_interval     = APP_ADV_SLOW_INTERVAL;
    options.ble_adv_slow_timeout      = APP_ADV_SLOW_TIMEOUT;

    err_code = ble_advertising_init(&advdata, NULL, &options, on_adv_evt, NULL);
    APP_ERROR_CHECK(err_code);
}


//用watchid当做唯一标示
static void MakeBtDeviceName(void)
{
        uint8 ReadWatchIdTemp[WATCH_ID_LEN]={0};
        uint8 i,temp;
		        
        ReadWatchId(ReadWatchIdTemp);
        temp=6;
        for(i=15;i<20;i++)
        {
                DEVICE_NAME[temp]=ReadWatchIdTemp[i];
                temp++;
        }
        DEVICE_NAME[temp]=0;
}


//验证程序是否在有效范围
static void VerifyProgramData(void)
{
        uint8 ReadWatchIdTemp[WATCH_ID_LEN]={0};

        PROGRAM_DATA_VALID_FLAG=0;
        ReadWatchId(ReadWatchIdTemp);

        if(VerifyWatchId(ReadWatchIdTemp, WATCH_ID_LEN))
        {
                PROGRAM_DATA_VALID_FLAG=1;
        }

}


/**@brief Function for initializing buttons and LEDs.
 *
 * @param[out] p_erase_bonds  True if the clear bonds button was pressed to wake the application up.
 */
static void buttons_leds_init(bool * p_erase_bonds)
{
}


/**@brief Function for initializing the Event Scheduler.
 */
static void scheduler_init(void)
{
        MakeBtDeviceName();
}


/**@brief Function for initializing the database discovery module.
 */
static void db_discovery_init(void)
{
    uint32_t err_code = ble_db_discovery_init();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for the Power manager.
 */
void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}

void power_reset_pin_config(void)
{
	NRF_POWER->RESET = 0x1;
}

void CheckUpdateTP(void)
{
	uint8_t buffer[5] = {0x00, 0x00, 0x00, 0x00, '\0'}; 
	uint8_t TAG[5] = "UPTP";   	
	uint32_t fw_size = 0;
	READ_OTA_INFO(buffer,OTA_TP_TAG_LOC,OTA_TP_TAG_LEN);
	if (strcmp((const char *)buffer, (const char *)TAG) == 0)
	{
		CypressI2CInit();
		READ_OTA_INFO(buffer,OTA_TP_SIZE_LOC,OTA_TP_SIZE_LEN);
		fw_size = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | (buffer[3]); 
		if(ts_upgrade_firmware(Ota_Tp_Base,fw_size)==1)
		{
			memset(buffer,0,5);
			WRITE_OTA_INFO(buffer,OTA_TP_TAG_LOC,OTA_TP_TAG_LEN);
			UpdateTPSuccess = true;
		}
	}
}

void CheckUpdateKl17(void)
{
	uint8_t	buffer[5] = {0x00, 0x00, 0x00, 0x00, '\0'}; 
	uint8_t TAG[5] = "KL17";
	uint8_t retries = 3;
	int32_t status = -1;
	    		
	READ_OTA_INFO(buffer,OTA_KL17_TAG_LOC,OTA_KL17_TAG_LEN);
	if (strcmp((const char *)buffer, (const char *)TAG) == 0)
	{
		KL17_Power_Enble();
		bootloader_init();	//bootloader init

		while(retries--)
		{
			status = do_update();
			if (status == 0)
			{
				memset(buffer,0,5);
				WRITE_OTA_INFO(buffer,OTA_KL17_TAG_LOC,OTA_KL17_TAG_LEN);				
				UpdateKl17Success = true;
				break;
			}
		}	
	}
	Update_flags=false;
}


void CheckUpdateAS7000(void)
{
	uint8_t	buffer[5] = {0x00, 0x00, 0x00, 0x00, '\0'}; 
	uint8_t TAG[5] = "UPHR";
	uint8_t retries = 3;
	uint32_t status = 1;
	    
	READ_OTA_INFO(buffer,OTA_HR_TAG_LOC,OTA_HR_TAG_LEN);
	if (strcmp((const char *)buffer, (const char *)TAG) == 0)
	{
		while(retries--)
		{
			status = AS7000_upgrade(1);
			if (status == 1)
			{   
				memset(buffer,0,5);
				WRITE_OTA_INFO(buffer,OTA_HR_TAG_LOC,OTA_HR_TAG_LEN);					
				UpdateAS7000Success = true;
				break;
			}
			else
			{
				nrf_delay_ms(3000);
			}
				AS7000_PowerOFF();
		}	
	}
}

//验证Gsensor的类型
void CheckGsensorType(void)
{
	uint8 sType =0;
	for(uint8 i=0; i<3;i++)
	{
		UnlockI2c();	
		nrf_delay_ms(5);
		I2cMultipleByteRead(LIS3DH_I2C_ADDR<<1, LIS3DH_WHO_AM_I, &sType, 1);
		if(sType==0x33)
		{
			GsensorType = SENSOR_LIS3DH;
			Lis3dh_Init();	
			break;	
		}
		
		sType =0x00;
		UnlockI2c();	
		nrf_delay_ms(5);
		I2cMultipleByteRead(BMA2x2_I2C_ADDR<<1, BMA2x2_CHIP_ID__REG, &sType, 1);
		//if(sType==0xF9)
		{
			GsensorType = SENSOR_BMA250E;
			Bma250E_Init();
			break;
		}			
		
	}
	//PRINTF("GsensorType=%d (0x%x) \r\n",GsensorType,sType);
}


/**@brief Function for application main entry.
 */
int main(void)
{
	uint8_t buffer[5] = {0x00, 0x00, 0x00, 0x00, '\0'}; 
	bool     erase_bonds;
	uint32_t err_code;
	UTCTimeStruct uTime;
	power_reset_pin_config();
	DisableReset();	
	nrf_delay_ms(600);			/*waitfor KL17 POWERUP*/
	
	BootloaderUartInitial();
	SpiFlash_Init();				/*Nordic占用Flash使用权,检测OTA,----禁止KL17的外部flash图片显示*/
	Get_Ota_Addr();	
	OtaFlag = ReadOtaSuccessFlag();
	if (OtaFlag == 1)
	{
		CheckUpdateTP();
		CheckUpdateKl17();
		CheckUpdateAS7000();
		ShowOtaResult(0,OtaFlag);
	}
	else {
		memset(buffer,0,5);
		WRITE_OTA_INFO(buffer,OTA_TP_TAG_LOC,OTA_TP_TAG_LEN);
		WRITE_OTA_INFO(buffer,OTA_KL17_TAG_LOC,OTA_KL17_TAG_LEN);				
		WRITE_OTA_INFO(buffer,OTA_HR_TAG_LOC,OTA_HR_TAG_LEN);					
	}
	EraseOtaSuccessFlag();	
	
	DisableKl17();				/*重启KL17*/	/*waitfor KL17 POWERUP*/	
	UartInitial();
	spi_init();
	Free_NordicFlash();   		/*Nordic让出Flash使用权,---允许KL17的外部flash图片显示*/
	ShowOtaResult(1,OtaFlag);
	Get_KL17_Version();
	
	Get_HR_AS7000_Version();
	
	gOS_GetUTCTime(&uTime);
	LCD_UpdateTime(&uTime);
	UserInfoInit();
	StoreDataInit();
	InitMotorVibrate();
	DisplayLogo(true);

	ClearAncsFlash();
	InitAncsSmsParameter();
	//SecondsTimer_init();
	app_trace_init();
	timers_init();
	
	buttons_leds_init(&erase_bonds);
	ble_stack_init();
	device_manager_init(erase_bonds);
	//Scanner_Parse_init();
	//beacon_adv_init();
	db_discovery_init();
	scheduler_init();
	gap_params_init();
	service_init();
	advertising_init();
	conn_params_init();
	//scanner_init();
	err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
	APP_ERROR_CHECK(err_code);
	pS_OsInitSystem();
	//时钟初始化函数，定义了时钟更新函数Clock_second_handle，每次定时时间到的时候，更新系统计数时钟
	Clock_init();
	ResetRtcClock();
	//定时器开始函数，定时周期20ms
	Clock_start();
	RemindsInit();
	PowerStateInit();
	//InitMotorVibrate();
	GetBatVelue();
	CypressI2CInit();
	for(uint8 i=0; i<8;i++) 
		UnlockI2c();
	CheckGsensorType();
	StepInit();
	ClockEvtInit();
	ClearCRCFlag();	
	VerifyProgramData();
	Gpio_Interrupt_Initial();
	StartKeyScan(); 
	UpdateBleStatus();

	if(BatChargeInit())
		CheckUsbInDebounce();
	Bc5PowerPinInit();	
	// Enter main loop.
	for (;;)
	{
		pS_OsRunSystem();
	}

}


/**
 * @}
 */

