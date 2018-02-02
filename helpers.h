#ifndef HELPERS_H
#define HELPERS_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "lpc17xx_gpio.h"
#include "lpc17xx_systick.h"
#include "lpc17xx_libcfg_default.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_pinsel.h"
#include "lpc_types.h"
//#include "serial.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_adc.h"


void turn_on(uint8_t lights);
void turn_on_single(int light);
void wait(double seconds);
void Systick_Handler(void);
void Delay(unsigned long tick);
int read_usb_serial_none_blocking(uint8_t *buf,int length);
int read_uart1(uint8_t *buf, int length);
int write_usb_serial_blocking(uint8_t *buf,int length);
int write_uart1(uint8_t *buf,int length);
void serial_init(void);
Status i2c_write(uint8_t addr, uint8_t data[]);
void lcd_init(void);
void lcd_write_uint8_t(uint8_t c, int pos, int line);
int lcd_uint8_t_lookup(uint8_t c);
void lcd_clear();
void lcd_write_str(uint8_t str[], int pos, int line, size_t size);
void i2c_read(uint8_t addr, uint8_t data[], uint8_t response[]);
void by2bi_convert_single(char byte, uint8_t binary[]);
void by2bi_convert(char byte[], uint8_t binary[], size_t bytes);
int check_rows(int cols);
int read_buttons();
char keypad_char_decode(int button_pattern);
void myDAC_init();
int myADC_init(int pinOut);  



#endif
