#include "my_simu_iic.h"
#include "nrf_delay.h"
#include "hw_debug.h"

static bool twi_master_clear_bus(void)
{
    bool bus_clear;
    IIC_SDA_HIGH();
    IIC_SCL_HIGH();
    IIC_DELAY(4);
    if (IIC_SDA_READ() == 1 && IIC_SCL_READ() == 1)
    {
        bus_clear = true;
    }
    else if (IIC_SCL_READ() == 1)
    {
        bus_clear = false;
        for (uint_fast8_t i = 18; i--;)
        {
            IIC_SCL_LOW();
            IIC_DELAY(4);
            IIC_SCL_HIGH();
            IIC_DELAY(4);

            if (IIC_SDA_READ() == 1)
            {
                bus_clear = true;
                break;
            }
        }
    }
    else
    {
        bus_clear = false;
    }
    return bus_clear;
}

bool my_simu_iic_init(void)
{
  IIC_SCL_OUTPUT();
  IIC_SCL_HIGH();
  IIC_SDA_OUTPUT();	
  IIC_SDA_HIGH();
  return twi_master_clear_bus();
}

void my_simu_iic_start(void)
{
	IIC_SDA_OUTPUT();  
	IIC_SDA_HIGH();
	
	IIC_SCL_HIGH();
	IIC_DELAY(30);//

	IIC_SDA_LOW(); 
	IIC_DELAY(4);
	IIC_SCL_LOW(); 
//	IIC_DELAY(4);
}	
void my_simu_iic_stop(void)
{
	IIC_SDA_OUTPUT();  
	IIC_SCL_HIGH();
	IIC_DELAY(30);//
	IIC_SDA_LOW(); 
	IIC_DELAY(4);
	IIC_SDA_HIGH(); 
}	

void my_simu_iic_sent_ack(void)
{
	IIC_SCL_LOW();
	IIC_SDA_OUTPUT();
	IIC_DELAY(4);
	
	IIC_SDA_LOW();
	IIC_DELAY(4);
	
	IIC_SCL_HIGH();
	IIC_DELAY(4);
	IIC_SCL_LOW();
	IIC_DELAY(4);
}

void my_simu_iic_sent_nack(void)
{
	IIC_SCL_LOW();
	IIC_SDA_OUTPUT();
	IIC_DELAY(4);
	
	IIC_SDA_HIGH();
	IIC_DELAY(4);
	
	IIC_SCL_HIGH();
	IIC_DELAY(4);
	
	IIC_SCL_LOW();
	IIC_DELAY(4);
}

uint8_t my_simu_iic_wait_ack(void)
{
	//第9个时钟周期
	uint8_t ucErrTime=0;
	IIC_SDA_HIGH();
	IIC_SDA_INPUT();
	IIC_SCL_HIGH();
	IIC_DELAY(4);
	while(IIC_SDA_READ())
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			my_simu_iic_stop();
			return 0;
		} 
		IIC_DELAY(4);		
	}
	IIC_SCL_LOW();
	IIC_DELAY(4);
	return 1;
}


void my_simu_iic_send_byte(uint8_t value)
{
	//8个时钟周期
	IIC_SDA_OUTPUT();
	IIC_SCL_LOW();
	IIC_DELAY(20);		
    for (uint8_t i = 0; i < 8; i++)
    {
		if((value&0x80)>>7)
		{
			IIC_SDA_HIGH();
		}
		else
		{
			IIC_SDA_LOW();
		}
		value<<=1;
		IIC_SCL_HIGH();
		IIC_DELAY(4); 
		IIC_SCL_LOW();
		IIC_DELAY(4);			           
    }	 
}

uint8_t  my_simu_iic_read_byte(bool ack)
{
	uint8_t value = 0;
	nrf_gpio_cfg_input(sda,NRF_GPIO_PIN_PULLUP);
	IIC_SCL_OUTPUT(); 
	IIC_DELAY(40); 
	for (uint8_t i = 0; i < 8; i++)
	{
		IIC_SCL_LOW();
		IIC_DELAY(4); 
		IIC_SCL_HIGH();	
		value = value << 1;
		if(nrf_gpio_pin_read(sda) == 1)
		{
			value = value + 1;
		}
		IIC_DELAY(4);
		
	}
	if(ack)
	{
		my_simu_iic_sent_ack();
	}
	else
	{
		my_simu_iic_sent_nack();
	}
	return value;
		           
}	 	


