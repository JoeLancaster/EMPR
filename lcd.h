#ifndef LCD_H
#define LCD_H

#include <stdint.h>
#include <stdlib.h>

void lcd_init(void);
void lcd_write_uint8_t(uint8_t c, int pos, int line);
int lcd_uint8_t_lookup(uint8_t c);
void lcd_clear();
void lcd_write_str(uint8_t str[], int pos, int line, size_t size);


#endif
