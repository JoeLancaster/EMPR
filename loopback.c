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

#include "led.h"

#define RB_MAX 1024
ring_buf_t rb;
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

#define x

#ifdef x
void ua1hdl(void) {
  all_off();
  static const size_t rxb_size = 1;
  uint8_t rxb[rxb_size];
  uint8_t linestat = UART_GetLineStatus(ua1);
  uart_break_flag = linestat & UART_LINESTAT_BI;
  if(linestat & UART_LINESTAT_OE) {
    led_number(0xFF);
  }
  if(linestat & UART_LINESTAT_RXFE) {
    UART_ReceiveByte(ua1); //discard erroneous byte
   }
  if(linestat & UART_LINESTAT_RDR) {
    int tmp = UART_Receive(ua1, rxb, rxb_size, NONE_BLOCKING);

    if(tmp < 1){return;}
    uint8_t str[4];
    sprintf(str, "%03d ", rxb[0]);
    write_usb_serial_blocking(str, 4);

  }
  return;
}

#endif



int c = 0;
volatile void UART1_IRQHandler(void) {
  #ifdef UADBG
  c++;
  uint8_t linestat = UART_GetLineStatus(ua1);
  uint32_t iid = UART_GetIntId(ua1);
  uint8_t cst[64];
  sprintf(cst, "I#: %d\n\r\n\r", c);
  write_usb_serial_blocking(cst, strlen(cst));
  if(linestat & UART_LINESTAT_OE){
    write_usb_serial_blocking("LS:OE\n\r", 7);
  }
  if(linestat & UART_LINESTAT_BI){
    write_usb_serial_blocking("LS:BI\n\r", 7);
  }
  if(linestat & UART_LINESTAT_RXFE) {
    uint8_t n = UART_ReceiveByte(ua1);
    uint8_t str[13];
    sprintf(str, "LS:RXFE:%03d\n\r", n);
    write_usb_serial_blocking(str, 13);
  }
  if(linestat & UART_LINESTAT_RDR) {
    write_usb_serial_blocking("LS:RDR\n\r", 8);
    UART_ReceiveByte(ua1);
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
  ua1hdl();
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

void lcd_write_quads(){
  lcd_clear();
  int i;
  char str[16];
  for(i = 0; i < BUF_MAX; i += 4){
    sprintf(str, "%03d %03d %03d %03d\0", buf[i], buf[i+1], buf[i+2], buf[i+3]);
    lcd_write_str(str, 0, 0, 16);
  }
}
p*/
void m1(void){
  lcd_clear();
  lcd_write_str("Begin.\0", 0, 0, 6);
}

void main () {
  int b = 0;
  uint8_t str[6];
  write_usb_serial_blocking("Start.\n\r", 8);
  for(;;) {
    //    while(rb_is_empty(&rb)) {}
    if(uart_break_flag){
      write_usb_serial_blocking("\n\r", 2);
    }   
    //sprintf(str, "%03d | ", rb_get(&rb));
    //write_usb_serial_blocking(str , 6);    
  }
}
