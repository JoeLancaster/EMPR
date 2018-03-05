#include "lpc17xx_gpio.h"
#include "lpc17xx_systick.h"
#include "lpc17xx_libcfg_default.h"
#include "lpc17xx_systick.h"
#include "lpc17xx_uart.h"		
#include "lpc17xx_pinsel.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_dac.h"
#include "lpc_types.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_timer.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "helpers.h"
#include "uart1.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_systick.h"

#define I2CDEV_M LPC_I2C1
#define RED  0
#define GREEN 1
#define BLUE 2
#define YELLOW 3
#define MAGENTA 4
#define CYAN 5
#define WHITE 6
#define SIZE 3
extern sys_flag1, sys_time1, break_flag;
char state = 0xFF;
char last_state = 0xFF;
int sys_flag = -1;
int sys_time = -1;
int pos = 0;
int line = 0;
int A[3];
int B[3];
int C[3];
int D[3];
int time;
int packets;
int i,j;
int red[3], green[3], blue[3],red_intensity,green_intensity,blue_intensity,intensity_val;
char inten[3];
double SECOND = 480000; // Approximately 1 seconds worth of loops
volatile unsigned long SysTickCnt;
/* Uses lowest four bits of input integer and turns on appropriate led ports through bit masking */
void turn_on(uint8_t lights)
{

	uint8_t mask = 0x00;
	uint8_t current = GPIO_ReadValue(0x01) >> 16;

	lights  = lights << 2;
	mask = 0x04;
	current = (lights & mask) | current;
	
	lights = lights << 1;
	mask = 0x30;
	current = (lights & mask) | current;
	
	lights = lights << 1;
	mask = 0x80;
	current = (lights & mask) | current;
	
	GPIO_ClearValue(0x01, 0x00B40000);
	
	uint32_t new = current;
	new = new << 16;
	GPIO_SetValue(0x01, new);
	
}

/* Turns on corresponding light from 0 to 3 */
void turn_on_single(int light)
{
	switch(light)
	{
		case 0:
			turn_on(1);
			break;
		case 1:
			turn_on(2);
			break;
		case 2:
			turn_on(4);
			break;
		case 3:
			turn_on(8);
			break;
	}

}


/* Uses count based loop to wait for 'n' seconds */
void wait(double seconds)
{
	double time = (seconds * SECOND);
	double i=0;
	while(i < time)
	{
		i++;
	}
}

/* Uses interrupts to implement a delay where ticks is in miliseconds 
void Delay(unsigned long tick)
{
	unsigned long systickcnt;
	systickcnt = SysTickCnt;
	while((SysTickCnt - systickcnt) < tick);
}	
*/
/*void SysTick_Handler(void)
{
	SysTickCnt++;
}*/
void timer_init() {
  TIM_TIMERCFG_Type tcfg;
  TIM_ConfigStructInit(TIM_TIMER_MODE, &tcfg);
  tcfg.PrescaleOption = TIM_PRESCALE_USVAL;
  tcfg.PrescaleValue = 4;
  
  TIM_MATCHCFG_Type mcfg0;
  mcfg0.MatchChannel = 0;
  mcfg0.IntOnMatch = TRUE;
  mcfg0.ResetOnMatch = TRUE;
  mcfg0.StopOnMatch = FALSE;
  mcfg0.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;
  mcfg0.MatchValue = 22;

  
  TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &tcfg);
  TIM_Init(LPC_TIM1, TIM_TIMER_MODE, &tcfg);
  TIM_ConfigMatch(LPC_TIM0, &mcfg0);
  mcfg0.MatchChannel = 1;
  mcfg0.MatchValue = 3;
  TIM_ConfigMatch(LPC_TIM1, &mcfg0);
  
  NVIC_SetPriority(TIMER0_IRQn, ((0x01<<3)|0x01));
  NVIC_EnableIRQ(TIMER0_IRQn);

  NVIC_SetPriority(TIMER1_IRQn, ((0x01<<4)|0x01));
  NVIC_EnableIRQ(TIMER1_IRQn);
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
	sys_flag1=0;
	sys_time1=time;
	UART_ForceBreak(LPC_UART1);
	while(sys_flag!=-1);
	
}
void SysTick_Handler(void)
{
	if(sys_flag ==  -1)
	{
		return;
	}
	if(sys_flag >= 0)
	{
		sys_flag++;
	}
	if(sys_flag == sys_time)
	{
		LPC_UART1 -> LCR &= ~(UART_LCR_BREAK_EN);
		sys_flag = -1;
		SYSTICK_ClearCounterFlag();
	}
}

