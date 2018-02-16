#include "helpers.h"
#include "uart1.h"
#include "lpc17xx_timer.h"
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
int i,j;
int red[3], green[3], blue[3],red_intensity,green_intensity,blue_intensity,intensity_val;
char inten[3];


extern break_flag;

__attribute__((constructor))  static void init()
{
	serial_init1();
	uart1_init();
	timer_init();
}

void TIMER0_IRQHandler(void){
  if(TIM_GetIntStatus(LPC_TIM0, TIM_MR0_INT) == SET){
    LPC_UART1 -> LCR &= ~(UART_LCR_BREAK_EN);
  }
  TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
  TIM_Cmd(LPC_TIM0, DISABLE);
}
void TIMER1_IRQHandler(void){
  if(TIM_GetIntStatus(LPC_TIM1, TIM_MR1_INT) == SET){
    break_flag = 1;
  }
  TIM_ClearIntPending(LPC_TIM1, TIM_MR1_INT);
  TIM_Cmd(LPC_TIM1, DISABLE);
}

int main(void)
{
	setup();
	int count=1;
	char * str[4];
	/*while(1)
	{
		dmx_write(255,255,255);
		wait(0.1);*/
		/*dmx_write(0,0,0);
		wait(0.5);
		/*dmx_write(0,0,0);
		wait(0.5);
		dmx_write(0,0,255);
		wait(0.5);
		dmx_write(0,0,0);
		wait(0.5);*/
	//}
	dmx_clear();
	//G2(A,SIZE);
	dmx_clear();
	//G2_single();
	for(i=0;i<1;i++)
	{
		A[i]=255;
	}
	//wait(1);
	/*lcd_init();
	lcd_write_str("Next Packet ", 0,0, sizeof("Next Packet "));
	wait(1);*/
	//G2(B,SIZE);
	/*for(i=0;i<2;i++)
	{*/
		B[0]=0;
		B[1]=255;
		B[2]=0;

		C[0]=0;
		C[1]=0;
		C[2]=255;
		
		D[0]=255;
		D[1]=255;
		D[2]=255;
	//}
	G3();
	/*sprintf(str,"%d ",C[0]);
	write_usb_serial_blocking(str,4);
	sprintf(str,"%d ",C[1]);
	write_usb_serial_blocking(str,4);
	sprintf(str,"%d ",C[2]);
	write_usb_serial_blocking(str,4);*/
	//lcd_write_str("TEST ", 0,0, sizeof("TEST "));
	
	//dmx_write(C[0],C[1],C[2]);
	/*lcd_init();
	lcd_write_str("G3", 0,0, sizeof("g3"));
	wait(1);
	G3();*/
	//G1();
	//G2_single();
	//G2(A, SIZE);
	
	//G2(B, SIZE);
	//dmx_clear();
	/*lcd_init();
	lcd_write_str("Please enter time for  delay: ",0,0,sizeof("Please enter time for  delay: "));
	wait(1);
	lcd_init();
	while(key_pressed()!=-1 && key_pressed() <= 6)
	{
		time=key_pressed();
		show_seq(A, B, time);	
	}*/
	//show_seq(A,B,1);


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
	red_intensity=256;
	green_intensity=256;
	blue_intensity=256;
	lcd_init();
	lcd_write_str("Enter intensity for Red: ",0,0,sizeof("enter intensity for reddd"));
	wait(1);
	/* Only allows Intensity values less than or equal to 255 */ //Test this hypothesis
	while(!(red_intensity<=255))
	{
		for(i=0;i<3;i++)
		{
			red[i]=0;
		}
		red_intensity = read_intensity(red,3,0);
	}
	lcd_init();
	lcd_write_str("Enter intensity for Green: ",0,0,sizeof("enter intensity for Greenn"));
	wait(1);
	while(!(green_intensity<=255))
	{
		for(i=0;i<3;i++)
		{
		
			green[i]=0;
		}
		green_intensity = read_intensity(green,3,0);
	}
	lcd_init();
	lcd_write_str("Enter intensity for Blue: ",0,0,sizeof("Enter intensity for blueee"));
	wait(1);
	while(!(blue_intensity<=255))
	{
		for(i=0;i<3;i++)
		{
			blue[i]=0;
		}
		blue_intensity = read_intensity(blue,3,0);
	}
	lcd_init();
	/* Displays packet defined by user */
	dmx_write(red_intensity,green_intensity,blue_intensity);

}
void G2(int packet[], size_t size)
{	
	i=0;
	red_intensity=256;
	green_intensity=256;
	blue_intensity=256;
	lcd_init();
	lcd_write_str("Enter intensity for Red: ",0,0,sizeof("enter intensity for reddd"));
	wait(1);
	/* Only allows Intensity values less than or equal to 255 */ //Test this hypothesis
	while(!(red_intensity<=255))
	{
		for(i=0;i<3;i++)
		{
			red[i]=0;
		}
		red_intensity = read_intensity(red,3,0);
	}
	lcd_init();
	lcd_write_str("Enter intensity for Green: ",0,0,sizeof("enter intensity for Greenn"));
	wait(1);
	while(!(green_intensity<=255))
	{
		for(i=0;i<3;i++)
		{
		
			green[i]=0;
		}
		green_intensity = read_intensity(green,3,0);
	}
	lcd_init();
	lcd_write_str("Enter intensity for Blue: ",0,0,sizeof("Enter intensity for blueee"));
	wait(1);
	while(!(blue_intensity<=255))
	{
		for(i=0;i<3;i++)
		{
			blue[i]=0;
		}
		blue_intensity = read_intensity(blue,3,0);
	}
	lcd_init();
	/* Stores packet for G3() */
	/*sprintf(str1,"%d ",red_intensity);
	write_usb_serial_blocking(str1,4);
	sprintf(str1,"%d ",green_intensity);
	write_usb_serial_blocking(str1,4);
	sprintf(str1,"%d ",blue_intensity);
	write_usb_serial_blocking(str1,4);*/
	packet[0] = red_intensity;
	packet[1] = green_intensity;
	packet[2] = blue_intensity;
	return packet;
}

