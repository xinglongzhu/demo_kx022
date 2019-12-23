/************************************************************************************************************
**	FileName:	bt_peripheral.c
**	Description:	
--------------------------------------------------------------------------------------------------------					
**	Company:	    Radioland
--------------------------------------------------------------------------------------------------------
**	Modified:
**	Author 			Date 			 Comment
_________________________________________________________________________________________________________
**	Long     	    2019-01-11		 Original
**
**
************************************************************************************************************/
#include "bt.h"
#include "bt_peripheral.h"
#include "hw_debug.h"
/*******************************************************************************************************************
**	public Parameters
********************************************************************************************************************/




/*******************************************************************************************************************
**	Hardware  Defined
********************************************************************************************************************/
#define APP_BLE_CONN_CFG_TAG            	1 

#define DEVICE_NAME                     	"demo_kx022"  

#define APP_ADV_INTERVAL                    160                                         /**< The advertising interval (in units of 0.625 ms).*/
#define APP_ADV_TIMEOUT_IN_SECONDS           0                                       /**< The advertising duration (180 seconds) in units of 10 milliseconds. */

/*lint -emacro(524, MIN_CONN_INTERVAL) // Loss of precision */
#define MIN_CONN_INTERVAL                   MSEC_TO_UNITS(15, UNIT_1_25_MS)           /**< Minimum connection interval (15 ms) */
#define MAX_CONN_INTERVAL                   MSEC_TO_UNITS(15, UNIT_1_25_MS)            /**< Maximum connection interval (15 ms). */

#define SLAVE_LATENCY                       0                                          /**< Slave latency. */

#define CONN_SUP_TIMEOUT                    MSEC_TO_UNITS(1000, UNIT_10_MS)             /**< Connection supervisory timeout (1000 ms). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY      APP_TIMER_TICKS(5000)                      /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY       APP_TIMER_TICKS(30000)                     /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT        3                                          /**< Number of attempts before giving up the connection parameter negotiation. */

//auto write
#define APP_FEATURE_NOT_SUPPORTED           BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2        /**< Reply when unsupported features are requested. */





/*******************************************************************************************************************
**	public Parameters
********************************************************************************************************************/
ble_advertising_t m_advertising;
BLE_ADVERTISING_DEF(m_advertising);                                 /**< Advertising module instance. */
uint16_t          m_conn_handle  = BLE_CONN_HANDLE_INVALID;        /**< Handle of the current connection. */

bool connect_flag = false;
/*******************************************************************************************************************
**	static Parameters
********************************************************************************************************************/
static ble_uuid_t m_adv_uuids[] = {{BLE_UUID_HUMAN_INTERFACE_DEVICE_SERVICE, BLE_UUID_TYPE_BLE}};


/*******************************************************************************************************************
**	function 
********************************************************************************************************************/



/**@brief Function for starting advertising.
 */
void advertising_start(void)
{
    uint32_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
void conn_params_init(void)
{
    ret_code_t             err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = NULL;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}




/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
 void bt_peripheral_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    ret_code_t err_code;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            LogD("Connected\r\n");
			connect_flag = true;
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break;

        case BLE_GAP_EVT_DISCONNECTED:
			connect_flag = false;
            LogD("BLE_GAP_EVT_DISCONNECTED reson=%d\r\n",p_ble_evt->evt.gap_evt.params.disconnected.reason);
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            break;
		
        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
		{
            LogD("PHY update request.\r\n");
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
		}
			break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            // Pairing not supported
            err_code = sd_ble_gap_sec_params_reply(m_conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            // No system attributes have been stored.
            err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
            APP_ERROR_CHECK(err_code);
            break;		

			
        case BLE_GATTS_EVT_HVN_TX_COMPLETE:
            break;

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            LogD("GATT Client Timeout.\r\n");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            LogD("GATT Server Timeout.\r\n");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;
        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
        {
            ble_gatts_evt_rw_authorize_request_t  req;
            ble_gatts_rw_authorize_reply_params_t auth_reply;
			//LogD("Per BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST\n\r");

            req = p_ble_evt->evt.gatts_evt.params.authorize_request;

            if (req.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)
            {
                if ((req.request.write.op == BLE_GATTS_OP_PREP_WRITE_REQ)     ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_NOW) ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL))
                {
                    if (req.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)
                    {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
                    }
                    else
                    {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
                    }
                    auth_reply.params.write.gatt_status = APP_FEATURE_NOT_SUPPORTED;
                    err_code = sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                               &auth_reply);
                    APP_ERROR_CHECK(err_code);
                }  
			}
			else if ( req.type == BLE_GATTS_AUTHORIZE_TYPE_READ  )
			{
				//LogD("\r\nBLE_GATTS_AUTHORIZE_TYPE_READ\r\n");

					auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
					//uint16_t handleRead=p_ble_evt->evt.gatts_evt.params.authorize_request.request.read.handle; 
																	
					auth_reply.params.read.gatt_status=BLE_GATT_STATUS_SUCCESS;	
					auth_reply.params.read.update = 0;			
					err_code=sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gatts_evt.conn_handle,&auth_reply);
					//LogD("conn_handle=%x\r\n",p_ble_evt->evt.gatts_evt.conn_handle);
				   // LogD("handleRead=%x\r\n",handleRead);
					//LogD("read_offset=%x\r\n",p_ble_evt->evt.gatts_evt.params.authorize_request.request.read.offset);
					//LogD("update=%x\r\n",auth_reply.params.read.update);
					//APP_ERROR_CHECK(err_code);
                    if(NRF_SUCCESS != err_code)
                    {
                      NRF_LOG_INFO("err_code=%x\r\n",err_code);
                    }

				
			}			
       
        }
        break; // BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST		

        default:
            // No implementation needed.
            break;
    }
}

/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
void gap_params_init(void)
{
    ret_code_t              err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_HID_GAMEPAD);
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling advertising errors.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void ble_advertising_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}
/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            LogD("BLE_ADV_EVT_FAST.\r\n");
            break;

        case BLE_ADV_EVT_SLOW:
            LogD("BLE_ADV_EVT_SLOW.\r\n");
            break;

        case BLE_ADV_EVT_IDLE:
			LogD("BLE_ADV_EVT_IDLE.\r\n");
            break;

        default:
            break;
    }
}
/**@brief Function for initializing the Advertising functionality.
 */
void advertising_init(void)
{
    uint32_t               err_code;
    uint8_t                adv_flags;
    ble_advertising_init_t init;

    memset(&init, 0, sizeof(init));

    adv_flags                            = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    init.advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance      = true;
    init.advdata.flags                   = adv_flags;
    init.advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.advdata.uuids_complete.p_uuids  = m_adv_uuids;


    init.config.ble_adv_fast_enabled               = true;	
    init.config.ble_adv_fast_interval              = APP_ADV_INTERVAL;
    init.config.ble_adv_fast_timeout               = APP_ADV_TIMEOUT_IN_SECONDS;

    init.evt_handler   = on_adv_evt;
    init.error_handler = ble_advertising_error_handler;

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}




