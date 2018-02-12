#include "helpers.h"
#include "uart1.h"
#define RED  0
#define GREEN 1
#define BLUE 2
#define YELLOW 3
#define MAGENTA 4
#define CYAN 5
#define WHITE 6
#define SIZE 3
char state = 0xFF;
char last_state = 0xFF;
int pos = 0;
int line = 0;
int A[3];
int B[3];
int C[3];
int D[3];
int time;
__attribute__((constructor))  static void init()
{
	serial_init1();
	uart1_init();
	//timer_init();
	SYSTICK_InternalInit(1);
	SYSTICK_IntCmd(ENABLE);
	SYSTICK_Cmd(ENABLE);
}
int main(void)
{
	setup();
	int count=0;
	int i;
	/*for(i=0;i<200;i++)
	{
		dmx_write(0,255,0);
	}*/
	while(1)
	{
		dmx_write(0,0,0);
		wait(0.5);
		dmx_write(255,255,255);
		wait(0.5);
		dmx_write(255,0,0);
		wait(0.5);
		dmx_write(0,255,0);
		wait(0.5);
		dmx_write(0,0,255);
		wait(0.5);
		dmx_write(0,255,255);
		wait(0.5);
	}
	//G1();
	//G2_single();
	/*G2(A, SIZE);
	wait(1);
	lcd_init();
	lcd_write_str("Next Packet ", 0,0, sizeof("Next Packet "));
	wait(1);
	G2(B, SIZE);
	//dmx_clear();
	// Implement longer ability to display larger sequences
	lcd_init();
	lcd_write_str("Please enter time for  delay: ",0,0,sizeof("Please enter time for  delay: "));
	wait(1);
	lcd_init();
	while(key_pressed()!=-1 && key_pressed() <= 6)
	{
		time=key_pressed();
		show_seq(A, B, time);	
	}*/
	


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

void G2_single()
{
	int red[3], green[3], blue[3],red_intensity,green_intensity,blue_intensity,i,j,pos;
	red_intensity=256;
	green_intensity=256;
	blue_intensity=256;
	lcd_init();
	lcd_write_str("Enter intensity for Red: ",0,0,sizeof("enter intensity for reddd"));
	wait(1);
	/* Only allows Intensity values less than or equal to 255 */ //Test this hypothesis
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
	lcd_init();
	/* Displays packet defined by user */
	dmx_write(red_intensity,green_intensity,blue_intensity);

}
void G2(int packet[], size_t size)
{
	int red[3], green[3], blue[3],red_intensity,green_intensity,blue_intensity,i,j,pos;
	red_intensity=256;
	green_intensity=256;
	blue_intensity=256;
	lcd_init();
	lcd_write_str("Enter intensity for Red: ",0,0,sizeof("enter intensity for reddd"));
	wait(1);
	/* Only allows Intensity values less than or equal to 255 */ //Test this hypothesis
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
	lcd_init();
	/* Stores packet for G3() */
	packet[0] = red_intensity;
	packet[1] = green_intensity;
	packet[2] = blue_intensity;
	return packet;
}
/*
void G3()
{*/
	/* Calls on packets defined in G2(), creates a sequence of these packets which will be defined by the user, and displayed on Lighting Module when corresponding key pressed */
/*	while(1) // May need to use switch case
	{
		state=read_buttons();
		if(state== ) //val for A #ToDo
		{
			// Displays sequence A defined by user
			// Create display[] array which will have sequence of packets to be displayed
		}
		if(state == ) // val for B #ToDo
		{

		}
		if(state == ) // Val for C #ToDo
		{

		}
		if(state == ) // Val for D #ToDo
		{

		}
	}
}
*/
int read_intensity(int intensity[], size_t size, int pos)
{
	/* Reads intensity value entered on Keypad, displays value on LCD Display and returns value as an integer */
	int intensity_val;
	pos=0;
	lcd_init();
	state=0xFF;
	while(1)
	{
		state=read_buttons();
		if(state==0xDE) // if '#' entered no longer reads from Keypad; breaks from while loop
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
		last_state=state;
		wait(0.01);
		/* Catches input that is not Valid */ //Test if function works with this error handling code
		/*if(key_pressed()==-1)
		{
			lcd_init();
			lcd_write_str("Please Enter a  Valid digit!",0,0,sizeof("please enter a  valid digita"));
			wait(2);
			lcd_init();
			lcd_write_str("Please Restart  to try again", 0,0, sizeof("please restart  to try again"));	
		}*/
	}
	/* Decodes input from Keypad and stores result in char array */
	char inten[3];
	inten[0]= (keypad_char_decode(intensity[0]));
	inten[1]= (keypad_char_decode(intensity[1]));
	inten[2]= (keypad_char_decode(intensity[2]));

	/* Converts String(char array) into and int */
	intensity_val=atoi(inten);

	return intensity_val;

}
int concat(int* arr, size_t len)
{
	/* Concatenates integers in an array arr of size  len */
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
	/*  Basic setup of monitor, Keypad, LCD Display, Lighting Module */
	lcd_init();
	lcd_write_str("Hello User", 0,0, sizeof("hello user"));
	//dmx_clear();
}

void dmx_clear()
{
	/* Clears lights on Lighting module */
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

void show_seq(int packet1[], int packet2[], int time)
{
	int r_pack1, g_pack1, b_pcak1, r_pack2, g_pack2, b_pcak2;
	/* Intensity values for 1st Packet */
	r_pack1 = packet1[0];
	g_pack1 = packet1[1];
	b_pcak1 = packet1[2];

	/* Intensity values for 2nd Packet */
	r_pack2 = packet2[0];
	g_pack2 = packet2[1];
	b_pcak2 = packet2[2];
	
	dmx_write(r_pack1, g_pack1, b_pcak1);
	wait(time);
	dmx_write(r_pack2, g_pack2, b_pcak2);
	wait(time);
	dmx_clear();
} 

int key_pressed()
{
	/* Returns key pressed on Keypad */
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
