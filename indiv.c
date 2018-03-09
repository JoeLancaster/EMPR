#include "helpers.h"
#include "uart1.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "lpc17xx_rtc.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_systick.h"
#define ARR_SIZE(arr) ( sizeof((arr)) / sizeof((arr[0])) )
#define MAX_SEQ 20
#define NO_INPUT -1
#define TIME_OUT 3
#define DEBOUNCE 250 //Or use wait()
#define THRESHOLD 10
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
extern sys_flag, sys_time, break_flag;
int simon_seq[MAX_SEQ];
int current_lvl, current_col, seq_len, packets;
int colours[4] = {0,1,2,3};
int difficulty=1; // Set to 1 by default
int time_period=0x3F0;
float rate=1;
unsigned long init_guess_time;  //Change later
int guess;

//Can try to create a struct that has keypressed, colour and then frequency for sound

//Use arr[rand() % ARR_SIZE(arr)] to get random element from array where arr is name of array
char state = 0xFF;
char str[4];
char last_state = 0xFF;
int pos = 0;
int line = 0;
int arr[16], i;
time_t seed;
__attribute__((constructor))  static void init()
{
	serial_init1();
	uart1_init();
	timer_init();
	myDAC_init();
	SYSTICK_InternalInit(1);
	SYSTICK_IntCmd(ENABLE);
	SYSTICK_Cmd(ENABLE);
}

