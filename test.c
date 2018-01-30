#include "helpers.h"
#include <string.h>
#include "uart1.h"
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
#include "uart1.h"

uint8_t break_flag = 0;
/*
void TIMER0_IRQHandler(void){
  if(break_flag == 0){
    return;
  }
  LPC_UART1 -> LCR &= ~(UART_LCR_BREAK_EN);
}
*/
void SysTick_Handler(void){
  SYSTICK_ClearCounterFlag();
  if(break_flag == 0){
    return;
  }
  if(break_flag == 1){
    break_flag++;
  }
  if(break_flag == 2){
    LPC_UART1 -> LCR &= ~(UART_LCR_BREAK_EN);
    break_flag = 0;
  }
}

__attribute__((constructor)) static void init() {
  serial_init1();
  uart1_init();
  SYSTICK_InternalInit(1);
  SYSTICK_IntCmd(ENABLE);
  SYSTICK_Cmd(ENABLE);
  //NVIC_SetPriority(TIMER0_IRQn, 1);
  //NVIC_EnableIRQ(TIMER0_IRQn);
}

int main(void) 
{
	//serial_init1();
	dmx_write(0, 0 , 0);
}
	
	
