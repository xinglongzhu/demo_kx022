/************************************************************************************************************
**	FileName:	ntc_protocol.c
**	Description:	
--------------------------------------------------------------------------------------------------------					
**	Company:	    Radioland
--------------------------------------------------------------------------------------------------------
**	Modified:
**	Author 			Date 			 Comment
_________________________________________________________________________________________________________
**	Long     	    2019-01-15		 Original
************************************************************************************************************/
#include "ntc_protocol.h"
#include "bt_server.h"
#include "bt.h"
#include "ble_ntcs.h"
#include "hw_debug.h"

void ntcs_data_handler(ble_ntcs_evt_t * p_evt)
{
	uint8_t ntc_data[20];
	if (p_evt->type == BLE_NTCS_EVT_CHANNEL1_GET_DATA)
    {
		memset(ntc_data, 0, 20);
		memcpy(ntc_data, p_evt->params.rx_data.p_data, p_evt->params.rx_data.length);
		dump(ntc_data,20);
		ble_ntcs_notify_send(&m_ntcs, ntc_data, 20, BLE_UUID_NTCS_CHANNEL2_CHARACTERISTIC);
	}

}







