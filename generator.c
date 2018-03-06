#include "helpers.h"
#include "uart1.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_systick.h"
#define DEFAULTA 0x3F1
#define DEFAULTB 0x3F3
#define DEFAULTC 0x3F5
#define DEFAULTD 0x3F7      
#define RED  0x3F2
#define GREEN 0x3F4
#define BLUE 0x3F6
#define YELLOW 0x3F8
#define MAGENTA 0x3FA
#define CYAN 0x3FC
#define WHITE 0x3FE
#define SIZE 3
extern sys_flag, sys_time;
char state = 0xFF;
char stateMain = 0xFF;
char last_state = 0xFF;
char last_stateMain = 0xFF;
int pos = 0;
int line = 0;
int A[3];
int B[3];
int C[3];
int D[3];
int time;
int packets;
int time_period=0x3F0;
float rate=1;
int i,j;
int red[3], green[3], blue[3],red_intensity,green_intensity,blue_intensity,intensity_val;
char inten[3];


extern break_flag;

__attribute__((constructor))  static void init()
{
	serial_init1();
	uart1_init();
	timer_init();
	myDAC_init();
	//SysTick_Config(SystemCoreClock/1000 - 1); 
	SYSTICK_InternalInit(1);
	SYSTICK_Cmd(ENABLE);
	SYSTICK_IntCmd(ENABLE);	
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

/*void dmx_wait(int time)
{
	sys_flag=0;
	sys_time=time;
	UART_ForceBreak(LPC_UART1);
	while(sys_flag!=-1);
}*/

void dmx_wait(int time, int sound)
{
	sys_flag=0;
	sys_time=time;
	UART_ForceBreak(LPC_UART1);
	while(sys_flag!=-1)
	{
		/*for(time_period = 0x3FD; time_period < 0x3FF; time_period++)
        	{
        		DAC_UpdateValue ( LPC_DAC,(uint32_t)(time_period*rate));
      		}
        	rate += 0.01;

       		 if(rate >= 1)
        	{
        		rate = 0;
        	}*/
		play_sound(sound);
	}
	
}

void play_sound(int time_period)
{
	//time_period=0x3F1;
	for(time_period;time_period < 0x3FF; time_period++)
        {
        	DAC_UpdateValue ( LPC_DAC,(uint32_t)(time_period*rate));
        }
        rate += 0.01;

        if(rate >= 1)
        {
	        rate = 0;
        }
}

int main(void)
{
	setup();
	int count=0;
	int joe;
	/*for(joe=0;joe<7;joe++)
	{
		show_col(joe,1000);
	}
	dmx_clear();*/
	
	/*for(joe = 1; joe < 200; joe++)
	{
		dmx_write(25 * joe, 0,0);
		dmx_wait(10);
		dmx_write(0, 25 * joe, 0);
		dmx_wait(10);
		dmx_write(0,0,25 * joe);
		dmx_wait(10);

		count++;
	}*/
	//write_usb_serial_blocking("Here",4);
	/*for(i=0;i<1;i++)
	{
		A[i]=255;
	}
	B[0]=0;
	B[1]=255;
	B[2]=0;

	C[0]=0;
	C[1]=0;
	C[2]=255;*/
	//show_seq2(B,C,1);*/
	//G2(A,SIZE);
	/*lcd_init();
	lcd_write_str("Next Packet ", 0,0, sizeof("Next Packet "));
	wait(1);
	G2(B,SIZE);*/
	//show_seq2(A,B,1);
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
	/*dmx_clear();
	//G2(A,SIZE);
	dmx_clear();
	//G2_single();
	for(i=0;i<1;i++)
	{
		A[i]=255;
	}
	//wait(1);*/
	/*lcd_init();
	lcd_write_str("Next Packet ", 0,0, sizeof("Next Packet "));
	wait(1);*/
	//G2(B,SIZE);
	/*for(i=0;i<2;i++)
	{*/
		/*B[0]=0;
		B[1]=255;
		B[2]=0;

		C[0]=0;
		C[1]=0;
		C[2]=255;

		D[0]=255;
		D[1]=255;
		D[2]=255;*/
	//}
	//G3();
	//dmx_write(C[0],C[1],C[2]);
	/*lcd_init();
	lcd_write_str("G3", 0,0, sizeof("g3"));
	wait(1);
	G3();*/
	//show_seq4(A,B,C,D,1);

	// Code for Demo  //
	// G1 //
	pos=0;
	stateMain=0xFF;
	lcd_init();
	lcd_write_str("G1", 0,0, sizeof("g1"));
	wait(1);
	G1();
	dmx_clear();
	// G2 //
	pos=0;
	stateMain=0xFF;
	lcd_init();
	lcd_write_str("G2", 0,0, sizeof("g2"));
	wait(1);
	lcd_init();
	lcd_write_str("Please enter number of packets: ", 0,0, sizeof("please enter number of packets: "));
	wait(1);
	lcd_init();
	while(1)
	{
		lcd_init();
		stateMain = read_buttons();
		if(stateMain == 0x7E) // '*' moves to next stage //
		{
			break;
		}
		// Test following code
		if(keypad_char_decode(last_stateMain)!=keypad_char_decode(stateMain) &&
		keypad_char_decode(stateMain)!='G')
		{
				if(stateMain == 0x77)
				{
					G2(A,SIZE);
					lcd_init();
					show_seq1(A,1000);
					break;
				}
				if(stateMain == 0xB7)
				{
					G2(A,SIZE);
					lcd_init();
					lcd_write_str("Next Packet ", 0,0, sizeof("Next Packet "));
					lcd_init();
					G2(B,SIZE);
					lcd_init();
					show_seq2(A,B,1000);
					break;
				}
				if(stateMain == 0xD7)
				{
					G2(A,SIZE);
					lcd_init();
					lcd_write_str("Next Packet ", 0,0, sizeof("Next Packet "));
					lcd_init();
					G2(B,SIZE);
					lcd_init();
					lcd_write_str("Next Packet ", 0,0, sizeof("Next Packet "));
					lcd_init();
					G2(C,SIZE);
					lcd_init();
					show_seq3(A,B,C,1000);
					break;
				}
				if(stateMain == 0x7B)
				{
					G2(A,SIZE);
					lcd_init();
					lcd_write_str("Next Packet ", 0,0, sizeof("Next Packet "));
					lcd_init();
					G2(B,SIZE);
					lcd_init();
					lcd_write_str("Next Packet ", 0,0, sizeof("Next Packet "));
					lcd_init();
					G2(C,SIZE);
					lcd_init();
					lcd_write_str("Next Packet ", 0,0, sizeof("Next Packet "));
					lcd_init();
					G2(D,SIZE);
					lcd_init();
					show_seq4(A,B,C,D,1000);
					break;
				}
				if(stateMain == 0xBE)
				{
					lcd_init();
					lcd_write_str("You entered 0 Reset to continue",0,0,32);
					wait(2);
					lcd_init();
				}
				if(stateMain > 0xD7)
				{
					
					lcd_init();
					lcd_write_str("Please Enter a  Valid digit!",0,0,sizeof("please enter a  valid digita"));
					wait(2);
					lcd_init();
	
				}
	    }		
	    last_stateMain=stateMain;
	}
	lcd_init();
	lcd_write_str("Success", 0,0, sizeof("success"));
	// G3 //
	lcd_init();
	lcd_write_str("G3", 0,0, sizeof("g3"));
	wait(1);
	pos=0;
	stateMain=0xFF;
	lcd_init();
	lcd_write_str("Please enter length of sequence: ", 0,0, sizeof("Please enter length of sequence: "));
	wait(1);
	lcd_init();
	while(1)
	{
		stateMain=read_buttons();
		if(stateMain == 0x7E) // '*' moves to next stage //
		{
			break;
		}
		if(keypad_char_decode(last_stateMain)!=keypad_char_decode(stateMain) &&
		keypad_char_decode(stateMain)!='G')
		{
			if(stateMain == 0x77)  // Value for 1
			{
				G3(1);
			}
			if(stateMain == 0xB7)  // Value for 2
			{
				G3(2);
			}
			if(stateMain == 0xD7)  // Value for 3
			{
				G3(3);
			}
			if(stateMain == 0x7B)  // Value for 4
			{
				G3(4);
			}
			if(stateMain == 0xBE)  // Value for 0
			{
				lcd_init();
				lcd_write_str("You entered 0 Reset to continue",0,0,32);
				wait(2);
				lcd_init();
			}
			if(stateMain > 0xD7)
			{	
				lcd_init();
				lcd_write_str("Please Enter a  Valid digit!",0,0,sizeof("please enter a  valid digita"));
				wait(2);
				lcd_init();
			}
		}
	}
	// END //
	lcd_init();
	lcd_write_str("Generator Stage Complete :)",0,0,sizeof("generator stage complete :)"));
	
}

void G1()
{
 /*	Displays fixed pattern when user enters a corresponding number on the keypad   */
	int key;
	state=0xFF;
	dmx_clear(); // Clears the Lighting module
	while(1)
	{
		state=read_buttons();
		lcd_clear();
		if(state == 0x7E)
		{
			break;
		}
		if(keypad_char_decode(last_state)!=keypad_char_decode(state) &&
		keypad_char_decode(state)!='G')
		{
			if(state <= 0xDB)
			{
				if(state == 0xBE){ key=0; dmx_write(255,0,0); }
				if(state == 0x77){ key=1; dmx_write(0,255,0); }
				if(state == 0xB7){ key=2; dmx_write(0,0,255); }
				if(state == 0xD7){ key=3; dmx_write(255,255,0); }
				if(state == 0x7B){ key=4; dmx_write(255,0,255); }
				if(state == 0xBB){ key=5; dmx_write(0,255,255); }
				if(state == 0xDB){ key=6; dmx_write(255,255,255); }
			}
			else
			{
				lcd_init();
				dmx_clear();
				lcd_write_str("Please Enter a  Valid digit!",0,0,sizeof("please enter a  valid digita"));
				wait(2);		
			}	
		}
		last_state=state;
	}
	/*while(key_pressed()!=-1 && key_pressed() <= 6)
	{
		key=key_pressed();
		show_col(key, 0);
	}*/
	/* Catches digits that are not Valid */
	/*dmx_write(255,0,0); // Displays RED indicating an error
	lcd_init();
	lcd_write_str("Please Enter a  Valid digit!",0,0,sizeof("please enter a  valid digita"));
	wait(2);
	lcd_init();
	lcd_write_str("Please Restart  to try again", 0,0, sizeof("please restart  to try again"));*/
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
	/*sprintf(str1,"%d ",red_intensity);
	write_usb_serial_blocking(str1,4);
	sprintf(str1,"%d ",green_intensity);
	write_usb_serial_blocking(str1,4);
	sprintf(str1,"%d ",blue_intensity);
	write_usb_serial_blocking(str1,4);*/

	/* Stores packet for G3() */
	packet[0] = red_intensity;
	packet[1] = green_intensity;
	packet[2] = blue_intensity;

	return packet;
}

void G3(int length)
{
	/* Calls on packets defined in G2(), creates a sequence of these packets which will be defined by the user, and displayed on Lighting Module when corresponding key pressed */
	int sequence[length];
	/* Which packets needed for sequence */
	lcd_init();
	lcd_write_str("Please enter order of sequence: ",0,0,sizeof("Please enter order of sequence: "));
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
			}
			if(state == 0xEB) // val for B
			{
				lcd_write_uint8_t(keypad_char_decode(0xEB),pos,0);
				sequence[pos] = B;
				pos++;
			}
			if(state == 0xED) // val for C
			{
				lcd_write_uint8_t(keypad_char_decode(0xED),pos,0);
				sequence[pos] = C;
				pos++;
			}
			if(state == 0xEE) // val for D
			{
				lcd_write_uint8_t(keypad_char_decode(0xEE),pos,0);
				sequence[pos] = D;
				pos++;
			}
		}
		last_state=state;
		wait(0.01);
	}
	if(length == 1){ show_seq1(sequence[0],1000); }
	if(length == 2){ show_seq2(sequence[0],sequence[1],1000); }
	if(length == 3){ show_seq3(sequence[0],sequence[1],sequence[2],1000); }
	if(length == 4){ show_seq4(sequence[0],sequence[1],sequence[2],sequence[3],1000); }
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

