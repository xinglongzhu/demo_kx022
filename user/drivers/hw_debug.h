/************************************************************************************************************
**	FileName:	hw_debug.h
**	Description:	
**	Major Function:	
**
--------------------------------------------------------------------------------------------------------
**	Software:	
**	Hareware:	
**						
**	Company:	hifly
--------------------------------------------------------------------------------------------------------
**	Modified:
**	Author 			Date 			 Comment
________________________________________________________________________________________________________
**	Cesar.Li		2014-10-10		Original
**
**
************************************************************************************************************/


#ifndef	_HW_DEBUG_H
#define	_HW_DEBUG_H
#include "config.h"
#include "nrf_log.h"
#include <stdio.h>

#if NRF_LOG_USES_UART
#include "user_log.h"
#endif

#if NRF_LOG_ENABLED
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"  
#endif


/*******************************************************************************************************************
**	Hardware  Defined
********************************************************************************************************************/
//colour
/*
background : font color
40: 30: black
41:  31: red
42:  32: green
43:  33: yeall
44:  34: blue
45:  35: purple
46: 36: dark green
47: 37: white
*/

#define	color_bRed			"\033[41"
#define	color_bGreen		"\033[42m"
#define	color_bBlue			"\033[44m"
#define	color_Red			"\033[1;31m"
#define	color_Green			"\033[1;32m"
#define	color_Blue			"\033[1;34m"
#define color_end			"\033[0m"

#if ((NRF_LOG_ENABLED == 1) || (NRF_LOG_USES_UART == 1))
#define DEBUGEN 1
#else
#define DEBUGEN 0
#endif


#define Logw_EN 1
#define Loge_EN 1
#define Logi_EN 1
#define Logd_EN 1


#if DEBUGEN
	#if NRF_LOG_USES_UART
		#define LOGDEBUG				USER_LOG_PRINTF 
	#else
		#define LOGDEBUG				NRF_LOG_RAW_INFO
	#endif
#else
	#define LOGDEBUG(...) 
#endif



#if Logw_EN
#define LogW	LOGDEBUG
#define cLogW(color,format, args...) LOGDEBUG(color format color_end, ##args)
#else
#define LogW(...)
#define cLogW(...) 
#endif


#if Loge_EN
#define LogE	LOGDEBUG
#define cLogE(color,format, args...) LOGDEBUG(color format color_end, ##args)
#else
#define LogE(...) 
#define cLogE(...) 
#endif



#if Logi_EN
#define LogI	LOGDEBUG
#define cLogI(color,format, args...) LOGDEBUG(color format color_end, ##args)
#else
#define LogI(...)
#define cLogI(...) 
#endif




#if Logd_EN
#define LogD	LOGDEBUG
#define cLogD(color,format, args...) LOGDEBUG(color format color_end, ##args)
#else
#define LogD(...) 
#define cLogD(...) 
#endif




/*******************************************************************************************************************
**	Functions
********************************************************************************************************************/
void core_dump(void);
void dump(void * p_buffer, uint32_t len);



#endif
//end of the file