// Read options
int read_usb_serial_none_blocking(uint8_t *buf,int length)
{
	return(UART_Receive((LPC_UART_TypeDef *)LPC_UART0, (uint8_t *)buf, length, NONE_BLOCKING));
}

int read_uart1(uint8_t *buf,int length)
{
	return(UART_Receive((LPC_UART_TypeDef *)LPC_UART1, (uint8_t *)buf, length, NONE_BLOCKING));
}

// Write options
int write_usb_serial_blocking(uint8_t *buf,int length)
{
	return(UART_Send((LPC_UART_TypeDef *)LPC_UART0,(uint8_t *)buf,length, BLOCKING));
}

int write_uart1(uint8_t *buf,int length)
{
	while(UART_CheckBusy(LPC_UART1)==SET);
	return(UART_Send((LPC_UART_TypeDef *)LPC_UART1,(uint8_t *)buf,length, BLOCKING));
}
/* init code for the USB serial line */
/*void serial_init(void)
{
	UART_CFG_Type UARTConfigStruct;			// UART Configuration structure variable
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;	// UART FIFO configuration Struct variable
	PINSEL_CFG_Type PinCfg;	*/			// Pin configuration for UART
	/*
	 * Initialize UART pin connect
	 */
	/*PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	// USB serial first
	PinCfg.Portnum = 2;
	PinCfg.Pinnum = 0; //TX
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 1;//RX
	PINSEL_ConfigPin(&PinCfg);*/
		
	/* Initialize UART Configuration parameter structure to default state:
	 * - Baudrate = 9600bps
	 * - 8 data bit
	 * - 1 Stop bit
	void lcd_clear() * - None parity
	 */
	//UART_ConfigStructInit(&UARTConfigStruct);
	/* Initialize FIFOConfigStruct to default state:
	 * - FIFO_DMAMode = DISABLE
	 * - FIFO_Level = UART_FIFO_TRGLEV0
	 * - FIFO_ResetRxBuf = ENABLE
	 * - FIFO_ResetTxBuf = ENABLE
	 * - FIFO_State = ENABLE
	 */
	//UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);
	// Built the basic structures, lets start the devices/
	// USB serial
	//UART_Init((LPC_UART_TypeDef *)LPC_UART1, &UARTConfigStruct);		// Initialize UART0 peripheral with given to corresponding parameter
	//UART_FIFOConfig((LPC_UART_TypeDef *)LPC_UART1, &UARTFIFOConfigStruct);	// Initialize FIFO for UART0 peripheral
	//UART_TxCmd((LPC_UART_TypeDef *)LPC_UART1, ENABLE);			// Enable UART Transmit
	
//}

void serial_init1(void)
{
	UART_CFG_Type UARTConfigStruct;			// UART Configuration structure variable
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;	// UART FIFO configuration Struct variable
	PINSEL_CFG_Type PinCfg;				// Pin configuration for UART
	/*
	 * Initialize UART pin connect
	 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	// USB serial first
	PinCfg.Portnum = 0;
	PinCfg.Pinnum = 2;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 3;
	PINSEL_ConfigPin(&PinCfg);
		
	/* Initialize UART Configuration parameter structure to default state:
	 * - Baudrate = 9600bps
	 * - 8 data bit
	 * - 1 Stop bit
	 * - None parity
	 */
	UART_ConfigStructInit(&UARTConfigStruct);
	/* Initialize FIFOConfigStruct to default state:
	 * - FIFO_DMAMode = DISABLE
	 * - FIFO_Level = UART_FIFO_TRGLEV0
	 * - FIFO_ResetRxBuf = ENABLE
	 * - FIFO_ResetTxBuf = ENABLE
	 * - FIFO_State = ENABLE
	 */
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);
	// Built the basic structures, lets start the devices/
	// USB serial
	UART_Init((LPC_UART_TypeDef *)LPC_UART0, &UARTConfigStruct);		// Initialize UART0 peripheral with given to corresponding parameter
	UART_FIFOConfig((LPC_UART_TypeDef *)LPC_UART0, &UARTFIFOConfigStruct);	// Initialize FIFO for UART0 peripheral
	UART_TxCmd((LPC_UART_TypeDef *)LPC_UART0, ENABLE);			// Enable UART Transmit
	
	
}


