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
#include "lpc17xx_timer.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "helpers.c"
//#include "helpers.h"

//#define _ADC_CHANNEL ADC_CHANNEL_2
#define PI 3.14159
int main(void)
{
	serial_init1();
	PINSEL_CFG_Type PinCfg;				// Pin configuration for ADC
	/*
	 * Initialize ADC pin connect
	 */
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Funcnum = 2;
	PinCfg.Pinnum = 26;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Funcnum = 2;
	PinCfg.Pinnum = 24;
	PINSEL_ConfigPin(&PinCfg);
	
	ADC_Init(LPC_ADC, 200000);
	ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_1, ENABLE);
	uint16_t result;
	char str[32];
	ADC_ChannelGetStatus(LPC_ADC, 1, 0);
	DAC_Init(LPC_DAC);
	//float time=0;
	float step=0.001;
	uint32_t time;
        float rate =1;
	while (1)
        {
                for(time = 1; time < 0x3FF; time++)
                {
                        DAC_UpdateValue ( LPC_DAC,(uint32_t)(time*rate));
                }

                //rate += 0.001;

                if(rate >= 1)
                {
                        rate = 0;
                }
        }

        return 1;
	/*while(1)
	{
		if(time>(2*PI))
		{
			time-=2*PI;
		}
		wait(0.0001);
		DAC_UpdateValue(LPC_DAC, 1500+500*sin(time));
		time+=step;
	}*/
}