int main(void)
{
	setup();
	RTC_Init(LPC_RTC);
	RTC_ResetClockTickCounter(LPC_RTC);
	RTC_Cmd(LPC_RTC, ENABLE);	
	srand((int)RTC_GetTime(LPC_RTC, RTC_TIMETYPE_SECOND));
	/*while(1)
	{
		show_col(0, 1000);
		show_col(1, 1000);
		show_col(2, 1000);
		show_col(3, 1000);
		show_col(4, 1000);
	}*/
	simon_game();
	wait(1);
	loop();

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

void setup()
{
	/*  Basic setup of monitor, Keyboard, LCD Monitor, Lighting Module */
	// Will play sound  'hello' through speaker of some sort and cycle through  all 4 colours of the game
	lcd_init();
	lcd_write_str("Hello User", 0,0, sizeof("hello user"));
	memset(simon_seq, '2', MAX_SEQ);
	// May include voice over as oppose to displaying text, additonally may have lights synchronise with sound
	/*dmx_write(255,0, 0);
	dmx_wait(500);
	dmx_write(0,255,0);
	dmx_wait(500);
	dmx_write(0,0,255);
	dmx_wait(500);
	dmx_write(255,255,0);*/
	int i;
	for(i=0;i<4;i++){ show_col(i,250); }
	seq_len=0;
	difficulty=1;
	//simon_game();
}

void simon_game()
{
	dmx_clear();
	lcd_init();
	lcd_write_str("Please pick a   difficulty: ",0,0,sizeof("please pick a   difficulty: "));
	wait(1);
	lcd_init();
	//Keyboard  detects key press
	pos=0;
	state=0xFF;
	while(1)
	{
		state=read_buttons();
		/*if(state == 0x7E) // '*' moves to next stage //
		{
			break;
		}*/
		if(keypad_char_decode(last_state)!=keypad_char_decode(state) &&
		keypad_char_decode(state)!='G')
		{
			if(state <= 0xDB)
			{
				if(state == 0xBE){ lcd_init(); lcd_write_str("You entered 0 Reset to continue",0,0,32); wait(2); break;}
				if(state == 0x77){ difficulty=1; lcd_write_uint8_t('1',0,0); break;}
				if(state == 0xB7){ difficulty=2; lcd_write_uint8_t('2',0,0); break; }
				if(state == 0xD7){ difficulty=3; lcd_write_uint8_t('3',0,0); break; }
				if(state == 0x7B){ difficulty=4; lcd_write_uint8_t('4',0,0); break; }
				if(state == 0xBB){ difficulty=5; lcd_write_uint8_t('5',0,0); break; }
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
		wait(0.01);
	}
	if(difficulty==1){ seq_len=4; }
	if(difficulty==2){ seq_len=8; }
	if(difficulty==3){ seq_len=12; }
	if(difficulty==4){ seq_len=16; }
	if(difficulty==4){ seq_len=20; }
	//Creates pattern based on difficulty chosen, higher difficulty --> goes up to a longer sequence
	int i,k;
	for(k=0;k<seq_len; k++) { simon_seq[k] = 0; }
	int randIndex, prevIndex;
	randIndex=0;
	prevIndex=7;
	for (i=0; i<seq_len; i++)
	{
		while(randIndex==prevIndex){ randIndex=rand() % 4; }
		// simon_seq has array of colours(0to3)
		simon_seq[i] = colours[randIndex]; // Add size for colour array size
		prevIndex=randIndex;
	}
	/*current_lvl=1;
	char str1[4];
	sprintf(str, "%d ",difficulty);
	sprintf(str1, "%d ", seq_len);
	lcd_init();
	lcd_write_str(str,0,0,2);
	lcd_write_str(str1,0,1,3);*/
}

void dmx_clear()
{
	/* Clears lights on Lighting module */
	dmx_write(0,0,0);
}
void show_single_col(int col)
{
	/* Displays requested colour */
	int inten1,inten2, inten3;
	if(col == 0){ inten1=255; inten2=0; inten3=0; }  	// Red
	if(col == 1){ inten1=0; inten2=255; inten3=0; }  	// Green
	if(col == 2){ inten1=0; inten2=0; inten3=255; }		// Blue
	if(col == 3){ inten1=255; inten2=255; inten3=0; }  	// Yellow
	if(col == 4){ inten1=255; inten2=0; inten3=255; }  	// Magenta
	if(col == 5){ inten1=0; inten2=255; inten3=255; }  	// Cyan
	if(col == 6){ inten1=255; inten2=255; inten3=255; }  	// White
	dmx_write(inten1, inten2, inten3);
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


void show_seq(int current_lvl)
{
	// Adjusts time based on level player is on
	int time=0;
	if(current_lvl <= 5)
	{
		time=2000;
	}
	else if(current_lvl <= 13)
	{
		time=1000;
	}
	else
	{
		time=500;
	}
	int i;
	for(i=0; i<current_lvl; i++)
	{
		current_col=simon_seq[i]; // saves colour to be shown in variable
		show_col(simon_seq[i], time);
	}
}

int key_pressed()
{
	/* Returns key pressed by Player */
	// Remember to change to generator version
	int key=-1;
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
void lost(int last_col)
{
	/* Called when Player loses the game */
	//lcd_write_uint8_t(current_col,0,0);
	lcd_init();
	lcd_write_str("You Lost :( ",0,0,sizeof("you lost :( "));
	show_single_col(last_col);
	int lose;
	while(1)
	{
		//Play a losing game sound
		for(lose=0x3F0;lose<0x3F5; lose++)
		{
			dmx_wait(250, lose);
		}
	}
	//setup(); //Resets game
}

void won()
{
	/* Called when Player wins the game */
	lcd_init();
	lcd_write_str("You Won! :) ",0,0,sizeof("you won! :) "));
	int i;
	while(1)
	{
		// Cycles through 4 colours , add victory sound
		for (i=0; i<4; i++)
		{
			show_col(i,500);
		}
	}
	setup(); // Restarts game
}

void loop()
{
	/* Loop that runs as game is being played, checking Player's input */
	current_lvl=1;
	while(1)
	{
	// Displays seq that was configured i.e.  Computer sends pattern
		dmx_clear();
		wait(0.5);
		lcd_init();
		show_seq(current_lvl);
		dmx_clear();
		/*dmx_wait(250);
		dmx_clear();*/
		//lcd_write_str("here",0,0,5);
		// Player's input  read and processed
		int s;
		for (s=0; s<current_lvl; s++)
		{
			//	init_guess_time = ;
			guess=7;
			pos=0;
			state=0xFF;
			lcd_init();
			lcd_write_str("Please enter    Guess: ",0,0,sizeof("please enter    guess: "));
			wait(1);
			lcd_init();
			while(1)
			{
				state=read_buttons();
				if(state==0x7E){ break; }
				if(keypad_char_decode(last_state)!=keypad_char_decode(state) && keypad_char_decode(state)!='G')
				{
					if(state == 0xBE){ guess=0; lcd_write_uint8_t('0',pos,0);show_single_col(0); dmx_wait(500, RED);}
					if(state == 0x77){ guess=1; lcd_write_uint8_t('1',pos,0);show_single_col(1); dmx_wait(500, GREEN);;}
					if(state == 0xB7){ guess=2; lcd_write_uint8_t('2',pos,0);show_single_col(2); dmx_wait(500, BLUE);}
					if(state == 0xD7){ guess=3; lcd_write_uint8_t('3',pos,0);show_single_col(3); dmx_wait(500, YELLOW);}
					/*if(state == 0x7B){ guess=4; lcd_write_uint8_t('4',pos,0);show_single_col(4);}*/
					if(state > 0xD7)
					{	
						lcd_init();
						lcd_write_str("Please Enter a  Valid digit!",0,0,sizeof("please enter a  valid digita"));
						wait(2);
						lcd_init();
						pos=0;
					}
					pos++;

				}
				last_state=state;
				wait(0.01);
			}
			/* Catches digits that are not valid 
			if((!(guess <= 6)) || guess == -1)
			{
				lcd_init();
				lcd_write_str("Please Enter a  Valid digit!",0,0, sizeof("please enter a  valid digita"));
				wait(2);
				lcd_init();
				lcd_write_str("Please Restart  to try again", 0,0, sizeof("please restart  to try again"));
			}*/
			if(guess != (int)simon_seq[s])
			{
				lcd_init();
				lost(simon_seq[s]); // or use current_col
			}
		}
		lcd_init();
		lcd_write_str("Correct :)",0,0,sizeof("correct :)"));
		int correct;
		for(correct=0x3F8;correct<0x3FD; correct++)
		{
			dmx_wait(250, correct);
		}
		current_lvl++;
		if(current_lvl > seq_len)
		{
			lcd_init();
			won();
		}
	}
}