void G3()
{
	/* Calls on packets defined in G2(), creates a sequence of these packets which will be defined by the user, and displayed on Lighting Module when corresponding key pressed */
	int sequence[4];
	// Implement longer ability to display larger sequences
	/* Which packets needed for sequence */
	lcd_init();
	lcd_write_str("Please enter 1st packet for sequence: ",0,0,sizeof("Please enter 1st packet for sequence: "));
	wait(1);
	lcd_init();
	pos=0;
	state=0xFF;
	while(1) 
	{
		state=read_buttons();
		if(state == 0xDE)
		{
			break;
		}
		if(keypad_char_decode(last_state)!=keypad_char_decode(state) && 
		keypad_char_decode(state)!='G')
		{
			if(state == 0xE7) //val for A 
			{
				lcd_write_uint8_t(keypad_char_decode(0xE7),pos,0);
				sequence[pos] = A;
				pos++;
				//dmx_write(255,0,0);
			}
			if(state == 0xEB) // val for B 
			{
				lcd_write_uint8_t(keypad_char_decode(0xEB),pos,0);
				sequence[pos] = B;
				pos++;
				//dmx_write(0,255,0);
			}
			if(state == 0xED) // val for C 
			{
				lcd_write_uint8_t(keypad_char_decode(0xED),pos,0);
				sequence[pos] = C;
				pos++;
				//dmx_write(0,255,0);
			}
			if(state == 0xEE) // val for D
			{
				lcd_write_uint8_t(keypad_char_decode(0xEE),pos,0);
				sequence[pos] = D;
				pos++;
				//dmx_write(0,255,0);
			}
			
		}
		last_state=state;
		wait(0.01);
		/*
		if(state == ) // Val for C #ToDo
		{

		}
		if(state == ) // Val for D #ToDo
		{

		}
		*/
	}
	show_seq(sequence[0],sequence[1],sequence[2],sequence[3],1);
	
	
	/*lcd_write_str("Please enter 1st packet for sequence: ",0,0,sizeof("Please enter 1st packet for sequence: "));
	wait(1);*/
        /* More than 2 TODO */
	
	/* Sequence delay */
	/*lcd_write_str("Please enter time for  delay: ",0,0,sizeof("Please enter time for  delay: "));
	wait(1);
	lcd_init();*/
	/*while(key_pressed()!=-1 && key_pressed() <= 6)
	{
		time=key_pressed();
		show_seq(A, B, time);	
	}*/
}

int read_intensity(int intensity[], size_t size, int pos)
{
	/* Reads intensity value entered on Keypad, displays value on LCD Display and returns value as an integer */
	intensity_val=0;
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

void show_seq(int packet1[], int packet2[], int packet3[], int packet4[], int time)
{
	int r_pack1, g_pack1, b_pcak1, r_pack2, g_pack2, b_pcak2,r_pack3, g_pack3, b_pcak3,r_pack4, g_pack4, b_pcak4;
	/* Intensity values for 1st Packet */
	r_pack1 = packet1[0];
	g_pack1 = packet1[1];
	b_pcak1 = packet1[2];

	/* Intensity values for 2nd Packet */
	r_pack2 = packet2[0];
	g_pack2 = packet2[1];
	b_pcak2 = packet2[2];

	r_pack3 = packet3[0];
	g_pack3 = packet3[1];
	b_pcak3 = packet3[2];
	
	r_pack4 = packet4[0];
	g_pack4 = packet4[1];
	b_pcak4 = packet4[2];
	
	dmx_write(r_pack1, g_pack1, b_pcak1);
	wait(time);
	dmx_write(r_pack2, g_pack2, b_pcak2);
	wait(time);
	dmx_write(r_pack3, g_pack3, b_pcak3);
	wait(time);
	dmx_write(r_pack4, g_pack4, b_pcak4);
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
