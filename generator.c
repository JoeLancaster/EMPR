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
	int count=0;
	while(count<500)
	{
		dmx_write(0,255,0);
		count++;
	}
	//G1();
	G2();
	
		
}

void G1()
{
 /*	Displays fixed pattern when user enters a corresponding number on the keypad   */
	int key;
	dmx_clear(); // Clears the Lighting module
	while(key_pressed()!=-1 && key_pressed() <= 6)
	{
		key=key_pressed();
		show_col(key, 0);
	}
	/* Catches digits that are not Valid */
	dmx_write(255,0,0); // Displays RED indicating an error
	lcd_init();
	lcd_write_str("Please Enter a  Valid digit!",0,0,sizeof("please enter a  valid digita"));
	wait(2);
	lcd_init();
	lcd_write_str("Please Restart  to try again", 0,0, sizeof("please restart  to try again"));
}

void G2()
{
	int red[3], green[3], blue[3],red_intensity,green_intensity,blue_intensity,i,j,pos;
	red_intensity=256;
	green_intensity=256;
	blue_intensity=256;
	lcd_init();
	lcd_write_str("Enter intensity for Red: ",0,0,sizeof("enter intensity for reddd"));
	wait(1);
	while(!(red_intensity<=255))
	{
		red_intensity = read_intensity(red,3,pos);
	} 
	lcd_init();
	lcd_write_str("Enter intensity for Green: ",0,0,sizeof("enter intensity for Greenn"));
	wait(1);
	while(!(green_intensity<=255))
	{
		green_intensity = read_intensity(green,3,pos);
	} 
	lcd_init();
	lcd_write_str("Enter intensity for Blue: ",0,0,sizeof("Enter intensity for blueee"));
	wait(1);
	while(!(blue_intensity<=255))
	{
		blue_intensity = read_intensity(blue,3,pos);
	} 
	dmx_write(red_intensity,green_intensity,blue_intensity);
	
}
int read_intensity(int intensity[], size_t size, int pos)
{
	int intensity_val;
	pos=0;
	lcd_init();
	state=0xFF;
	while(1)
	{
		state=read_buttons();
		if(state==0xDE) // if '#' enter
		{
			break;
		}
		if(keypad_char_decode(last_state)!=keypad_char_decode(state) && 
		keypad_char_decode(state)!='G')
		{
			intensity[pos]=state;
			lcd_write_uint8_t(keypad_char_decode(state),pos,line);
			pos++;
		}
		//i++;
		last_state=state;
		wait(0.01);
		/* Catches input that is not Valid */
		/*if(key_pressed()==-1)
		{
			lcd_init();
			lcd_write_str("Please Enter a  Valid digit!",0,0,sizeof("please enter a  valid digita"));
			wait(2);
			lcd_init();
			lcd_write_str("Please Restart  to try again", 0,0, sizeof("please restart  to try again"));	
		}*/
	}
	char inten[3];
	inten[0]= (keypad_char_decode(intensity[0]));
	inten[1]= (keypad_char_decode(intensity[1]));
	inten[2]= (keypad_char_decode(intensity[2]));

	intensity_val=atoi(inten);

	return intensity_val;

}
int concat(int* arr, size_t len)
{
    int result = 0;
    int i;
    for (i = 0; i < len; i++)
    {
        int digits = floor(log10(arr[i])) + 1;
        result *= pow(10, digits);
        result += arr[i];
    }

    return result;
}

void setup()
{
	/*  Basic setup of monitor, Keyboard, LCD Monitor, Lighting Module */
	lcd_init();
	lcd_write_str("Hello User", 0,0, sizeof("hello user"));
}

void dmx_clear()
{
	dmx_write(0,0,0);
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

/*char* itoa(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}*/

int key_pressed()
{
	int key=-1;
	state=0xFF;
	while(keypad_char_decode(last_state)==keypad_char_decode(state) && keypad_char_decode(state)=='G')
	{
		state=last_state;
		state=read_buttons();
	}
	char r = keypad_char_decode(state);
	if(!isdigit(r))
	{
		return key;
	}
 	key = r - '0';
	return key;
}
