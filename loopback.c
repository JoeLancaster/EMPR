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
#include <stdlib.h>
#include <string.h>
#include "helpers.h"
#include "ring_buffer.h"

#define RB_MAX 256
ring_buf_t rb;

__attribute__((constructor)) static void init() {
  rb_init(&rb, RB_MAX);
  serial_init1();
  uart1_init();
  NVIC_EnableIRQ(UART1_IRQn);
  UART_IntConfig(LPC_UART1, UART_INTCFG_RBR, ENABLE);
  lcd_init();
  lcd_clear();
}

void uart1_hndl(void){
  static uint8_t buf[4];
  while(UART_CheckBusy(LPC_UART1) == SET){}
  if(rb_is_full(&rb)) {
    write_usb_serial_blocking("RB FULL\n\r", 9);
    return;
  }
  int tmp = UART_Receive(LPC_UART1, buf, 4, NONE_BLOCKING);
  if(tmp == 0){
    return;
  }
  int i;
  for(i = 0; i < tmp; i++) {
    rb_put(&rb, buf[i]);
  }
}

volatile void UART1_IRQHandler(void) {
  uart1_hndl();
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
*/
void m1(void){
  lcd_clear();
  lcd_write_str("Begin.\0", 0, 0, 6);
}

void main () {
  uint8_t str[4];
  for(;;) {
    while(rb_is_empty(&rb)) {}
    sprintf(str, "%03d ", rb_get(&rb));
    write_usb_serial_blocking(str, 4);
  }
}
