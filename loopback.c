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


__attribute__((constructor)) static void init() {
  serial_init1();
  uart1_init();
}

void main () {
  write_usb_serial_blocking("Hello", 5);
  //uint8_t txb[10];
  uint8_t rxb[10];
  //memset(txb, 'A', 10);
  memset(rxb, 'B', 10);
  char str[5];
  for(;;){
    //UART_Send(LPC_UART1, txb, 10, NONE_BLOCKING);
    UART_Receive(LPC_UART1, rxb, 10, NONE_BLOCKING);
    int i;
    for(i = 0; i < 10; i++){
      sprintf(str, "%d.\0", rxb[i]);
      write_usb_serial_blocking(str, 5);
    }
  }
}
