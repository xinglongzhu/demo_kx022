#include <stdint.h>
#include <string.h>
#include "KX022.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "hw_debug.h"
#include "my_simu_iic.h"
#include "nrf_delay.h"
#include "task.h"

static bool kx022_register_write(uint8_t reg_addr, uint8_t value)
{
	bool transfer_succeeded;
	my_simu_iic_start();
	my_simu_iic_send_byte(KX022_ADDRESS_WRITE);
	transfer_succeeded = my_simu_iic_wait_ack();
	my_simu_iic_send_byte(reg_addr);
	transfer_succeeded &= my_simu_iic_wait_ack();
	my_simu_iic_send_byte(value);
	transfer_succeeded &= my_simu_iic_wait_ack();
	my_simu_iic_stop();	
	return transfer_succeeded;
}

static bool kx022_register_read(uint8_t reg_address, uint8_t * destination, uint8_t rdatalen)
{
    bool transfer_succeeded;
	my_simu_iic_start();
	my_simu_iic_send_byte(KX022_ADDRESS_WRITE);
	transfer_succeeded = my_simu_iic_wait_ack();
	my_simu_iic_send_byte(reg_address);
	transfer_succeeded &= my_simu_iic_wait_ack();
	//read start
	my_simu_iic_start();
	my_simu_iic_send_byte(KX022_ADDRESS_READ);
	transfer_succeeded &= my_simu_iic_wait_ack();
	for(uint8_t i=0;i<rdatalen;i++)
	{		
		destination[i] = my_simu_iic_read_byte(i==(rdatalen-1)?0:1);
	} 
	my_simu_iic_stop();
	
	return transfer_succeeded;
}

void kx022_uinit(void)
{
	 kx022_register_write(CNTL1,0x00);
}


bool kx022_init(void)
{
	my_simu_iic_init();
	nrf_delay_ms(10);
	bool init_success = false;	
	for(int i = 0; i < 5; i ++)
	{
		if(kx022_register_write(CNTL2,0XBF) == true)
		{
			init_success = true;
			break;
		}		    
		else
		{
			continue;
		}
	}
	nrf_delay_ms(1);
	LogD("kx022_init=%d\r\n",init_success);
	if(init_success == true)
	{
		kx022_register_write(CNTL1,0X00);
		kx022_register_write(CNTL1,0X40);
		kx022_register_write(ODCNTL,0X02);
		kx022_register_write(CNTL1,0X80);
		kx022_uinit();
	}
	return init_success;
	
}



void acc_data_process(uint8_t *acc_buff,int16_t*buf)
{                                                                                 
	char buff[10] = {0};
	float aaa = 0;
	for(uint8_t i = 0; i < 3; i++)
	{
		
		aaa = buf[i] * 0.00006;
		sprintf(buff,"%f",aaa);
		if(buff[0] == '-')
		{
			acc_buff[i*4] = 1;	
			acc_buff[i*4+1] = buff[1] - 0x30;
			acc_buff[i*4+2] = buff[3] - 0x30;
			acc_buff[i*4+3] = buff[4] - 0x30;
			if((acc_buff[i*4+1] == 0) && (acc_buff[i*4+2] == 0) &&(acc_buff[i*4+3] == 0))
			{
				acc_buff[i*4] = 0;
			}
		}
		else
		{
			acc_buff[i*4] = 0;	
			acc_buff[i*4+1] = buff[0] - 0x30;
			acc_buff[i*4+2] = buff[2] - 0x30;
			acc_buff[i*4+3] = buff[3] - 0x30;		
		}		
	}
	LogD("\r\n");
	LogD("x=%d %d.%d%d\r\n",acc_buff[0],acc_buff[1],acc_buff[2],acc_buff[3]);
	LogD("y=%d %d.%d%d\r\n",acc_buff[4],acc_buff[5],acc_buff[6],acc_buff[7]);
	LogD("z=%d %d.%d%d\r\n",acc_buff[8],acc_buff[9],acc_buff[10],acc_buff[11]);
	LogD("\r\n");
}

void acc_read (uint8_t *acc_buff)
{
	int16_t sss[3];
	uint8_t buf[2];
	uint16_t temp = 0;
	kx022_register_write(CNTL1,0X80);
	if(kx022_register_read(XOUTH,&buf[0],1))
	{
		kx022_register_read(XOUTL,&buf[1],1);
		temp = buf[0];
		temp = (temp << 8) | buf[1];
		sss[0] = temp;
	}

	if(kx022_register_read(YOUTH,&buf[0],1))
	{
		kx022_register_read(YOUTL,&buf[1],1);
		temp = buf[0];
		temp = (temp << 8) | buf[1];
		sss[1] = temp;

	}
	if(kx022_register_read(ZOUTH,&buf[0],1))
	{
		kx022_register_read(ZOUTL,&buf[1],1);
		temp = buf[0];
		temp = (temp << 8) | buf[1];
		sss[2] = temp;
	}
	acc_data_process(acc_buff,sss);
	kx022_uinit();
}

void acc_data_handle(void)
{
	uint8_t acc_buff[12];
    static uint32_t get_acc_data_timer = 0;
	if((mSysTick - get_acc_data_timer) > 5)
	{
		get_acc_data_timer = mSysTick;	
		acc_read(acc_buff);
	}
}



