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
#include "triggers.h"
#include "ui.h"
#include "lcd.h"
#include "keypad.h"
#include "led.h"



#define uart1 ((LPC_UART1_TypeDef *)LPC_UART1)
#define RB_MAX 1024
#define A_NICE_TIME 1.618034
ring_buf_t rb;
uint8_t uart_break_flag;
int pos=0;
int state=0xFF;
int last_state=0xFF;
int m2_can_go;
int systick_bound;
int syscnt;

__attribute__((constructor)) static void init() {
  rb_init(&rb, RB_MAX);
  serial_init1();
  uart1_init();
  uart_break_flag = 0;
  m2_can_go = 1;
  syscnt = 0;
  systick_bound = 0;
  NVIC_EnableIRQ(UART1_IRQn);
  UART_IntConfig(LPC_UART1, UART_INTCFG_RBR, ENABLE);
  UART_IntConfig(LPC_UART1, UART_INTCFG_RLS, ENABLE);
  lcd_init();
  lcd_clear();
  SYSTICK_InternalInit(1);
  SYSTICK_IntCmd(ENABLE);

}
void SysTick_Handler(void) {
  if((++syscnt) >= systick_bound){
    m2_can_go = 1;
    syscnt = 0;
  }
}

void ua1hdl(LPC_UART_TypeDef * ua1) {
  static const size_t rxb_size = 14;
  uint8_t rxb[rxb_size];
  uint32_t linestat = UART_GetLineStatus(uart1);
  uart_break_flag |= (linestat & UART_LINESTAT_BI); 
  if(linestat & UART_LINESTAT_OE) { //overrun error - we can't keep up
    led_number(8);
  }
  if(linestat & UART_LINESTAT_RXFE) { //received an error, but the byte is still on top of FIFO
    UART_ReceiveByte(ua1); //discard erroneous byte
   }
  if(linestat & UART_LINESTAT_RDR) { //data ready
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
#ifdef UADBG
int c = 0;
#endif
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
    led_number(8);
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

//#define mfour

void M2() {
  lcd_init();
  lcd_write_str("Enter rate", 0, 0, 11);
  wait(A_NICE_TIME);
  systick_bound = packets_ps(integer_input());
  lcd_init();
  lcd_write_str("Ready", 0, 0, 6);
  int state = 0xFF;
  const size_t rbh_size = 4;
  uint8_t rb_head_old[rbh_size];
  uint8_t str[17];
  int i;
  lcd_init();
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
  lcd_write_str("Capturing", 0, 0, 10);
  for(; i < buf_size;) {
    while(rb_is_empty(&rb)) {}
    packets[i++] = rb_get(&rb);
    
  }
  i = 0;
  int state = 0xFF;
  int scroll=0; 
  int j=0;
  uint8_t size_i=13;
  uint8_t size_s=17;
  int last_state = state;
  uint8_t ist[5];
  uint8_t x[1];
  lcd_init();
  lcd_write_str("Ready", 1, 0, 6);
  wait(1);
  lcd_init();
	while(1)
  	{
  		state = read_buttons();
		if(state==0x7E){ break; }
		/*if(state==0xDE) // Go to 1st Packet
		{
			i=0; 
			sprintf(str, " %03d %03d %03d %03d", packets[i], packets[i+1], packets[i+2], packets[i+3]);
       	 		sprintf(infostr, "Packet # %03d", (i/packet_size)+1);
			lcd_write_str(infostr, 0, 0, size_i);
        		lcd_write_str(str,0, 1, size_s);	
		}*/	
  		if(keypad_uint8_t_decode(last_state)!=keypad_uint8_t_decode(state) && keypad_uint8_t_decode(state) != 'G')
		{
      			if(state == 0xE7 && state!=last_state) //val for A
			{
				scroll=0;
				j=0;
				if(i < (buf_size - packet_size))
				{
		  			i += packet_size;
				}
				
			}
      			if(state == 0xEB && state!=last_state) // val for B
			{
				scroll=0;
				j=0;
				if(i >= packet_size)
				{
	  				i -= packet_size;
				}
			}
			if(state == 0x77) // val for 1
			{ 
				if(j < packet_size)
				{
					scroll=1;
					j+=1;
				}
				if(j==packet_size)
				{
					scroll=3;
				}
			}
			if(state == 0xB7) // val for 2
			{
				if(j >= 0)
				{ 
					scroll=2;
					j-=1;
				}
				if(j == -1)
				{
					scroll=4;
				}
			}
			if(state==0xDE) // Go to 1st Packet
			{
				scroll=0;
			}	
		}
		if(last_state!=state)
		{
			last_state=state;
			wait(0.01);
			if(scroll == 0)
			{ 
				sprintf(str, " %03d %03d %03d %03d", packets[i], packets[i+1], packets[i+2], packets[i+3]);
       	 			sprintf(infostr, "Packet # %03d", (i/packet_size)+1);
				size_i=13;
			}
			if(scroll == 1)
			{
				sprintf(infostr, "Slot # %03d  ", j);
				sprintf(str, " %03d %03d %03d %03d", packets[i+j], packets[i+(j+1)], packets[i+(j+2)], packets[i+(j+3)]);
			}
			if(scroll == 2)
			{
				sprintf(infostr, "Slot # %03d  ", j);
				sprintf(str, " %03d %03d %03d %03d", packets[i+j], packets[i+(j+1)], packets[i+(j+2)], packets[i+(j+3)]);
			}
			if(scroll == 3)
			{
				sprintf(str,"End of Packet! ");
			}
			if(scroll == 4)
			{
				sprintf(str,"Can't go further!");
				scroll=0;
				j=0;
			}
        		lcd_write_str(infostr, 0, 0, size_i);
        		lcd_write_str(str,0, 1, size_s);	
		}
  	}
}

void M4(){
  uart_break_flag = 0;
  trigger t;
  lcd_init();
  lcd_write_str("Choose condition.", 0, 0, 17);
  wait(A_NICE_TIME);
  lcd_init();
  lcd_write_str("1:LT 2:GT 3:GE", 0, 0, 15);
  lcd_write_str("4:LE 5:EQ 6:NEQ", 0, 1, 16);
  uint8_t v = keypad_uint8_t_decode(keypad_get_single());
  if(v < '1' || v > '6'){
    lcd_init();
    lcd_write_str("Incorrect", 0, 0, 10);
  }
  else {
    switch(v){
      case '1':
	t.condition = LT;
	break;
      case '2':
	t.condition = GT;
	break;
      case '3':
	t.condition = GE;
	break;
      case '4':
	t.condition = LE;
	break;
      case '5':
	t.condition = EQ;
	break;
      case '6':
	t.condition = NEQ;
	break;
    }
  }
  lcd_init();
  lcd_write_str("Choose channel.", 0, 0, 16);
  wait(A_NICE_TIME);
  lcd_init();
  lcd_write_str("1:R 2:G 3:B", 0, 0, 12);
  v = keypad_uint8_t_decode(keypad_get_single());
  switch(v){
    case '1':
      t.channel = RED;
      break;
    case '2':
      t.channel = GREEN;
      break;
    case '3':
      t.channel = BLUE;
      break;
  }
  lcd_init();
  lcd_write_str("Enter value", 0, 0, 12);
  wait(A_NICE_TIME);
  t.val = integer_input();
  lcd_init();
  lcd_write_str("Waiting", 0, 0, 8);
  const size_t rb_size = 4;
  uint8_t rxb[rb_size];
  int i;
  int caught = 0;
  int packet_no = 0;
  uint8_t str[17];
  uart_break_flag = 0;
  do {
    state = read_buttons();
    if(uart_break_flag) {
      uart_break_flag = 0;
      for(i = 0; i < rb_size; i++) {
	while(rb_is_empty(&rb));
	rxb[i] = rb_get(&rb);
      }
      sprintf(str, "%03d %03d %03d %03d", rxb[0], rxb[1], rxb[2], rxb[3]);
      write_usb_serial_blocking(str, 17);
      caught = trigger_eval(&t, rxb);
      packet_no++;
    }
  } while((keypad_uint8_t_decode(state) != '#') && !caught);
  if(caught) {

    sprintf(str, "%03d %03d %03d %03d", rxb[0], rxb[1], rxb[2], rxb[3]);
    lcd_init();
    lcd_write_str(str, 0, 0, 16);
    sprintf(str, "Packet #: %06d", packet_no);
    lcd_write_str(str, 0, 1, 17);
  } else {
    lcd_init();
    lcd_write_str("User cancelled.", 0, 0, 15);
  }
  while(keypad_uint8_t_decode(read_buttons()) != '*');
}



void main () {
   M3(4);
  /*for(;;){
    lcd_init();
    lcd_write_str("1:M2 2:M3 3:M4", 0, 0, 15);
    uint8_t choice = keypad_uint8_t_decode(keypad_get_single());
    switch(choice) {
    case '1':
      M2();
      break;
    case '2':
      lcd_init();
      lcd_write_str("No. packets", 0, 0, 12);
      wait(A_NICE_TIME);
      M3(integer_input());
      break;
    case '3':
      M4();
      break;
    }
  }*/
}
