#ifndef KX022_H
#define KX022_H

#include <stdint.h>
#include <stdbool.h>

#define XHPL            0x00        //R
#define XHPH            0X01        //R
#define YHPL            0X02        //R
#define YHPH            0X03        //R
#define ZHPL            0X04        //R
#define ZHPH            0X05        //R
#define XOUTL           0X06        //R
#define XOUTH           0X07        //R
#define YOUTL           0X08        //R
#define YOUTH           0X09        //R
#define ZOUTL           0X0A        //R
#define ZOUTH           0X0B        //R
#define COTR            0X0C        //R
#define RESERVE1        0X0D
#define RESERVE2        0X0E
#define Who_AM_I        0X0F        //R/W
#define TSCP            0X10        //R
#define TSPP            0X11        //R
#define INS1            0X12        //R
#define INS2            0X13        //R
#define INS3            0X14        //R
#define STAT            0X15        //R
#define RESERVE3        0X16
#define INT_REL         0x17        //R
#define CNTL1           0X18        //R/W
#define CNTL2           0X19        //R/W
#define CNTL3           0X1A        //R/W
#define ODCNTL          0X1B        //R/W
#define INC1            0X1C        //R/W
#define INC2            0X1D        //R/W
#define INC3            0X1E        //R/W
#define INC4            0X1F        //R/W
#define INC5            0X20        //R/W
#define INC6            0X21        //R/W
#define TILT_TIMER      0X22        //R/W
#define WUFC            0X23        //R/W
#define TDTRC           0X24        //R/W
#define TDTC            0X25        //R/W
#define TTH             0X26        //R/W
#define TTL             0X27        //R/W
#define FTD             0X28        //R/W
#define STD             0X29        //R/W
#define TLT             0X2A        //R/W
#define TWS             0X2B        //R/W
#define RESERVE4        0X2C         
#define RESERVE5        0X2D
#define RESERVE6        0X2E
#define RESERVE7        0X2F
#define ATH             0X30         //R/W
#define RESERVE8        0X31
#define TILT_ANGLE_LL   0X32         //R/W
#define TILT_ANGLE_HL   0X33         //R/W
#define HYST_SET        0X34         //R/W
#define LP_CNTL         0X35         //R/W
#define RESERVE9        0X36
#define RESERVE10       0X37
#define RESERVE11       0X38
#define RESERVE12       0X39
#define BUF_CNTL1       0X3A         //R/W
#define BUF_CNTL2       0X3B         //R/W
#define BUF_STATUS_1    0X3C         //R
#define BUF_STATUS_2    0X3D         //R
#define BUF_CLEAR       0X3E         //W
#define BUF_READ        0X3F         //R
#define SELF_TEST       0X60         //W

#define KX022_ADDRESS_WRITE    0X3C
#define KX022_ADDRESS_READ     0x3D

#define KX022_ADDRESS          0x1E


static bool kx022_register_write(uint8_t reg_addr, uint8_t value);

static bool kx022_register_read(uint8_t reg_address, uint8_t * destination, uint8_t rdatalen);

bool kx022_init(void);

void acc_read (uint8_t *acc_buff);
void kx022_timeout_handler(void);
void kx022_interrupt_Init(void);
void kx022_uinit(void);
void acc_data_handle(void);
#endif //KX022_H
