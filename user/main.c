#include <stdint.h>
#include <string.h>
#include "kx022.h"
#include "task.h"
//log
#include "hw_debug.h"
#include "bt.h"
#include "bt_peripheral.h"
#include "task.h"
#include "bt_server.h"


void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(0xDEADBEEF, line_num, p_file_name);
}


/**@brief Function for application main entry.
 */
int main(void)
{
	ble_init();
	kx022_init();	
	timers_start();
	advertising_start();
	LogD("main started\r\n");
    // Enter main loop.
    for (;;)
    {
		if (NRF_LOG_PROCESS() == false)
		{
			nrf_pwr_mgmt_run();
		}
    }
}

