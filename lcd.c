#include "lcd.h"
#include <stdint.h>
#include <stdlib.h>

/* Initialises LCD */
void lcd_init(void) {
	uint8_t data[2];
	data[0] = 0x00;
	data[1] = 0x00;
	i2c_write(59, data);
	data[1] = 0x34; 
	i2c_write(59, data);
	data[1] = 0x0C; 
	i2c_write(59, data);
	data[1] = 0x06; 
	i2c_write(59, data);
	data[1] = 0x35; 
	i2c_write(59, data);
	data[1] = 0x04; 
	i2c_write(59, data);
	data[1] = 0x10; 
	i2c_write(59, data);
	data[1] = 0x42; 
	i2c_write(59, data);
	data[1] = 0x9F; 
	i2c_write(59, data);
	data[1] = 0x34; 
	i2c_write(59, data);
	data[1] = 0x02; 
	i2c_write(59, data);
	lcd_clear();
}

/* Clears LCD Screen */
void lcd_clear() {
	int pos=0;
	while(pos<40)
	{
		lcd_write_uint8_t(' ', pos, 0);
		lcd_write_uint8_t(' ', pos, 1);
		pos++;
	}


		 	
}
 /* Writes a single uint8_tacter on the LCD Screen */
void lcd_write_uint8_t(uint8_t c, int pos, int line) {
	uint8_t data[2];
	data[0] = 0x00;
	data[1] = 0x80 + pos+line*40;
	i2c_write(59, data);
	data[0] = 0xC0;
	data[1] = lcd_uint8_t_lookup(c);
	i2c_write(59, data);
	/*data[0] = 0xC0;
	data[1] = lcd_uint8_t_lookup(c);
	i2c_write(59, data);*/
}
 /* Writes a string on the LCD Screen */
void lcd_write_str(uint8_t str[], int pos, int line, size_t size) {
	int i=0;
	int j=0;
	//int strlen=sizeof(str)/sizeof(str[0]);
	//int strlen = sizeof(str);
	while(i<size-1) //Subtracted by 1 to get rid of extra unnecessary uint8_tacters 
	{	
		if(line==0)
		{
			if(i<16)
			{
				lcd_write_uint8_t(str[i], pos+i, 0);
			}
			else
			{
				lcd_write_uint8_t(str[i], 24+j, 1); 
				j++;		
			}
		}
		else
		{
			lcd_write_uint8_t(str[i], pos+24+i, 1); 
			
		}
		i++;	
	}
}

/* Looks up given uint8_tacter(based on index from datasheet */
int lcd_uint8_t_lookup(uint8_t c) {
	if(' '<=c && c>='!'){return c+128;}
	if('A'<=c && c>='Z'){return c+128;}
	if('a'<=c && c>='z'){return c+80;}
	if('0'<=c && c>='9'){return c+128;}
	if('#'<=c && c>='*'){return c+128;}

	if(c == 32){return 0xA0;}
	
	return 0x20;
}	
