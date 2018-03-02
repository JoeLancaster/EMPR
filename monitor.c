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
int m2_can_go;

int syscnt;

__attribute__((constructor)) static void init() {
  rb_init(&rb, RB_MAX);
  serial_init1();
  uart1_init();
  uart_break_flag = 0;
  m2_can_go = 1;
  syscnt = 0;
  NVIC_EnableIRQ(UART1_IRQn);
  //NVIC_EnableIRQ(UART0_IRQn);
  UART_IntConfig(LPC_UART1, UART_INTCFG_RBR, ENABLE);
  UART_IntConfig(LPC_UART1, UART_INTCFG_RLS, ENABLE);
  //UART_IntConfig(LPC_UART0, UART_INTCFG_RBR, ENABLE);
  //UART_IntConfig(LPC_UART0, UART_INTCFG_RLS, ENABLE);
  lcd_init();
  lcd_clear();
  SYSTICK_InternalInit(1);
  SYSTICK_IntCmd(ENABLE);
}

void SysTick_Handler(void) {

  if((++syscnt) >= 250){
    m2_can_go = 1;
    syscnt = 0;
  }


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

void m2()
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

int arcmp(uint8_t * ar1, uint8_t * ar2, size_t size){
  int r = 0xFF;
  int i;
  for(i = 0; i < size; i++){
    r &= (ar1[i] == ar2[i]);
    if(!r){return r;}
  }
  return r;
}


void M2() {
  int state = 0xFF;
  const size_t rbh_size = 4;
  uint8_t rb_head_old[rbh_size];
  uint8_t str[17];
  int i;
  SYSTICK_Cmd(ENABLE);
  do {
    uint8_t rb_head[rbh_size];
    if(uart_break_flag){
      uart_break_flag = 0;
      for(i = 0; i < rbh_size; i++) {
	while(rb_is_empty(&rb));
	rb_head[i] = rb_get(&rb);
      }
      if(!arcmp(rb_head, rb_head_old, rbh_size) && m2_can_go){
	sprintf(str, " %03d %03d %03d %03d", rb_head[0], rb_head[1], rb_head[2], rb_head[3]);
	lcd_init();
	lcd_write_str(str, 0, 0, 17);
	m2_can_go = 0;
      }
      memcpy(rb_head_old, rb_head, rbh_size);}
  } while(keypad_uint8_t_decode(state) != '#');
}

void lcd_write_byte(char str[])
{
	lcd_write_str(str,pos,0,4);
}


void M3(int no_packets) {
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
  int scroll=0; 
  int j=0;
  int last_state = state;
  uint8_t ist[5];
  uint8_t x[1];
  lcd_write_str("Ready", 1, 0, 6);
  wait(1);
  lcd_init();
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
					scroll=0;
		  			i += packet_size;
				}
				
			}
      			if(state == 0xEB && state!=last_state) // val for B
			{
				if(i >= packet_size)
				{
					scroll=0;
	  				i -= packet_size;
				}
			}
			if(state == 0x77) // val for 1
			{ 
				if(j < packet_size)
				{
					scroll=1;
					j+=packet_size;
				}
			}
			/*if(state == 0xB7) // val for 2
			{
				if(j >= 0)
				{ 
					scroll=2;
					j-=packet_size;
				}
			}*/
		}
		if(last_state!=state)
		{
			last_state=state;
			wait(0.01);
			if(scroll == 0)
			{ 
				sprintf(str, " %03d %03d %03d %03d", packets[i], packets[i+1], packets[i+2], packets[i+3]);
       	 			sprintf(infostr, "Packet # %03d", (i/4)+1);
			}
			if(scroll == 1)
			{
				sprintf(str, " %03d %03d %03d %03d", packets[i+(j-3)], packets[i+(j-2)], packets[i+(j-1)], packets[i+j]);
			}
        		lcd_write_str(infostr, 0, 0, 13);
        		lcd_write_str(str,0, 1, 17); //HACK	
		}
  	}
}


void main () 
{
  	lcd_write_str("hi", 1, 0, 3);
  	
	M3(6);
  	return;



	write_usb_serial_blocking("Start.\n\r", 8);
	lcd_init();
	lcd_write_str("START",0,0,6);	
	wait(1);
	M2();
	lcd_init();
	lcd_write_str("SUCCESS",0,0,8);	
}




