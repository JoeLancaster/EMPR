#include "helpers.h"
#include <math.h>
#define I2CDEV_M LPC_I2C1

double SECOND = 480000; // Approximately 1 seconds worth of loops

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

int write_usb_serial_blocking(uint8_t *buf,int length)
{
	return(UART_Send((LPC_UART_TypeDef *)LPC_UART0,(uint8_t *)buf,length, BLOCKING));
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
	UARTConfigStruct.Baud_rate = 460800;
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

Status i2c_write(uint8_t addr, uint8_t data[]) {
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
void i2c_read(uint8_t addr, uint8_t data[], uint8_t response[]) {
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



/* Initialises DAC */
void myDAC_init() {
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
int myADC_init(int pinOut) {
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

int arcmp(uint8_t * ar1, uint8_t * ar2, size_t size){ 
  int r = 0xFF;
  int i;
  for(i = 0; i < size; i++){
    r &= (ar1[i] == ar2[i]);
    if(!r){return r;}
  }
  return r;
}

int packets_ps(int rate){
  return floor(1000 / rate);
}
