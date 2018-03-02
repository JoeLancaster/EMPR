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

//#include "led.h"

#define uart1 ((LPC_UART1_TypeDef *)LPC_UART1)
#define RB_MAX 1024
ring_buf_t rb;
uint8_t uart_break_flag;
int pos=0;
int state=0xFF;
int last_state=0xFF;
#define uart1 ((LPC_UART1_TypeDef *)LPC_UART1)


__attribute__((constructor)) static void init() {
  rb_init(&rb, RB_MAX);
  serial_init1();
  uart1_init();
  uart_break_flag = 0;


  NVIC_EnableIRQ(UART1_IRQn);
  //NVIC_EnableIRQ(UART0_IRQn);
  UART_IntConfig(LPC_UART1, UART_INTCFG_RBR, ENABLE);
  UART_IntConfig(LPC_UART1, UART_INTCFG_RLS, ENABLE);
  //UART_IntConfig(LPC_UART0, UART_INTCFG_RBR, ENABLE);
  //UART_IntConfig(LPC_UART0, UART_INTCFG_RLS, ENABLE);
  lcd_init();
  lcd_clear();
}

void ua1hdl(LPC_UART_TypeDef * ua1) {
  //all_off();
  static const size_t rxb_size = 14;
  uint8_t rxb[rxb_size];
  uint32_t linestat = UART_GetLineStatus(uart1);
  uart_break_flag |= (linestat & UART_LINESTAT_BI); 
  if(linestat & UART_LINESTAT_OE) {
    //led_number(0xFF);
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

//#define UADBG

int c = 0;
volatile void UART1_IRQHandler(void) {
  #ifdef UADBG
  c++;
  uint8_t linestat = UART_GetLineStatus(uart1);
  uint32_t iid = UART_GetIntId(uart1);
  uint8_t cst[64];
  sprintf(cst, "I#: %d\n\r\n\r", c);
  write_usb_serial_blocking(cst, strlen(cst));
  uart_break_flag = linestat & UART_LINESTAT_BI;
  if(linestat & UART_LINESTAT_OE){
    write_usb_serial_blocking("LS:OE\n\r", 7);
  }
  if(uart_break_flag){
    write_usb_serial_blocking("LS:BI\n\r", 7);
  }
  if(linestat & UART_LINESTAT_RXFE) {
    uint8_t n = UART_ReceiveByte(uart1);
    uint8_t str[13];
    sprintf(str, "LS:RXFE:%03d\n\r", n);
    write_usb_serial_blocking(str, 13);
  }
  if(linestat & UART_LINESTAT_RDR) {
    write_usb_serial_blocking("LS:RDR\n\r", 8);
    UART_ReceiveByte(uart1);
  }
  return;
  
  #endif //UADBG

  ua1hdl(uart1);
  //uart1_hndl();
}
volatile void UART0_IRQHandler(void) {
  ua1hdl(LPC_UART0);
  return;
  lcd_init();
  uint8_t rxb[5];
  int j = UART_Receive(LPC_UART0, rxb, 5, NONE_BLOCKING);
  sprintf(rxb, "%03d", j);
  lcd_write_str(rxb, 0, 0, 4);
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
	uint8_t temp; 
	int i = 0;
	int count = 0;
	uint8_t strs[4][4];
 	uint8_t str[4];	
	state=0xFF;
	for(;;)
	{
		state=read_buttons();
		if(state == 0x7E)
		{
			break;
		}
		last_state=state;
		wait(0.01);
		while(rb_is_empty(&rb));
		temp = rb_get(&rb);
	    	sprintf(str, "%03d ", temp);
		write_usb_serial_blocking(str , 4);
		if(count == 0){	lcd_init(); lcd_write_byte(str); pos+=sizeof(str);}
		strcpy(strs[count], str);
		count++;	
		if(count == 4)
		{
			write_usb_serial_blocking("\n\r", 2);
			lcd_write_byte(strs[1]);
			pos+=sizeof(strs[1]);
			lcd_write_byte(strs[2]);
			pos+=sizeof(strs[2]);
			lcd_write_byte(strs[3]);
			for(i=0;i<4;i++){ strcpy(strs[i], "00"); }
			pos=0;
			count=0;
		}
	}
}

void lcd_write_byte(char str[])
{
	lcd_write_str(str,pos,0,4);
}


void m3(int no_packets) {
  write_usb_serial_blocking("M3", 2);
  uint8_t str[16];
  uint8_t infostr[12];
  
  const size_t packet_size = 4;
  const int buf_size  = packet_size * no_packets;
  uint8_t packets[buf_size];
  int i = 0;
  lcd_write_str("Capturing", 1, 0, 9);
  //while(!uart_break_flag) {}
  //while(uart_break_flag) {}
  for(; i < buf_size;) {
    while(rb_is_empty(&rb)) {}
    packets[i++] = rb_get(&rb);
    
  }
  i = 0;
  int state = 0xFF;
  int last_state = state;
  uint8_t ist[5];
  uint8_t x[1];
  lcd_write_str("Ready", 1, 0, 6);
  wait(1);
	while(1)
  	{
  		state = read_buttons();
		if(state==0x7E){ break; }
  		if(keypad_uint8_t_decode(last_state)!=keypad_uint8_t_decode(state) && keypad_uint8_t_decode(state) != 'G')
		{
      			if(state == 0xE7 && state!=last_state) //val for A
			{
				if(i < (buf_size - packet_size))
				{
		  			i += packet_size;
				}
				
			}
      			if(state == 0xEB && state!=last_state) // val for B
			{
				if(i >= packet_size)
				{
	  				i -= packet_size;
				}
			}
		}
		if(last_state!=state)
		{
			last_state=state;
			wait(0.01); 
			sprintf(str, " %03d %03d %03d %03d", packets[i], packets[i+1], packets[i+2], packets[i+3]);
       	 		sprintf(infostr, "Packet # %03d", i/4);
        		lcd_write_str(infostr, 0, 0, 13);
        		lcd_write_str(str,0, 1, 17); //HACK	
		}
  	}
}


void main () 
{
  lcd_write_str("hi", 1, 0, 2);
  m3(6);
  return;



	write_usb_serial_blocking("Start.\n\r", 8);
	lcd_init();
	lcd_write_str("START",0,0,6);	
	wait(1);
	M2();
	lcd_init();
	lcd_write_str("SUCCESS",0,0,8);	
}