/* Takes in 'x' and returns a character string containing its respective binary value */
const char *byte_to_binary(int x)
{
    static char b[9];
    b[0] = '\0';

    int z;
    for (z = 128; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }
    return b;
}

/*void by2bi_convert_single(char byte, uint8_t binary[])
{
	int i = 0;
	int b = 0x01;
	while(i<8)
	{
	  if((byte & b) == b)
		{
			binary[i] = '1';
		}
		else
		{
			binary[i] = '0';
		}
		b = b << 1;
		i++;
	}
}

void by2bi_convert(char byte[], uint8_t binary[], size_t bytes)
{
	char str[8*bytes];
	char temp[8];	
	int i=0;
	while(i<bytes)
	{
	  by2bi_convert_single(byte[i], (char *)temp); 
		strcat(str, temp);
	}
}
*/
/* Writes to i2c device */		
Status i2c_write(uint8_t addr, uint8_t data[])
{
	PINSEL_CFG_Type PinCfg;				// Pin configuration for I2C
	/*
	 * Initialize I2C pin connect
	 */
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Funcnum = 3;
	PinCfg.Pinnum = 0;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 1;
	PINSEL_ConfigPin(&PinCfg);
	
	I2C_Init(I2CDEV_M, 100000);

	I2C_Cmd(I2CDEV_M, ENABLE);
	uint8_t response[1];
	
	I2C_M_SETUP_Type transferMCfg;
	
	transferMCfg.sl_addr7bit = addr;
        transferMCfg.tx_data = data;
        transferMCfg.tx_length = sizeof(data);
        transferMCfg.rx_data = response;
        transferMCfg.rx_length = sizeof(response);
        transferMCfg.retransmissions_max = 3;
	return I2C_MasterTransferData(I2CDEV_M, &transferMCfg, 0);
	
}

/* Reads from i2c device */
void i2c_read(uint8_t addr, uint8_t data[], uint8_t response[])
{
	PINSEL_CFG_Type PinCfg;				// Pin configuration for I2C
	/*
	 * Initialize I2C pin connect
	 */
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Funcnum = 3;
	PinCfg.Pinnum = 0;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 1;
	PINSEL_ConfigPin(&PinCfg);
	
 	I2C_Init(I2CDEV_M, 100000);

	I2C_Cmd(I2CDEV_M, ENABLE);

	I2C_M_SETUP_Type transferMCfg;
	uint8_t inst[1];
	inst[0] = data[0];
	transferMCfg.sl_addr7bit = addr;
        transferMCfg.tx_data = inst;
        transferMCfg.tx_length = 1;
        transferMCfg.rx_data = response;
        transferMCfg.rx_length = 1;
        transferMCfg.retransmissions_max = 3;

	I2C_MasterTransferData(I2CDEV_M, &transferMCfg, 0);
	
}

/* Initialises LCD */
void lcd_init(void)
{
	uint8_t data[2];
	data[0] = 0x00;
	data[1] = 0x00;
	i2c_write(59, data);
	data[1] = 0x34; 
	i2c_write(59, data);
	data[1] = 0x0C; 
	i2c_write(59, data);
	data[1] = 0x06; 
	i2c_write(59, data);
	data[1] = 0x35; 
	i2c_write(59, data);
	data[1] = 0x04; 
	i2c_write(59, data);
	data[1] = 0x10; 
	i2c_write(59, data);
	data[1] = 0x42; 
	i2c_write(59, data);
	data[1] = 0x9F; 
	i2c_write(59, data);
	data[1] = 0x34; 
	i2c_write(59, data);
	data[1] = 0x02; 
	i2c_write(59, data);
	lcd_clear();
}

