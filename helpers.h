#ifndef HELPERS_H
#define HELPERS_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "lpc17xx_i2c.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_systick.h"
#include "lpc17xx_libcfg_default.h"
#include "lpc17xx_systick.h"
#include "lpc17xx_uart.h"		
#include "lpc17xx_pinsel.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_dac.h"
#include "lpc_types.h"
#include <stdlib.h>


void wait(double seconds);
int write_usb_serial_blocking(uint8_t *buf,int length);
void serial_init(void);
void serial_init1(void);
Status i2c_write(uint8_t addr, uint8_t data[]);
void i2c_read(uint8_t addr, uint8_t data[], uint8_t response[]);
void myDAC_init();
int myADC_init(int pinOut);

int arcmp(uint8_t *, uint8_t *, size_t);


#endif
