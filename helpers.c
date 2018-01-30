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
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "helpers.h"
#define I2CDEV_M LPC_I2C1

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

/* Uses interrupts to implement a delay where ticks is in miliseconds */
void Delay(unsigned long tick)
{
	unsigned long systickcnt;
	systickcnt = SysTickCnt;
	while((SysTickCnt - systickcnt) < tick);
}	

/*void SysTick_Handler(void)
{
	SysTickCnt++;
}*/

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
	return(UART_Send((LPC_UART_TypeDef *)LPC_UART1,(uint8_t *)buf,length, BLOCKING));
}
/* init code for the USB serial line */
void serial_init(void)
{
	UART_CFG_Type UARTConfigStruct;			// UART Configuration structure variable
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;	// UART FIFO configuration Struct variable
	PINSEL_CFG_Type PinCfg;				// Pin configuration for UART
	/*
	 * Initialize UART pin connect
	 */
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	// USB serial first
	PinCfg.Portnum = 2;
	PinCfg.Pinnum = 0; //TX
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 1;//RX
	PINSEL_ConfigPin(&PinCfg);
		
	/* Initialize UART Configuration parameter structure to default state:
	 * - Baudrate = 9600bps
	 * - 8 data bit
	 * - 1 Stop bit
	void lcd_clear() * - None parity
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
	UART_Init((LPC_UART_TypeDef *)LPC_UART1, &UARTConfigStruct);		// Initialize UART0 peripheral with given to corresponding parameter
	UART_FIFOConfig((LPC_UART_TypeDef *)LPC_UART1, &UARTFIFOConfigStruct);	// Initialize FIFO for UART0 peripheral
	UART_TxCmd((LPC_UART_TypeDef *)LPC_UART1, ENABLE);			// Enable UART Transmit
	
}

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
char keypad_uint8_t_decode(int button_pattern)
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




