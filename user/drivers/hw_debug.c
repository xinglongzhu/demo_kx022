/************************************************************************************************************
**	FileName:	hw_debug.c
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
_________________________________________________________________________________________________________
**	Cesar.Li			2014-10-10	Original
**
**
************************************************************************************************************/

#include "hw_debug.h"

/**
 * @brief      Write character to stream
 *
 * @param[in]  ch       Character to be written. The character is passed as its int promotion.
 * @param[in]  stream   Pointer to a FILE object that identifies the stream where the character is to be written.
 *
 * @returns    If there are no errors, the same character that has been written is returned.
 *             If an error occurs, EOF is returned and the error indicator is set (see ferror).
 *
 * @details    Writes a character to the stream and advances the position indicator.\n
 *             The character is written at the current position of the stream as indicated \n
 *             by the internal position indicator, which is then advanced one character.
 *
 * @note       The above descriptions are copied from http://www.cplusplus.com/reference/clibrary/cstdio/fputc/.
 *
 *
 */

void core_dump(void)
{
	#define STATC_CORE_DUMP_OFF  6
	uint8_t i,num;

	__IO uint32_t*  sp;
	__IO uint32_t xpsr = (uint32_t)__get_xPSR();
	__IO uint32_t* msp = (uint32_t*)__get_MSP();//   (uint32_t*)__get_MSP;
	__IO uint32_t* psp = (uint32_t*)__get_PSP();
	
	LogE("**	PSP=0x%x, MSP=0x%x, xPSR=0x%x\n\r",(uint32_t)psp,(uint32_t)msp,(uint32_t)xpsr);
	
	if (xpsr & 0x4){
		sp = psp;
	}else{
		sp = msp;
	}
	sp += STATC_CORE_DUMP_OFF;
	
	
  /* Go to infinite loop when Hard Fault exception occurs */
	LogE("**********************************************************************\n\r");
	LogE("**	core dump\n\r");
	LogE("**	PSP=0x%x, MSP=0x%x\n\r",(uint32_t)psp,(uint32_t)msp);
	LogE("**	sp=0x%x\n\r",(uint32_t)*sp);
	for (i=0,num=0; i< 16; i++){
		if (0==(i%4)){
			LogE("\n\r**   ");
		}
		LogE("0x%x : 0x%.8x		",( (uint32_t)(sp + i) ),*(sp + i) );
		if ( (*(sp + i) == 0xfffffff9) && (num == 0) )
		{	
			num = i;
			num++;
		}
	}
	
	LogE("**\n\r");
	LogE("**	SP     0x%x : 0x%.8x\n\r",( (uint32_t)(sp + num) ),*(sp + num) );num++;
	LogE("**	R1     0x%x : 0x%.8x\n\r",( (uint32_t)(sp + num) ),*(sp + num));num++;
	LogE("**	R2     0x%x : 0x%.8x\n\r",( (uint32_t)(sp + num) ),*(sp + num));num++;
	LogE("**	R3     0x%x : 0x%.8x\n\r",( (uint32_t)(sp + num) ),*(sp + num));num++;
	LogE("**	R12    0x%x : 0x%.8x\n\r",( (uint32_t)(sp + num) ),*(sp + num));num++;
	LogE("**	LR     0x%x : 0x%.8x\n\r",( (uint32_t)(sp + num) ),*(sp + num));num++;
	LogE("**	PC     0x%x : 0x%.8x\n\r",( (uint32_t)(sp + num) ),*(sp + num));num++;
	LogE("**	xPSR   0x%x : 0x%.8x\n\r",( (uint32_t)(sp + num) ),*(sp + num));num++;
	LogE("**	old SP 0x%x : 0x%.8x\n\r",( (uint32_t)(sp + num) ),*(sp + num));num++;
	LogE("**\n\r");
	LogE("**********************************************************************\n\r");
}

void dump(void * p_buffer, uint32_t len)
{
#if  NRF_LOG_ENABLED	
	uint8_t *p = (uint8_t*)p_buffer;
    for (uint32_t index = 0; index <  len; index++)
    {
        LogD("0x%02X ", p[index]);
    }
    LogD("\r\n");
#endif	
}


