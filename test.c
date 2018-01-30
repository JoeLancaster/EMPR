#include "helpers.h"
#include <string.h>
#include "uart1.h"


int main(void) 
{
	uart1_init();
	//serial_init1();
	char buf[4];
	char buff[3];
	memset(buff, 'A', 3);
	buf[0]=0x00;
	buf[1]=0;
	buf[2]=255;
	buf[3]=0;
	/*buf[1]=255;
	buf[2]=0;
	buf[3]=255;*/
	int i;
	/*for(i = 0; i < 10000; i++){
		//UART_ForceBreak(LPC_UART1);
		write_uart1(buf + 1, 3);
	}*/	
	//UART_ForceBreak(LPC_UART1);
	//write_uart1(buf, 3);
	//write_uart1(buf + 1, 3);
	int count=0;
	while(1) 
	{
		
		write_uart1(buf, 4);
		//read_uart1(buff, 3);
		//write_usb_serial_blocking("done", 4);
		//count++;
		
		
	}
}
	
	