void show_col(int col, int time)
{
	/* Displays requested colour for desired duration */
	int inten1,inten2, inten3, sound;
	if(col == 0){ inten1=255; inten2=0; inten3=0; sound=RED; }  	// Red
	if(col == 1){ inten1=0; inten2=255; inten3=0; sound=GREEN;}  	// Green
	if(col == 2){ inten1=0; inten2=0; inten3=255; sound=BLUE;}      // Blue
	if(col == 3){ inten1=255; inten2=255; inten3=0; sound=YELLOW;}  // Yellow
	if(col == 4){ inten1=255; inten2=0; inten3=255; sound=MAGENTA;} // Magenta
	if(col == 5){ inten1=0; inten2=255; inten3=255; sound=CYAN;}    // Cyan
	if(col == 6){ inten1=255; inten2=255; inten3=255; sound=WHITE;} // White
	dmx_write(inten1, inten2, inten3);
	dmx_wait(time, sound);
}

void show_seq1(int packet1[], int time)
{
	int r_pack1, g_pack1, b_pcak1, sound;
	/* Intensity values for 1st Packet */
	r_pack1 = packet1[0];
	g_pack1 = packet1[1];
	b_pcak1 = packet1[2];
	sound=DEFAULTA;
	state=0xFF;
	while(1)
	{
		state=read_buttons();
		if(state == 0x7E)
		{
			dmx_clear();
			break;
		}
		if(state == 0xFF)
		{
			dmx_write(r_pack1, g_pack1, b_pcak1);
			dmx_wait(time, sound);
			dmx_clear();
		}
		/*else
		{
			dmx_clear();
			break;
		}*/
	}
}

