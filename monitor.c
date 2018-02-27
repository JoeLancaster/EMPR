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
#include "helpers.h"
#include "ring_buffer.h"


#define uart1 ((LPC_UART1_TypeDef *)LPC_UART1)
#define RB_MAX 1024
ring_buf_t rb;
int count;
int lcd_count = 1;
int pos=0;
uint8_t uart_break_flag;

__attribute__((constructor)) static void init() {
  rb_init(&rb, RB_MAX);
  serial_init1();
  uart1_init();
  uart_break_flag = 0;


  NVIC_EnableIRQ(UART1_IRQn);
  UART_IntConfig(LPC_UART1, UART_INTCFG_RBR, ENABLE);
  UART_IntConfig(LPC_UART1, UART_INTCFG_RLS, ENABLE);
  lcd_init();
}

void ua1hdl(LPC_UART_TypeDef * ua1) {
  all_off();
  static const size_t rxb_size = 14;
  uint8_t rxb[rxb_size];
  uint32_t linestat = UART_GetLineStatus(uart1);
  uart_break_flag |= (linestat & UART_LINESTAT_BI); 
  if(linestat & UART_LINESTAT_OE) {
    led_number(0xFF);
  }
  if(linestat & UART_LINESTAT_RXFE) {
    UART_ReceiveByte(ua1); //discard erroneous byte
   }
  if(linestat & UART_LINESTAT_RDR) {
    int tmp = UART_Receive(ua1, rxb, rxb_size, NONE_BLOCKING);
    if(tmp < 1){return;}
    int i;
    for(i = 0; i < tmp; i++) {
      rb_put(&rb, rxb[i]);
    }
  }
  return;
}


volatile void UART1_IRQHandler(void) {
  ua1hdl(uart1);
}

/*void write_quads(){
  int i;
  char str[17];
  for(i = 0; i < BUF_MAX; i += 4){
    sprintf(str, "%3d|%3d|%3d|%3d\n\r", buf[i], buf[i+1], buf[i+2], buf[i+3]);
    write_usb_serial_blocking(str, 17);
  }
}
*/

/*
void m1(void){
  lcd_clear();
  lcd_write_str("Begin.\0", 0, 0, 6);
}*/
void M2()
{
	int n = 0;
	int i = 0;
	int cnt=0;
	int count=0;
	int buf[8] = {0,255,255,255};
	uint8_t temp; 
	uint8_t arr[5];
 	uint8_t str[6];
	char strs[5][4];
	
	for(;;) 
	{
		while(rb_is_empty(&rb));
		temp=rb_get(&rb);
	    	sprintf(str, "%03d ", temp);
		arr[count]=temp;
   		if(uart_break_flag)
		{
      			write_usb_serial_blocking("\n\r", 2);
      			n++;
    		}
    		write_usb_serial_blocking(str , 6);
		strcpy(strs[count], str);
		count++;
		if(count == 5)
		{
			lcd_init();
			lcd_write_byte(strs[1]);
			pos+=sizeof(strs[1]);
			lcd_write_byte(strs[2]);
			pos+=sizeof(strs[2]);
			lcd_write_byte(strs[3]);
			pos+=sizeof(strs[3]);
			lcd_write_byte(strs[4]);
			for(i=0;i<5;i++){ strcpy(strs[i], "00"); }
			pos=0;
			count=0;
		}
		// Add function to exit for loop if * pressed
	}
}

void lcd_write_byte(char str[])
{
	lcd_write_str(str,pos,0,4);
}

void main () 
{

  
	write_usb_serial_blocking("Start.\n\r", 8);
	for(;;){
	  static uint8_t txd[1];
	  while(rb_is_empty(&rb)){}
	  txd[0] = rb_get(&rb);
	  write_usb_serial_blocking(txd, 1);
	}
	M2();
	lcd_init();
	lcd_write_str("SUCCESS",0,0,7);	



	
	/*int n = 0;
	int i = 0;
	int cnt=0;
	int count=0;
	int buf[8] = {0,255,255,255};
	uint8_t temp; 
	uint8_t arr[5];
 	uint8_t str[6];
	char strs[5][4];*/
	/*for(;;) 
	{
		
	    	while(rb_is_empty(&rb));
		temp=rb_get(&rb);
	    	sprintf(str, "%03d | ", temp);
		arr[count]=temp;
	    	write_usb_serial_blocking(str , 6);
	    	count++;*/
		/*if(count==6)
		{
			lcd_write_quads(arr);
			count=0;
		}*/
	    	/*if(cnt == 6) 
		{
		      	cnt = 0;
		      	write_usb_serial_blocking(".\n\r", 3);	
	    	}
 	}*/
	/*for(;;) 
	{
		while(rb_is_empty(&rb));
		temp=rb_get(&rb);
	    	sprintf(str, "%03d ", temp);
		arr[count]=temp;*/
    		/*while(rb_is_empty(&rb));
   		sprintf(str, "%03d | ", rb_get(&rb));*/
   		/*if(uart_break_flag)
		{
      			write_usb_serial_blocking("\n\r", 2);
      			n++;
    		}*/
		/*if(count==6)
		{
			lcd_write_quads(arr);
			count=0;
		}*/
   		/*//if(n == 10){ return;}
    		write_usb_serial_blocking(str , 6);
		strcpy(strs[count], str);
		count++;
		if(count == 5)
		{
			lcd_init();
			lcd_write_byte(strs[1]);
			pos+=sizeof(strs[1]);
			lcd_write_byte(strs[2]);
			pos+=sizeof(strs[2]);
			lcd_write_byte(strs[3]);
			pos+=sizeof(strs[3]);
			lcd_write_byte(strs[4]);
			for(i=0;i<5;i++){ strcpy(strs[i], "00"); }
			pos=0;
			count=0;
		}
	}*/
	/*pos=1;
	lcd_write_str(strs[1],pos,0,4);
	wait(2);
	pos+=sizeof(strs[1])-1;
	//lcd_init();
	lcd_write_str(strs[2],pos,0,4);
	wait(2);
	pos+=sizeof(strs[2]);
	//lcd_init();
	lcd_write_str(strs[3],pos,0,4);
	wait(2);
	pos+=sizeof(strs[3]);
	//lcd_init();
	lcd_write_str(strs[4],pos,0,4);*/
}




