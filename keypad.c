#include "keypad.h"
#include <stdint.h>


int keypad_get_single() {
  int state = 0xFF;
  do {
    state = read_buttons();
  } while(state == 0xFF);
  while(read_buttons() != 0xFF);
  return state;
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
