/************************************************************************************************************
**	FileName:	bt.c
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
#include "hw_debug.h"
#include "config.h"
#include "bt_peripheral.h"
#include "task.h"
#include "bt_server.h"
/*******************************************************************************************************************
**	Hardware  Defined
********************************************************************************************************************/
//stack
#define APP_BLE_OBSERVER_PRIO               3    /**< Application's BLE observer priority. You shouldn't need to modify this value. */
/*******************************************************************************************************************
**	public Parameters
********************************************************************************************************************/
NRF_BLE_GATT_DEF(m_gatt);                        /**< GATT module instance. */
/*******************************************************************************************************************
**	static Parameters
********************************************************************************************************************/





/*******************************************************************************************************************
**	function 
********************************************************************************************************************/
void low_power_init(void)
{
	sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
	NRF_POWER-> TASKS_LOWPWR = 1;
	NRF_POWER->DCDCEN = 1;	
}

/**@brief Function for initializing the nrf log module.
 */
void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/**@brief Function for initializing the GATT module.
 */
void gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing power management.
 */
void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for the SoftDevice initialization.
 *
 * @details This function initializes the SoftDevice and the BLE event interrupt.
 */
void ble_stack_init(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, bt_peripheral_evt_handler, NULL);
}

void ble_init(void)
{
	low_power_init();
    log_init();
	timers_init();
	power_management_init();
	ble_stack_init();
    gap_params_init();
    gatt_init();
    advertising_init();
	services_init();	
	conn_params_init();
}


