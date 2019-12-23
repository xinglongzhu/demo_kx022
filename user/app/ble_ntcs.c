#include "ble_ntcs.h"
#include "sdk_common.h"
#include "ble.h"
#include "ble_srv_common.h"
#include "bt.h"
#include "hw_debug.h"

#define BLE_NTCS_MAX_RX_CHAR_LEN        BLE_NTCS_MAX_DATA_LEN 
#define BLE_NTCS_MAX_TX_CHAR_LEN        BLE_NTCS_MAX_DATA_LEN 
//                                      uuid£º00000000-494c-4f47-4943-544543480000
#define NTCS_BASE_UUID                  {{0x00, 0x00, 0x48, 0x43, 0x45, 0x54, 0x43, 0x49, 0x47, 0x4f, 0x4c, 0x49, 0x00, 0x00, 0x00, 0x00}} 

static void on_connect(ble_ntcs_t * p_ntcs, ble_evt_t const * p_ble_evt)
{
    ret_code_t                 err_code;
    ble_ntcs_evt_t              evt;
    ble_gatts_value_t          gatts_val;
    uint8_t                    cccd_value[2];
    ble_ntcs_client_context_t * p_client = NULL;

    err_code = blcm_link_ctx_get(p_ntcs->p_link_ctx_storage, p_ble_evt->evt.gap_evt.conn_handle,(void *) &p_client);
    if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_ERROR("Link context for 0x%02X connection handle could not be fetched.",p_ble_evt->evt.gap_evt.conn_handle);
    }

    /* Check the hosts CCCD value to inform of readiness to send data using the RX characteristic */
    memset(&gatts_val, 0, sizeof(ble_gatts_value_t));
    gatts_val.p_value = cccd_value;
    gatts_val.len     = sizeof(cccd_value);
    gatts_val.offset  = 0;
    err_code = sd_ble_gatts_value_get(p_ble_evt->evt.gap_evt.conn_handle,p_ntcs->channel2_handles.cccd_handle,&gatts_val);
    if ((err_code == NRF_SUCCESS) && (p_ntcs->data_handler != NULL) && ble_srv_is_notification_enabled(gatts_val.p_value))
    {
        if (p_client != NULL)
        {			
            p_client->is_notification_enabled = true;
        }

        memset(&evt, 0, sizeof(ble_ntcs_evt_t));
        evt.type            = BLE_NTCS_EVT_COMM_STARTED;
        evt.p_ntcs          = p_ntcs;
        evt.conn_handle     = p_ble_evt->evt.gap_evt.conn_handle;
		p_ntcs->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
        evt.p_link_ctx      = p_client;

        p_ntcs->data_handler(&evt);
    }
}

static void on_write(ble_ntcs_t * p_ntcs, ble_evt_t const * p_ble_evt)
{
    ret_code_t                   err_code;
    ble_ntcs_evt_t               evt;
    ble_ntcs_client_context_t    * p_client;
    ble_gatts_evt_write_t const  * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    err_code = blcm_link_ctx_get(p_ntcs->p_link_ctx_storage,p_ble_evt->evt.gatts_evt.conn_handle,(void *) &p_client);
    if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_ERROR("Link context for 0x%02X connection handle could not be fetched.",p_ble_evt->evt.gatts_evt.conn_handle);
    }

    memset(&evt, 0, sizeof(ble_ntcs_evt_t));
    evt.p_ntcs       = p_ntcs;
    evt.conn_handle = p_ble_evt->evt.gatts_evt.conn_handle;
    evt.p_link_ctx  = p_client;
    //channel 2 have notify 
    if ((p_evt_write->handle == p_ntcs->channel2_handles.cccd_handle) &&(p_evt_write->len == 2))
    {
        if (p_client != NULL)
        {
            if (ble_srv_is_notification_enabled(p_evt_write->data))
            {
            	NRF_LOG_INFO("notification is enabled \r\n")
                p_client->is_notification_enabled = true;
                evt.type                          = BLE_NTCS_EVT_COMM_STARTED;
            }
            else
            {
            	NRF_LOG_INFO("notification is disabled \r\n")
                p_client->is_notification_enabled = false;
                evt.type                          = BLE_NTCS_EVT_COMM_STOPPED;
            }

            if (p_ntcs->data_handler != NULL)
            {
                p_ntcs->data_handler(&evt);
            }

        }
    }
	//channel 1  read     phone to ble
    else if((p_evt_write->handle == p_ntcs->channel1_handles.value_handle) &&(p_ntcs->data_handler != NULL))
    {
        evt.type                  = BLE_NTCS_EVT_CHANNEL1_GET_DATA;
        evt.params.rx_data.p_data = p_evt_write->data;
        evt.params.rx_data.length = p_evt_write->len;
        p_ntcs->data_handler(&evt);
		
    }

}



static void on_hvx_tx_complete(ble_ntcs_t * p_ntcs, ble_evt_t const * p_ble_evt)
{
    ret_code_t                  err_code;
    ble_ntcs_evt_t              evt;
    ble_ntcs_client_context_t * p_client;

    err_code = blcm_link_ctx_get(p_ntcs->p_link_ctx_storage,p_ble_evt->evt.gatts_evt.conn_handle,(void *) &p_client);
    if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_ERROR("Link context for 0x%02X connection handle could not be fetched.",p_ble_evt->evt.gatts_evt.conn_handle);
        return;
    }

    if (p_client->is_notification_enabled)
    {
        memset(&evt, 0, sizeof(ble_ntcs_evt_t));
        evt.type        = BLE_NTCS_EVT_TX_RDY;
        evt.p_ntcs      = p_ntcs;
        evt.conn_handle = p_ble_evt->evt.gatts_evt.conn_handle;
        evt.p_link_ctx  = p_client;

        p_ntcs->data_handler(&evt);
    }
}