void show_seq2(int packet1[], int packet2[], int time)
{
	int r_pack1, g_pack1, b_pcak1, sound1, r_pack2, g_pack2, b_pcak2, sound2;
	/* Intensity values for 1st Packet */
	r_pack1 = packet1[0];
	g_pack1 = packet1[1];
	b_pcak1 = packet1[2];
	sound1 = DEFAULTA;
	/* Intensity values for 2nd Packet */
	r_pack2 = packet2[0];
	g_pack2 = packet2[1];
	b_pcak2 = packet2[2];
	sound2 = DEFAULTB;

	state=0xFF;
	while(1)
	{
		state=read_buttons();
		if(state == 0x7E)
		{
			dmx_clear();
			break;
		}
		if(state==0xFF)
		{
			dmx_write(r_pack1, g_pack1, b_pcak1);
			dmx_wait(time, sound1);
			dmx_write(r_pack2, g_pack2, b_pcak2);
			dmx_wait(time, sound2);
		}
		/*else
		{
			dmx_clear();
			break;
		}*/
	}
}

void show_seq3(int packet1[], int packet2[], int packet3[], int time)
{
	
	int r_pack1, g_pack1, b_pcak1, sound1, r_pack2, g_pack2, b_pcak2, sound2, r_pack3, g_pack3, b_pcak3, sound3;
	/* Intensity values for 1st Packet */
	r_pack1 = packet1[0];
	g_pack1 = packet1[1];
	b_pcak1 = packet1[2];
	sound1 = DEFAULTA;
	/* Intensity values for 2nd Packet */
	r_pack2 = packet2[0];
	g_pack2 = packet2[1];
	b_pcak2 = packet2[2];
	sound2 = DEFAULTB;
	/* Intensity values for 3rd Packet */
	r_pack3 = packet3[0];
	g_pack3 = packet3[1];
	b_pcak3 = packet3[2];
	sound3 = DEFAULTC;

	state=0xFF;
	while(1)
	{
		state=read_buttons();
		if(state == 0x7E)
		{
			dmx_clear();
			break;
		}
		if(state==0xFF)
		{
			dmx_write(r_pack1, g_pack1, b_pcak1);
			dmx_wait(time, sound1);
			dmx_write(r_pack2, g_pack2, b_pcak2);
			dmx_wait(time, sound2);
			dmx_write(r_pack3, g_pack3, b_pcak3);
			dmx_wait(time, sound3);	
		}
		/*else
		{
			dmx_clear();
			break;
		}*/
	}
}
 
