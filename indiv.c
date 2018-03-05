#include "helpers.h"
#include "uart1.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "lpc17xx_rtc.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_systick.h"
#define ARR_SIZE(arr) ( sizeof((arr)) / sizeof((arr[0])) )
#define MAX_SEQ 31
#define NO_INPUT -1
#define TIME_OUT 3
#define DEBOUNCE 250 //Or use wait()
#define THRESHOLD 10
#define RED  0
#define GREEN 1
#define BLUE 2
#define YELLOW 3
#define MAGENTA 4
#define CYAN 5
#define WHITE 6
extern sys_flag, sys_time, break_flag;
uint8_t simon_seq[MAX_SEQ];
uint8_t current_lvl;
uint8_t current_col;
uint8_t colours[4] = {0,1,2,3};
int difficulty=1; // Set to 1 by default
uint8_t seq_len;
unsigned long init_guess_time;  //Change later
int8_t guess;

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
	//show_seq();
	simon_game();
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

void dmx_wait(int time)
{
	sys_flag=0;
	sys_time=time;
	UART_ForceBreak(LPC_UART1);
	while(sys_flag!=-1);
	
}

void setup()
{
	/*  Basic setup of monitor, Keyboard, LCD Monitor, Lighting Module */
	// Will play sound  'hello' through speaker of some sort and cycle through  all 4 colours of the game
	lcd_init();
	lcd_write_str("Hello User", 0,0, sizeof("hello user")
	memset(simon_seq, '2', MAX_SEQ);
	// May include voice over as oppose to displaying text, additonally may have lights synchronise with sound
	dmx_write(255,0, 0);
	dmx_wait(250);
	dmx_write(0,255,0);
	dmx_wait(250);
	dmx_write(0,0,255);
	dmx_wait(250);
	dmx_write(255,255,0);
	simon_game();
}

void simon_game()
{
	lcd_init();
	lcd_write_str("Please pick a difficulty from 1-4",0,0,sizeof("please pick a difficulty from 1-4");
	//Keyboard  detects key press
	state=read_buttons();
	while(keypad_uint8_t_decode(last_state)==keypad_uint8_t_decode(state) && keypad_uint8_t_decode(state)=='G')
	{
		state=read_buttons();
		last_state=state;	
	} //Possible error here in while loop
	char c = keypad_uint8_t_decode(state);
	if(!isdigit(c))
	{
		lcd_init();
		lcd_write_str("Please enter a valid digit",0,0,sizeof("please enter a valid digit"));
		setup(); // Restarts Game, can say if more time saves state of game, etc.
	}
	//lcd_write_uint8_t(state,0,0);
	difficulty = c-'0';
	switch(difficulty) 
	{
		case 1:
			seq_len =  8 ;
			break;
		case 2:
			seq_len = 14;
			break;
		case 3:
			seq_len = 20;
			break;
		case 4:
			seq_len = 31;
			break;

	}
	//Creates pattern based on difficulty chosen, higher difficulty --> goes up to a longer sequence
	for (int i=0; i<seq_len; i++)
	{
		randIndex=rand() % 4;
		// simon_seq has array of colours(0to3)
		simon_seq[i] = colours[randIndex]; // Add size for colour array size
	}
	current_lvl=1;
}

void dmx_clear()
{
	/* Clears lights on Lighting module */
	dmx_write(0,0,0);
}

void show_col(uint8_t col, uint8_t time)
{
	/* Displays requested colour for desired duration */
	int inten1,inten2, inten3;
	if(col == 0){ inten1=255; inten2=0; inten3=0; }  	// Red
	if(col == 1){ inten1=0; inten2=255; inten3=0; }  	// Green
	if(col == 2){ inten1=0; inten2=0; inten3=255; }		// Blue
	if(col == 3){ inten1=255; inten2=255; inten3=0; }  	// Yellow
	if(col == 4){ inten1=255; inten2=0; inten3=255; }  	// Magenta
	if(col == 5){ inten1=0; inten2=255; inten3=255; }  	// Cyan
	if(col == 6){ inten1=255; inten2=255; inten3=255; }  	// White
	dmx_write(inten1, inten2, inten3);
	dmx_wait(time);
}

void show_seq()
{
	// Adjusts time based on level player is on
	uint8_t time;
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

	for(int i=0; i<current_lvl; i++)
	{
		current_col=simon_seq[j]; // saves colour to be shown in variable
		show_col(simon_seq[j], time);
		dmx_wait(time);
	}
}

int key_pressed()
{
	/* Returns key pressed by Player */
	// Remember to change to generator version
	int key=-1;
	while(keypad_uint8_t_decode(last_state)==keypad_uint8_t_decode(state) && keypad_uint8_t_decode(state)=='G')
	{
		state=last_state;
		state=read_buttons();
	}
	char r = keypad_uint8_t_decode(state);
	if(!isdigit(r))
	{
		return key;
	}
 	key = r - '0';
	return key;
}
void lost(uint8_t key)
{
	/* Called when Player loses the game */
	lcd_write_char(current_col,0,0);
	show_col(current_col, 5000); // Need to adjust code to account for no 0 on keyboard
	//Play a losing game sound
	setup(); //Resets game
}

void won()
{
	/* Called when Player wins the game */
	while(1)
	{
		// Cycles through 4 colours , add victory sound
		for (int i=0; i<4; i++)
		{
			show_col(i,500);
		}
		state=read_buttons();
		while(keypad_uint8_t_decode(last_state)==keypad_uint8_t_decode(state) && keypad_uint8_t_decode(state)=='G')
		{
			last_state=state;
			state=read_buttons();
		}
		char w=keypad_uint8_t_decode(state);
		if(w == '*')
		{
			break;
		}
	}
	setup(); // Restarts game
}

void loop()
{
	/* Loop that runs as game is being played, checking Player's input */
	while(1)
	{
	// Displays seq that was configured i.e.  Computer sends pattern
		show_seq();

		// Player's input  read and processed
		for (int s=0; s<current_lvl; s++)
		{
			//	init_guess_time = ;
			guess = NO_INPUT;
			guess=key_pressed();
			/* Catches digits that are not valid */
			if((!(key <= 6)) || key == -1)
			{
				lcd_init();
				lcd_write_str("Please Enter a  Valid digit!",0,0, sizeof("please enter a  valid digita"));
				wait(2);
				lcd_init();
				lcd_write_str("Please Restart  to try again", 0,0, sizeof("please restart  to try again"));
			}
			if(guess != simon_seq[s])
			{
				lost(simon_seq[s]); // or use current_col
			}
		}
		current_lvl++;
		if(current_lvl > seq_len)
		{
			//wait(1);
			won();
		}
	}
}







