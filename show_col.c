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
#define RED  0;
#define GREEN 1;
#define BLUE 2;
#define YELLOW 3;
#define MAGENTA 4;
#define CYAN 5;
#define WHITE 6;
extern sys_flag, sys_time, break_flag;
uint8_t colours[4] = {0,1,2,3};
uint8_t simon_seq[MAX_SEQ];
uint8_t current_lvl;
uint8_t current_col;
char state = 0xFF;
char str[4];
char last_state = 0xFF;
int pos = 0;
int line = 0;
int arr[16];
int i;
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
	//int key=key_pressed();
	/* Catches digits that are not valid */
	/*if((!(key <= 6)) || key == -1)
	{
		lcd_init();
		lcd_write_str("Please Enter a  Valid digit!",0,0,sizeof("please enter a  valid digita"));
	wait(2);
	lcd_init();
	lcd_write_str("Please Restart  to try again", 0,0, sizeof("please restart  to try again"));
	}*/
	/*while(1)
	{
		show_col(0, 1000);
		show_col(1, 1000);
		show_col(2, 1000);
		show_col(3, 1000);
		show_col(4, 1000);
	}*/
	int count;
	/*while(1)
	{
		if(count == 12){ break; }
		show_col(0, 1000);
		show_col(1, 1000);
		count++;
	}*/
	show_seq();
}

void show_col(int col, int time)
{
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

void dmx_wait(uint32_t time)
{
	sys_flag=0;
	sys_time=time;
	UART_ForceBreak(LPC_UART1);
	while(sys_flag!=-1);
	
}

void setup()
{
	/*  Basic setup of monitor, Keyboard, LCD Monitor, Lighting Module */
	lcd_init();
	lcd_write_str("Hello User", 0,0, sizeof("hello user"));
	memset(simon_seq, '2', MAX_SEQ);
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
		return key;
	}
 	key = r - '0';
	return key;
}

void show_seq()
{
	// Adjusts time based on level player is on
	int i,k;
	int randIndex, prevIndex;
	prevIndex=7;
	//seed=time(NULL);
	//srand(time(NULL));
	//srand(seed);
	for(k=0;k<16; k++) { simon_seq[k] = 0; }
	for (i=0;i<16; i++) //seq_len
	{
		while(randIndex==prevIndex){ randIndex=rand() % 4; }
		//while(rand>=4);
		simon_seq[i] = colours[randIndex];
		prevIndex=randIndex;
	}
	// simon_seq has array of colours(0to3)
	current_lvl=12;
	uint8_t time;
	if(current_lvl <= 5)
	{
		time=3000;
	}
	else if(current_lvl <= 13)
	{
		time=2000;
	}
	else
	{
		time=1000;
	}
	int j;
	for(j=0;j<current_lvl; j++)
	{
		current_col=simon_seq[j]; // saves colour to be shown in variable
		show_col(simon_seq[j], 1000); // change to time
	}
}

int random(int min, int max)
{
    int range, result, cutoff;
 
    if (min >= max)
        return min; // only one outcome possible, or invalid parameters
    range = max-min+1;
    cutoff = (RAND_MAX / range) * range;
 
    // Rejection method, to be statistically unbiased.
    do {
        result = rand();
    } while (result >= cutoff);
 
    return result % range + min;
}