void ble_ntcs_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    if ((p_context == NULL) || (p_ble_evt == NULL))
    {
        return;
    }

    ble_ntcs_t * p_ntcs = (ble_ntcs_t *)p_context;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_ntcs, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
            on_write(p_ntcs, p_ble_evt);
            break;

        case BLE_GATTS_EVT_HVN_TX_COMPLETE:
            on_hvx_tx_complete(p_ntcs, p_ble_evt);
            break;

        default:
            // No implementation needed.
            break;
    }
}

//default read write  write no reaponse  notify
void m_character_add(ble_add_char_params_t* add_char_params,ble_ntcs_t * p_ntcs,uint16_t uuid)
{
    memset(add_char_params, 0, sizeof(add_char_params));
    add_char_params->uuid                     = uuid;
    add_char_params->uuid_type                = p_ntcs->uuid_type;
    add_char_params->max_len                  = BLE_NTCS_MAX_DATA_LEN;
    add_char_params->init_len                 = sizeof(uint8_t);
    add_char_params->is_var_len               = true;
    add_char_params->char_props.write         = 1;
    add_char_params->char_props.write_wo_resp = 1;
	add_char_params->char_props.read          = 1;
	add_char_params->char_props.notify        = 1;

    add_char_params->read_access  = SEC_OPEN;
    add_char_params->write_access = SEC_OPEN;
	add_char_params->cccd_write_access = SEC_OPEN;
}


uint32_t ble_ntcs_init(ble_ntcs_t * p_ntcs, ble_ntcs_init_t const * p_ntcs_init)
{
    ret_code_t            err_code;
    ble_uuid_t            ble_uuid;
    ble_uuid128_t         ntcs_base_uuid = NTCS_BASE_UUID;
    ble_add_char_params_t add_char_params;

    VERIFY_PARAM_NOT_NULL(p_ntcs);
    VERIFY_PARAM_NOT_NULL(p_ntcs_init);

    // Initialize the service structure.
    p_ntcs->data_handler = p_ntcs_init->data_handler;

    /**@snippet [Adding proprietary Service to the SoftDevice] */
    // Add a custom base UUID.
    err_code = sd_ble_uuid_vs_add(&ntcs_base_uuid, &p_ntcs->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_ntcs->uuid_type;
    ble_uuid.uuid = BLE_UUID_NTCS_SERVICE;

    // Add the service.
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &ble_uuid,
                                        &p_ntcs->service_handle);
    /**@snippet [Adding proprietary Service to the SoftDevice] */
    VERIFY_SUCCESS(err_code);
	memset(&add_char_params, 0, sizeof(add_char_params));
	m_character_add(&add_char_params,p_ntcs,BLE_UUID_NTCS_CHANNEL1_CHARACTERISTIC);
	add_char_params.char_props.read          = 0;
	add_char_params.char_props.notify        = 0;
	add_char_params.cccd_write_access        = SEC_NO_ACCESS;
    err_code = characteristic_add(p_ntcs->service_handle, &add_char_params, &p_ntcs->channel1_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	memset(&add_char_params, 0, sizeof(add_char_params));	
	m_character_add(&add_char_params,p_ntcs,BLE_UUID_NTCS_CHANNEL2_CHARACTERISTIC);
	add_char_params.char_props.write         = 0;
    add_char_params.char_props.write_wo_resp = 0;
	add_char_params.char_props.read          = 0;

    return characteristic_add(p_ntcs->service_handle, &add_char_params, &p_ntcs->channel2_handles);
    /**@snippet [Adding proprietary characteristic to the SoftDevice] */
}


//send notify 
uint32_t ble_ntcs_notify_send(ble_ntcs_t* p_ntcs, uint8_t* p_data, uint16_t p_length,uint16_t uuid)
{
    ret_code_t                 err_code;
    ble_gatts_hvx_params_t     hvx_params;
    ble_ntcs_client_context_t * p_client;

    VERIFY_PARAM_NOT_NULL(p_ntcs);
    err_code = blcm_link_ctx_get(p_ntcs->p_link_ctx_storage, p_ntcs->conn_handle, (void *) &p_client);
    VERIFY_SUCCESS(err_code);

    if ((p_ntcs->conn_handle == BLE_CONN_HANDLE_INVALID) || (p_client == NULL))
    {
        return NRF_ERROR_NOT_FOUND;
    }

    if (!p_client->is_notification_enabled)
    {
        return NRF_ERROR_INVALID_STATE;
    }

    if (p_length > BLE_NTCS_MAX_DATA_LEN)
    {
        return NRF_ERROR_INVALID_PARAM;
    }
    memset(&hvx_params, 0, sizeof(hvx_params));
	if(uuid == BLE_UUID_NTCS_CHANNEL2_CHARACTERISTIC )
	{
		hvx_params.handle = p_ntcs->channel2_handles.value_handle;	
	}
    hvx_params.p_data = p_data;
    hvx_params.p_len  = &p_length;
    hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;

    return sd_ble_gatts_hvx(p_ntcs->conn_handle, &hvx_params);
}

