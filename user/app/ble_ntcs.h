#ifndef BLE_NTCS_H__
#define BLE_NTCS_H__

#include <stdint.h>
#include <stdbool.h>
#include "sdk_config.h"
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"
#include "ble_link_ctx_manager.h"

#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************************************************************
**	Hardware  Defined
********************************************************************************************************************/
#define BLE_NTCS_DEF(_name, _ntcs_max_clients)                      \
    BLE_LINK_CTX_MANAGER_DEF(CONCAT_2(_name, _link_ctx_storage),  \
                             (_ntcs_max_clients),                  \
                             sizeof(ble_ntcs_client_context_t));   \
    static ble_ntcs_t _name =                                      \
    {                                                             \
        .p_link_ctx_storage = &CONCAT_2(_name, _link_ctx_storage) \
    };                                                            \
    NRF_SDH_BLE_OBSERVER(_name ## _obs,                           \
                         2,               \
                         ble_ntcs_on_ble_evt,                      \
                         &_name)
                         
#define BLE_UUID_NTCS_SERVICE                 0x1803 /**< The UUID of the Radioland ntc Service. */
#define BLE_UUID_NTCS_CHANNEL2_CHARACTERISTIC 0x1804              
#define BLE_UUID_NTCS_CHANNEL1_CHARACTERISTIC 0x1805 

	
#define OPCODE_LENGTH        1
#define HANDLE_LENGTH        2
/**@brief   Maximum length of data (in bytes) that can be transmitted to the peer by the Radioland ntc service module. */
#if defined(NRF_SDH_BLE_GATT_MAX_MTU_SIZE) && (NRF_SDH_BLE_GATT_MAX_MTU_SIZE != 0)
    #define BLE_NTCS_MAX_DATA_LEN (NRF_SDH_BLE_GATT_MAX_MTU_SIZE - OPCODE_LENGTH - HANDLE_LENGTH)
#else
    #define BLE_NTCS_MAX_DATA_LEN (BLE_GATT_MTU_SIZE_DEFAULT - OPCODE_LENGTH - HANDLE_LENGTH)
    #warning NRF_SDH_BLE_GATT_MAX_MTU_SIZE is not defined.
#endif


typedef enum
{
    BLE_NTCS_EVT_CHANNEL1_GET_DATA,      /**< Data received. */
    BLE_NTCS_EVT_TX_RDY,       			 /**< Service is ready to accept new data to be transmitted. */
    BLE_NTCS_EVT_COMM_STARTED, 			 /**< Notification has been enabled. */
    BLE_NTCS_EVT_COMM_STOPPED, 			 /**< Notification has been disabled. */
} ble_ntcs_evt_type_t;

typedef struct
{
    uint8_t const * p_data; /**< A pointer to the buffer with received data. */
    uint16_t        length; /**< Length of received data. */
} ble_ntcs_evt_rx_data_t;

typedef struct ble_ntcs_s ble_ntcs_t;

typedef struct
{
    bool is_notification_enabled; /**< Variable to indicate if the peer has enabled notification of the RX characteristic.*/
} ble_ntcs_client_context_t;


typedef struct
{
    ble_ntcs_evt_type_t         type;        /**< Event type. */
    ble_ntcs_t                * p_ntcs;       /**< A pointer to the instance. */
    uint16_t                   conn_handle; /**< Connection handle. */
    ble_ntcs_client_context_t * p_link_ctx;  /**< A pointer to the link context. */
    union
    {
        ble_ntcs_evt_rx_data_t rx_data; 
    } params;
} ble_ntcs_evt_t;

typedef void (* ble_ntcs_data_handler_t) (ble_ntcs_evt_t * p_evt);


typedef struct
{
    ble_ntcs_data_handler_t data_handler; /**< Event handler to be called for handling received data. */
} ble_ntcs_init_t;


struct ble_ntcs_s
{
    uint8_t                         uuid_type;          
    uint16_t                        service_handle;     
    ble_gatts_char_handles_t        channel1_handles;         
    ble_gatts_char_handles_t        channel2_handles;         
    blcm_link_ctx_storage_t * const p_link_ctx_storage;
    ble_ntcs_data_handler_t         data_handler; 
	uint16_t                        conn_handle;
};
/*******************************************************************************************************************
**	function 
********************************************************************************************************************/
uint32_t ble_ntcs_init(ble_ntcs_t * p_ntcs, ble_ntcs_init_t const * p_ntcs_init);
void ble_ntcs_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);
uint32_t ble_ntcs_notify_send(ble_ntcs_t* p_ntcs, uint8_t* p_data, uint16_t p_length,uint16_t uuid);
#ifdef __cplusplus
}
#endif


#endif // BLE_NTCS_H__

/** @} */