/* Clears LCD Screen */
void lcd_clear()
{
	int pos=0;
	while(pos<40)
	{
		lcd_write_uint8_t(' ', pos, 0);
		lcd_write_uint8_t(' ', pos, 1);
		pos++;
	}


		 	
}
 /* Writes a single uint8_tacter on the LCD Screen */
void lcd_write_uint8_t(uint8_t c, int pos, int line)
{
	uint8_t data[2];
	data[0] = 0x00;
	data[1] = 0x80 + pos+line*40;
	i2c_write(59, data);
	data[0] = 0xC0;
	data[1] = lcd_uint8_t_lookup(c);
	i2c_write(59, data);
	/*data[0] = 0xC0;
	data[1] = lcd_uint8_t_lookup(c);
	i2c_write(59, data);*/
}
 /* Writes a string on the LCD Screen */
void lcd_write_str(uint8_t str[], int pos, int line, size_t size)
{
	int i=0;
	int j=0;
	//int strlen=sizeof(str)/sizeof(str[0]);
	//int strlen = sizeof(str);
	while(i<size-1) //Subtracted by 1 to get rid of extra unnecessary uint8_tacters 
	{	
		if(line==0)
		{
			if(i<16)
			{
				lcd_write_uint8_t(str[i], pos+i, 0);
			}
			else
			{
				lcd_write_uint8_t(str[i], 24+j, 1); 
				j++;		
			}
		}
		else
		{
			lcd_write_uint8_t(str[i], pos+24+i, 1); 
			
		}
		i++;	
	}
}

/* Looks up given uint8_tacter(based on index from datasheet */
int lcd_uint8_t_lookup(uint8_t c)
{
	if(' '<=c && c>='!'){return c+128;}
	if('A'<=c && c>='Z'){return c+128;}
	if('a'<=c && c>='z'){return c+80;}
	if('0'<=c && c>='9'){return c+128;}
	if('#'<=c && c>='*'){return c+128;}

	if(c == 32){return 0xA0;}
	
	return 0x20;
}	

/* Checks rows on keyboard */
int check_rows(int cols)
{
	uint8_t temp[1];
	temp[0]=cols;
	temp[0]=temp[0]<<4;
	temp[0]=temp[0]+0x0F;
	uint8_t response[1];
	i2c_read(33, temp, response);
	return response[0]&0x0F;	

} 

/* Reads input from keyboard */
int read_buttons()
{
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
char keypad_char_decode(int button_pattern)
{
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

/* Initialises DAC */
void myDAC_init()
{
	PINSEL_CFG_Type PinCfg;	
	/*
	 * Initialize DAC pin connect
	 */ 
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;		
	PinCfg.Funcnum = 2;
	PinCfg.Pinnum = 26;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	
	DAC_Init(LPC_DAC);

}

/* Initialises ADC and returns channel for corresponding pinnum */
int myADC_init(int pinOut)
{
	int pinNum=pinOut+8;
	int channel=pinOut-15;
	PINSEL_CFG_Type PinCfg;	
	/*
	 * Initialize ADC pin connect
	 */ 
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;		
	PinCfg.Funcnum = 1;
	PinCfg.Pinnum = pinNum;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	ADC_Init(LPC_ADC, 200000);
	
	switch(pinNum)
	{
		case 26:	
			ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_3, ENABLE);
		case 25:
			ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_2, ENABLE);
		case 24:
			ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_1, ENABLE);
		case 23:
			ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0, ENABLE);	
	}

	ADC_BurstCmd(LPC_ADC,ENABLE);
	ADC_ChannelGetStatus(LPC_ADC, channel, 0);
	
	return channel;
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
				if(state == 0xBE){ key=0; show_col(key,0); }
				if(state == 0x77){ key=1; show_col(key,0); }
				if(state == 0xB7){ key=2; show_col(key,0); }
				if(state == 0xD7){ key=3; show_col(key,0); }
				if(state == 0x7B){ key=4; show_col(key,0); }
				if(state == 0xBB){ key=5; show_col(key,0); }
				if(state == 0xDB){ key=6; show_col(key,0); }
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

