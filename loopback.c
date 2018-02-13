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

#define BUF_SIZE 4
#define BUF_MAX BUF_SIZE * 32
uint8_t * buf;
int can_write;
char ** strbuf;
int buf_cnt;

/*int array_cmp(char *xs, char *ys, int size){
  int i;
  for(i = 0; i < size; i++){
    if(xs[i] != ys[i]){
      return 0;
    }
  }
  return 1;
  }*/

__attribute__((constructor)) static void init() {
  buf_cnt = 0;
  strbuf = (char **)malloc(BUF_SIZE);
  int i;
  for(i = 0; i < BUF_SIZE; i++){
    strbuf[i] = (char *)malloc(6);
  }
  buf = malloc(BUF_MAX);
  memset(buf, 'A', BUF_MAX);  
  serial_init1();
  uart1_init();
  //NVIC_EnableIRQ(UART1_IRQn);
  UART_IntConfig(LPC_UART1, UART_INTCFG_RLS, ENABLE);
  lcd_init();
  lcd_clear();
}

void uart1_hndl(void){
  //NVIC_DisableIRQ(UART1_IRQn);
  if(buf_cnt >= BUF_MAX){
    return;
  }
  while(UART_CheckBusy(LPC_UART1) == SET){}
  int tmp = UART_Receive(LPC_UART1, buf + buf_cnt, BUF_SIZE, NONE_BLOCKING);
  if(tmp == 0){
    return;
  }
  buf_cnt += tmp;
  //char str[15];
  //sprintf(str, "buf_cnt: %4d\n\r", buf_cnt);
  //write_usb_serial_blocking(str, 15);
  int i;
  //NVIC_EnableIRQ(UART1_IRQn);
  can_write = 1;
}

volatile void UART1_IRQHandler(void) {
  uart1_hndl();
}


void write_quads(){
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

void m1(void){
  lcd_clear();
  lcd_write_str("Begin.\0", 0, 0, 6);
}

void main () {
  // NVIC_DisableIRQ(UART1_IRQn);
  write_usb_serial_blocking("RESET\n\r", 7);
  const int RXB_SIZE = 8;
  uint8_t rxb[RXB_SIZE];
  memset(rxb, 'B', RXB_SIZE);
  char str[RXB_SIZE][5];
  for(;;){
    if(buf_cnt >= BUF_MAX){
      //lcd_write_quads();
      write_quads();
      buf_cnt = 0;
      //NVIC_EnableIRQ(UART1_IRQn);
      #ifdef SINGLE
      return;
      #endif
    }
    uart1_hndl();
    }
}
