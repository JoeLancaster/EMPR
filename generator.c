#include "helpers.h"
#include "uart1.h"
#define RED  0
#define GREEN 1
#define BLUE 2
#define YELLOW 3
#define MAGENTA 4
#define CYAN 5
#define WHITE 6
char state = 0xFF;
char last_state = 0xFF;
int pos = 0;
int line = 0;
__attribute__((constructor))  static void init()
{
	serial_init1();
	uart1_init();
	SYSTICK_InternalInit(1);
	SYSTICK_IntCmd(ENABLE);
	SYSTICK_Cmd(ENABLE);
}
int main(void)
{
	setup();
	/*dmx_write(255,0,0);
	wait(1); */
	/*show_col(RED, 1);
	show_col(GREEN, 1);
	show_col(BLUE, 1);
	show_col(YELLOW, 1);
	show_col(MAGENTA, 1);
	show_col(CYAN, 1);*/
	int key=key_pressed();
	while(key==-1 || key >= 7)
	{
		key=key_pressed;
	}
	show_col(key, 1);
	
}

void setup()
{
	/*  Basic setup of monitor, Keyboard, LCD Monitor, Lighting Module */
	lcd_init();
	lcd_write_str("Hello User", 0,0, sizeof("hello user"));
}

void show_col(uint8_t col, uint8_t time)
{	
	/* Displays requested colour for desired duration */
	switch(col)
	{
		case 0:
			dmx_write(255,0,0);
			wait(time);
			dmx_write(0,0,0);
			wait(time);
			break;
		case 1:
			dmx_write(0,255,0);
			wait(time);
			dmx_write(0,0,0);
			wait(time);
			break;
		case 2:
			dmx_write(0,0,255);
			wait(time);
			dmx_write(0,0,0);
			wait(time);
			break;
		case 3:
			dmx_write(255,255,0);
			wait(time);
			dmx_write(0,0,0);
			wait(time);
			break;
		case 4:
			dmx_write(255,0,255);
			wait(time);
			dmx_write(0,0,0);
			wait(time);
			break;
		case 5:
			dmx_write(0,255,255);
			wait(time);
			dmx_write(0,0,0);
			wait(time);
			break;
		case 6:
			dmx_write(255,255,255);
			wait(time);
			dmx_write(0,0,0);
			wait(time);
			break;
	}
}

int key_pressed()
{
	int key=-1;
	while(keypad_char_decode(last_state)==keypad_char_decode(state) && keypad_char_decode(state)=='G')
	{
		state=last_state;
		state=read_buttons();
	}
	char r = keypad_char_decode(state);
	if(!isdigit(r))
	{
		lcd_write_str("Enter a valid digit",0,0,sizeof("enter a valid digit"));
		return key;
	}
 	key = r - '0';
	return key;
}
