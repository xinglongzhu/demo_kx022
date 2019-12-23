#ifndef my_simu_iic_h_
#define my_simu_iic_h_

#include <stdbool.h>
#include <stdint.h>
#include "nrf_gpio.h"

#define scl    30
#define sda    31


#define IIC_SCL_HIGH()   do { NRF_GPIO->OUTSET = (1UL << scl); } while (0)   /*!< Pulls SCL line high */
#define IIC_SCL_LOW()    do { NRF_GPIO->OUTCLR = (1UL << scl); } while (0)   /*!< Pulls SCL line low  */
#define IIC_SDA_HIGH()   do { NRF_GPIO->OUTSET = (1UL << sda);  } while (0)   /*!< Pulls SDA line high */
#define IIC_SDA_LOW()    do { NRF_GPIO->OUTCLR = (1UL << sda);  } while (0)   /*!< Pulls SDA line low  */
#define IIC_SDA_INPUT()  do { NRF_GPIO->DIRCLR = (1UL << sda);  } while (0)   /*!< Configures SDA pin as input  */
#define IIC_SDA_OUTPUT() do { NRF_GPIO->DIRSET = (1UL << sda);  } while (0)   /*!< Configures SDA pin as output */
#define IIC_SCL_OUTPUT() do { NRF_GPIO->DIRSET = (1UL << scl); } while (0)   /*!< Configures SCL pin as output */
/*lint -restore */

#define IIC_SDA_READ() ((NRF_GPIO->IN >> sda) & 0x1UL)                     /*!< Reads current state of SDA */
#define IIC_SCL_READ() ((NRF_GPIO->IN >> scl) & 0x1UL)                    /*!< Reads current state of SCL */

#define IIC_DELAY(TIME_US) nrf_delay_us(TIME_US)



bool my_simu_iic_init(void);
void my_simu_iic_start(void);
void my_simu_iic_stop(void);
void my_simu_iic_sent_ack(void);
void my_simu_iic_sent_nack(void);
uint8_t my_simu_iic_wait_ack(void);
void my_simu_iic_send_byte(uint8_t value);
uint8_t  my_simu_iic_read_byte(bool ack);
#endif