void show_seq4(int packet1[], int packet2[], int packet3[], int packet4[], int time)
{
	int r_pack1, g_pack1, b_pcak1, sound1, r_pack2, g_pack2, b_pcak2, sound2, r_pack3, g_pack3, b_pcak3, sound3, r_pack4, g_pack4, b_pcak4, sound4;
	/* Intensity values for 1st Packet */
	r_pack1 = packet1[0];
	g_pack1 = packet1[1];
	b_pcak1 = packet1[2];
	sound1 = DEFAULTA;
	/* Intensity values for 2nd Packet */
	r_pack2 = packet2[0];
	g_pack2 = packet2[1];
	b_pcak2 = packet2[2];
	sound2 = DEFAULTB;
	/* Intensity values for 3rd Packet */
	r_pack3 = packet3[0];
	g_pack3 = packet3[1];
	b_pcak3 = packet3[2];
	sound3 = DEFAULTC;
	/* Intensity values for 4th Packet */
	r_pack4 = packet4[0];
	g_pack4 = packet4[1];
	b_pcak4 = packet4[2];
	sound4 = DEFAULTD;

	state=0xFF;
	while(1)
	{
		state=read_buttons();
		if(state == 0x7E)
		{
			dmx_clear();
			break;
		}
		if(state==0xFF)
		{
			dmx_write(r_pack1, g_pack1, b_pcak1);
			dmx_wait(time, sound1);
			dmx_write(r_pack2, g_pack2, b_pcak2);
			dmx_wait(time, sound2);
			dmx_write(r_pack3, g_pack3, b_pcak3);
			dmx_wait(time, sound3);
			dmx_write(r_pack4, g_pack4, b_pcak4);
			dmx_wait(time, sound4);
		}
		/*else
		{
			dmx_clear();
			break;
		}*/
	}
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
