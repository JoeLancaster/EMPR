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


void m1(void){
  lcd_clear();
  lcd_write_str("Begin.\0", 0, 0, 6);
}

void main () {
  int b = 0;
  uint8_t str[16];
  write_usb_serial_blocking("Start.\n\r", 8);
  for(;;) {

    while(rb_is_empty(&rb)) {}
    if(uart_break_flag){
      //sprintf(str, "b:%d ", uart_break_flag);
    write_usb_serial_blocking("\n\r", 2);
    uart_break_flag = 0;}

    sprintf(str, "%03d | ", rb_get(&rb));
      write_usb_serial_blocking(str, 6);

  }
}
