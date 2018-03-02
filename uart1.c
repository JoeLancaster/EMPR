#include "uart1.h"
#include "helpers.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_timer.h"
#include <stdint.h>
#include <stdlib.h>

extern uint8_t break_flag;

/*void TIMER0_IRQHandler(void)
{
        if (TIM_GetIntStatus(LPC_TIM0, TIM_MR0_INT)== SET)
        {
               return;
        }
	LPC_UART0 -> LCR &= ~(UART_LCR_BREAK_EN);
	TIM_Cmd(LPC_TIM0, DISABLE);
        TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
}*/

void uart1_init() {
  PINSEL_CFG_Type p;
  p.Funcnum = PINSEL_FUNC_2;
  p.OpenDrain = PINSEL_PINMODE_OPENDRAIN;
  p.Portnum = PINSEL_PORT_2;
  p.Pinnum = PINSEL_PIN_0;
  p.Pinmode = PINSEL_PINMODE_TRISTATE;
  PINSEL_ConfigPin(&p);
  p.Pinnum = PINSEL_PIN_1;
  PINSEL_ConfigPin(&p);

  UART_CFG_Type u;
  UART_ConfigStructInit(&u);
  u.Baud_rate = 250000;
  u.Parity = UART_PARITY_NONE;
  u.Stopbits = UART_STOPBIT_2;
  UART_Init((LPC_UART_TypeDef *)LPC_UART1, &u);
  UART_TxCmd((LPC_UART_TypeDef *)LPC_UART1, ENABLE);

  UART_FIFO_CFG_Type FIcfg;
  UART_FIFOConfigStructInit(&FIcfg);
  UART_FIFOConfig((LPC_UART_TypeDef *)LPC_UART1, &FIcfg);
}

void dmx_write(int red, int green, int blue) {
 const size_t PACKET_SIZE = 10;
 uint8_t packet[PACKET_SIZE];
 memset(packet,0x00,PACKET_SIZE);
 packet[0] = 0x00; //Start code
 packet[1] = red;
 packet[2] = green;
 packet[3] = blue;
// packet[4] = 0x00;
 TIM_Cmd(LPC_TIM0, ENABLE);
 UART_ForceBreak(LPC_UART1);
 TIM_Cmd(LPC_TIM1, ENABLE);
 while(break_flag == 0);
 break_flag=0;
 write_uart1(packet, PACKET_SIZE);
}
