#include "uart1.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_uart.h"
#include <stdint.h>
#include <stdlib.h>

extern uint8_t break_flag;

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
  UART_Init(LPC_UART1, &u);
  UART_TxCmd(LPC_UART1, ENABLE);

  UART_FIFO_CFG_Type FIcfg;
  UART_FIFOConfigStructInit(&FIcfg);
  UART_FIFOConfig(LPC_UART1, &FIcfg);
}

void dmx_write(int red, int green, int blue) {
 const size_t PACKET_SIZE = 4;
 uint8_t packet[PACKET_SIZE];
 packet[0] = 0xFF; //Mark After Break
 packet[1] = red;
 packet[2] = green;
 packet[3] = blue;
 break_flag = 1;
 UART_ForceBreak(LPC_UART1);
 write_uart1(packet, 4);
}
