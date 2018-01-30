#ifndef HELPERS_H
#define HELPERS_H

#include <stdlib.h>
#include <stdint.h>
#include "lpc17xx_i2c.h"

void turn_on(uint8_t lights);
void turn_on_single(int light);
void wait(double seconds);
void Systick_Handler(void);
void Delay(unsigned long tick);
int read_usb_serial_none_blocking(char *buf,int length);
int write_usb_serial_blocking(char *buf,int length);
void serial_init(void);
Status i2c_write(uint8_t addr, char data[]);
void lcd_init(void);
void lcd_write_char(char c, int pos, int line);
int lcd_char_lookup(char c);
void lcd_clear();
void lcd_write_str(char str[], int pos, int line, size_t size);
void i2c_read(uint8_t addr, char data[], char response[]);
void by2bi_convert_single(char byte, char binary[]);
void by2bi_convert(char byte[], char binary[], size_t bytes);
int check_rows(int cols);
int read_buttons();
char keypad_char_decode(int button_pattern);
void myDAC_init();
int myADC_init(int pinOut);  


#endif
