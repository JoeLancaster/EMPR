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

#define RB_MAX 1024
ring_buf_t rb;
int pos;
uint8_t uart_break_flag;
#define ua1 ((LPC_UART1_TypeDef *)LPC_UART1)


__attribute__((constructor)) static void init() {
  rb_init(&rb, RB_MAX);
  serial_init1();
  uart1_init();
  uart_break_flag = 0;


  NVIC_EnableIRQ(UART1_IRQn);
  UART_IntConfig(LPC_UART1, UART_INTCFG_RBR, ENABLE);
  UART_IntConfig(LPC_UART1, UART_INTCFG_RLS, ENABLE);
  //UART_IntConfig(LPC_UART1, UART, ENABLE);
  lcd_init();
  lcd_clear();
}

void uart1_hndl(void){
  uint8_t ls = UART_GetLineStatus(ua1);
  uint32_t iid = UART_GetIntId(ua1);
  uart_break_flag =  ls & UART_LINESTAT_BI;
  static uint8_t buf[1];
  if(ls & UART_LINESTAT_RDR)/* || (iid & UART_IIR_INTID_RDA) || (iid & UART_IIR_INTID_CTI))*/{
    while(UART_CheckBusy(ua1) == SET){}
    if(rb_is_full(&rb)) {
      write_usb_serial_blocking("RB FULL\n\r", 9);
      return;
    }
    int tmp = UART_Receive(ua1, buf, 1 , NONE_BLOCKING);
    if(tmp == 0){
      return;
    }
    int i;
    for(i = 0; i < tmp; i++) {
      rb_put(&rb, buf[i]);
    }
  }
}

#define RXB_LEN 16

void uart1_hdl(void){
  static uint8_t rxb[RXB_LEN];
  uint8_t linestat = UART_GetLineStatus(ua1);
  uint32_t iid = UART_GetIntId(ua1);
  uint8_t received = 0;

  uart_break_flag = linestat & UART_LINESTAT_BI;
  
    

  /*    if(uart_break_flag){ //break detected
      return;
      }*/
  /*else*/ if (linestat & UART_LINESTAT_RDR) {
      received = 1;
    }

      
  
  if ( (iid & UART_IIR_INTID_RDA) || (iid & UART_IIR_INTID_CTI) || received){
    //while(UART_CheckBusy(ua1) == SET){}
    uint32_t rc = UART_Receive(ua1, rxb, RXB_LEN, NONE_BLOCKING);
    if(rc < 1){
      return;
    }
    int i;
    for(i = 0; i < rc; i++){
      rb_put(&rb, rxb[i]);
    }
  }
}

#define REL

volatile void UART1_IRQHandler(void) {
  #ifdef UADBG
  uint8_t linestat = UART_GetLineStatus(ua1);
  uint32_t iid = UART_GetIntId(ua1);
  if(linestat & UART_LINESTAT_BI){
    write_usb_serial_blocking("LS:BI\n\r", 7);
  }
  if(linestat & UART_LINESTAT_RDR) {
    write_usb_serial_blocking("LS:RDR\n\r", 8);
    UART_ReceiveByte(ua1);
  }
  if(linestat & UART_LINESTAT_RXFE) {
    uint8_t n = UART_ReceiveByte(ua1);
    uint8_t str[13];
    sprintf(str, "LS:RXFE:%03d\n\r", n);
    write_usb_serial_blocking(str, 13);
  }
  
  #endif //UADBG
  #ifdef REL
  static uint8_t rxb[1];
  uint8_t linestat = UART_GetLineStatus(ua1);
  uint32_t iid = UART_GetIntId(ua1);
  uint8_t received = 0;
  uart_break_flag = linestat & UART_LINESTAT_BI;
  if(linestat & UART_LINESTAT_RXFE){
    UART_ReceiveByte(ua1);
  }
  received = UART_Receive(ua1, rxb, 1, NONE_BLOCKING);
  if(received < 1){return;}
  uint8_t str[4];
  sprintf(str, "%03d ", rxb[0]);
  write_usb_serial_blocking(str, 4);
  #endif
  //uart1_hndl();
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
 	//uint8_t str[6];
	char strs[5][4];
	
	static uint8_t rxb[1];
 	uint8_t linestat = UART_GetLineStatus(ua1);
  	uint32_t iid = UART_GetIntId(ua1);
  	uint8_t received = 0;
  	uart_break_flag = linestat & UART_LINESTAT_BI;
  	if(linestat & UART_LINESTAT_RXFE){
  	  UART_ReceiveByte(ua1);
  	}
	/*for(;;)
	{
  		received = UART_Receive(ua1, rxb, 1, NONE_BLOCKING);
  		if(received < 1){return;}
  		uint8_t str[4];
		temp=rxb[0];
  		sprintf(str, "%03d ", temp);
  		write_usb_serial_blocking(str, 4);
	}*/
	/*for(;;) 
	{
		//while(rb_is_empty(&rb));
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
		if(count == 4)
		{
			lcd_init();
			lcd_write_byte(strs[0]);
			pos+=sizeof(strs[0]);
			lcd_write_byte(strs[1]);
			pos+=sizeof(strs[1]);
			lcd_write_byte(strs[2]);
			pos+=sizeof(strs[2]);
			lcd_write_byte(strs[3]);
			for(i=0;i<5;i++){ strcpy(strs[i], "00"); }
			pos=0;
			count=0;
		}
		// Add function to exit for loop if * pressed
	}*/
}

void lcd_write_byte(char str[])
{
	lcd_write_str(str,pos,0,4);
}

void main () 
{
	write_usb_serial_blocking("Start.\n\r", 8);
	lcd_init();
	lcd_write_str("START",0,0,6);	
	wait(1);
	//M2();
	lcd_init();
	lcd_write_str("SUCCESS",0,0,8);	
		
	int b = 0;
  	uint8_t str[6];
  	write_usb_serial_blocking("Start.\n\r", 8);
  	for(;;) {
    		while(rb_is_empty(&rb)) {}
    		if(uart_break_flag){
     	 	write_usb_serial_blocking("\n\r", 2);

    		}   
   		sprintf(str, "%03d | ", rb_get(&rb));
   		write_usb_serial_blocking(str , 6);    
        }
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




