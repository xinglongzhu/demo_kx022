
/************************************************************************************************************
**	FileName:	task.c
**	Description:	
--------------------------------------------------------------------------------------------------------					
**	Company:	    Radioland
--------------------------------------------------------------------------------------------------------
**	Modified:
**	Author 			Date 			 Comment
_________________________________________________________________________________________________________
**	Long     	    2019-01-12		 Original
**
**
************************************************************************************************************/

#include "task.h"
#include "bt.h"
#include "hw_debug.h"
#include "kx022.h"
/*******************************************************************************************************************
**	Hardware  Defined
********************************************************************************************************************/
#define COMMON_TASK_TIMER_MS        	 200                        					 /**< Common timer interval (ticks). */
#define COMMON_TASK_INTERVAL      		 APP_TIMER_TICKS(COMMON_TASK_TIMER_MS)

/*******************************************************************************************************************
**	public Parameters
********************************************************************************************************************/
volatile uint32_t mSysTick;


/*******************************************************************************************************************
**	static Parameters
********************************************************************************************************************/
APP_TIMER_DEF(m_common_timer_id);  


/*********************************************************************************
** Function:			
** Parameters:		
** Returns:			
** details:		
** Modified:			
**	
*********************************************************************************/

/**@brief Function for handling the common_handler.
 *
 * @details This function will be called each time the COMMON_TASK_TIMER_MS.
 *
 * @param[in]	p_context.
 */
static void common_handler(void * p_context)
{
	mSysTick++;
	acc_data_handle();
}



/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module.
 */
void common_timers_init(void)
{
	uint32_t err_code;
	LogD("common_timers_init\n\r");
	// Create common timer.
	err_code = app_timer_create(&m_common_timer_id,
								APP_TIMER_MODE_REPEATED,
								common_handler);
	APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting timers.
 */
void common_timers_start(void)
{
	uint32_t err_code;

	err_code = app_timer_start(m_common_timer_id, COMMON_TASK_INTERVAL, NULL);
	APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting timers.
 */
void common_timers_stop(void)
{
	uint32_t err_code;
	err_code = app_timer_stop(m_common_timer_id);
	APP_ERROR_CHECK(err_code);
}



/*********************************************************************************
** Function:			
** Parameters:		
** Returns:			
** details:		
** Modified:			
**	
*********************************************************************************/




/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module.
 */
void timers_init(void)
{
	uint32_t err_code;
	LogD("%s\n\r",(uint32_t)__FUNCTION__);
	err_code = app_timer_init();
	APP_ERROR_CHECK(err_code);
    common_timers_init();
	
}


/**@brief Function for starting timers.
 */
void timers_start(void)
{	
	common_timers_start();	
}


/**@brief Function for starting timers.
 */
void timers_stop(void)
{	
	common_timers_stop();
}



