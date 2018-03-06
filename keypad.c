#include "keypad.h"

/* Checks rows on keyboard */
int check_rows(int cols) {
	uint8_t temp[1];
	temp[0]=cols;
	temp[0]=temp[0]<<4;
	temp[0]=temp[0]+0x0F;
	uint8_t response[1];
	i2c_read(33, temp, response);
	return response[0]&0x0F;	

} 

/* Reads input from keyboard */
int read_buttons() {
	int i=0;
	int mask=8;
	while(i<4)
	{
		int state=check_rows(0x0F^mask);
		if(state!=0x0F)
		{
			 return state|((0x0F^mask)<<4);
		}
		mask=mask>>1;
		i++;	
	}
	return 0xFF;	
}

/* Looks up hex value to return corresponding uint8_tacter (based on index from datasheet) */
char keypad_uint8_t_decode(int button_pattern) {
	char map[]="147*2580369#ABCDG";
	
	int row=4;
	int col=4;
	switch(button_pattern&0x0F)
	{
		case 0x07:
			row=0;
			break;
		case 0x0B:
			row=1;
			break;
		case 0x0D:
			row=2;
			break;
		case 0x0E:
			row=3;
			break;	
	}
	switch(button_pattern&0xF0)
	{
		case 0x70:
			col=0;
			break;
		case 0xB0:
			col=1;
			break;
		case 0xD0:
			col=2;
			break;
		case 0xE0:
			col=3;
			break;
	}
	if(col==4 || row==4)
	{
		return 'G';
	}
	return map[col*4+row];
}
