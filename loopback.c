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
uint8_t * buf;
int can_write;
char ** strbuf;

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
  strbuf = (char **)malloc(BUF_SIZE);
  int i;
  for(i = 0; i < BUF_SIZE; i++){
    strbuf[i] = (char *)malloc(6);
  }
  buf = malloc(BUF_SIZE);
  memset(buf, 'A', BUF_SIZE);
  
  serial_init1();
  uart1_init();
  //NVIC_EnableIRQ(UART1_IRQn);
  //UART_IntConfig(LPC_UART1, UART_INTCFG_RLS, ENABLE);
  
}

volatile void UART1_IRQHandler(void) {
  NVIC_DisableIRQ(UART1_IRQn);
  UART_Receive(LPC_UART1, buf, BUF_SIZE, NONE_BLOCKING);
  int i;
  for(i = 0; i < BUF_SIZE; i++){
    sprintf(strbuf[i], "%3d.\n\r", buf[i]);
  }
  NVIC_EnableIRQ(UART1_IRQn);
  can_write = 1;
}

void main () {
  write_usb_serial_blocking("Hello", 5);
  //uint8_t txb[10];
  const int RXB_SIZE = 8;
  uint8_t rxb[RXB_SIZE];
  uint8_t rxb_cp[RXB_SIZE];
  //memset(txb, 'A', 10);

  memset(rxb, 'B', RXB_SIZE);
  char str[RXB_SIZE][5];
  char str1[5];
  for(;;){
    memcpy(rxb_cp, rxb, RXB_SIZE);
    /*if(can_write == 1){
      NVIC_DisableIRQ(UART1_IRQn);
      int i;
      for(i = 0; i < BUF_SIZE; i++){
	write_usb_serial_blocking(strbuf[i], 6);
      }
      can_write = 0;
      NVIC_EnableIRQ(UART1_IRQn);
      }*/
    /* UART_Receive(LPC_UART1, rxb, RXB_SIZE, NONE_BLOCKING);
    int i;
    for(i = 0; i < RXB_SIZE; i++){
      sprintf(str[i], "%3d.\n\r", rxb[i]);
      write_usb_serial_blocking(str[i], 6);
      }*/
    UART_Receive(LPC_UART1, rxb, RXB_SIZE, NONE_BLOCKING);
    if(memcmp(rxb, rxb_cp, RXB_SIZE) != 0){
      sprintf(str, "%3d|%3d|%3d|%3d\n\r", rxb[0], rxb[1], rxb[2], rxb[3]);
      write_usb_serial_blocking(str, 17);
    }
  }
}
