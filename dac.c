#include "helpers.h"

//#define _ADC_CHANNEL ADC_CHANNEL_2
#define PI 3.14159
void play_sound(int time)
{
	serial_init1();
	uint16_t result;
	char str[32];
	myDAC_init();
	//float time=0;
	float step=0.001;
	uint32_t time_period;
        float rate =1;
	double DAC_SECOND = 125000;
	int period = time*DAC_SECOND;
	int i=0;
	while(i<period)
	{
                for(time_period = 0x3FD; time_period < 0x3FF; time_period++)
                {
                        DAC_UpdateValue ( LPC_DAC,(uint32_t)(time_period*rate));
                }

                rate += 0.01;

                if(rate >= 1)
                {
                        rate = 0;
                }
		i++;
		/*time=0x3FF;
		rate=0.5;
		DAC_UpdateValue ( LPC_DAC,(uint32_t)(time*rate));*/
	}
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
