#include "uart1.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_uart.h"


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