void show_col(uint8_t col, uint8_t time)
{
	/* Displays requested colour for desired duration */
	switch(col)
	{
		case 0:
			dmx_write(255,0,0);
			dmx_wait(time);
			dmx_write(0,0,0);
			dmx_wait(time);
			break;
		case 1:
			dmx_write(0,255,0);
			dmx_wait(time);
			dmx_write(0,0,0);
			dmx_wait(time);
			break;
		case 2:
			dmx_write(0,0,255);
			dmx_wait(time);
			dmx_write(0,0,0);
			dmx_wait(time);
			break;
		case 3:
			dmx_write(255,255,0);
			dmx_wait(time);
			dmx_write(0,0,0);
			dmx_wait(time);
			break;
		case 4:
			dmx_write(255,0,255);
			dmx_wait(time);
			dmx_write(0,0,0);
			dmx_wait(time);
			break;
		case 5:
			dmx_write(0,255,255);
			dmx_wait(time);
			dmx_write(0,0,0);
			dmx_wait(time);
			break;
		case 6:
			dmx_write(255,255,255);
			dmx_wait(time);
			dmx_write(0,0,0);
			dmx_wait(time);
			break;
	}
}
void show_seq1(int packet1[], int time)
{
	int r_pack1, g_pack1, b_pcak1;
	/* Intensity values for 1st Packet */
	r_pack1 = packet1[0];
	g_pack1 = packet1[1];
	b_pcak1 = packet1[2];
	
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
			wait(time);
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
	int r_pack1, g_pack1, b_pcak1, r_pack2, g_pack2, b_pcak2;
	/* Intensity values for 1st Packet */
	r_pack1 = packet1[0];
	g_pack1 = packet1[1];
	b_pcak1 = packet1[2];

	/* Intensity values for 2nd Packet */
	r_pack2 = packet2[0];
	g_pack2 = packet2[1];
	b_pcak2 = packet2[2];
	
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
			dmx_wait(time);
			dmx_write(r_pack2, g_pack2, b_pcak2);
			dmx_wait(time);
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
	
	int r_pack1, g_pack1, b_pcak1, r_pack2, g_pack2, b_pcak2,r_pack3, g_pack3, b_pcak3;
	/* Intensity values for 1st Packet */
	r_pack1 = packet1[0];
	g_pack1 = packet1[1];
	b_pcak1 = packet1[2];

	/* Intensity values for 2nd Packet */
	r_pack2 = packet2[0];
	g_pack2 = packet2[1];
	b_pcak2 = packet2[2];

	/* Intensity values for 3rd Packet */
	r_pack3 = packet3[0];
	g_pack3 = packet3[1];
	b_pcak3 = packet3[2];
	
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
			wait(time);
			dmx_write(r_pack2, g_pack2, b_pcak2);
			wait(time);
			dmx_write(r_pack3, g_pack3, b_pcak3);
			wait(time);		
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
	int r_pack1, g_pack1, b_pcak1, r_pack2, g_pack2, b_pcak2,r_pack3, g_pack3, b_pcak3,r_pack4, g_pack4, b_pcak4;
	/* Intensity values for 1st Packet */
	r_pack1 = packet1[0];
	g_pack1 = packet1[1];
	b_pcak1 = packet1[2];

	/* Intensity values for 2nd Packet */
	r_pack2 = packet2[0];
	g_pack2 = packet2[1];
	b_pcak2 = packet2[2];

	/* Intensity values for 3rd Packet */
	r_pack3 = packet3[0];
	g_pack3 = packet3[1];
	b_pcak3 = packet3[2];

	/* Intensity values for 4th Packet */
	r_pack4 = packet4[0];
	g_pack4 = packet4[1];
	b_pcak4 = packet4[2];
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
			wait(time);
			dmx_write(r_pack2, g_pack2, b_pcak2);
			wait(time);
			dmx_write(r_pack3, g_pack3, b_pcak3);
			wait(time);
			dmx_write(r_pack4, g_pack4, b_pcak4);
			wait(time);
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



