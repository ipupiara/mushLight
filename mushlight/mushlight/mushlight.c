
/*
 * mushLight.c
 *
 * Created: 11/9/2014 12:45:12 PM
 *  Author: ixchel
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <math.h>


#include "pwmPID.h"
#include "pwmUSART.h"
#include "pwmadc.h"
#include "pwmpwm.h"


void init()
{
	#ifndef GAUGE
	startUSART(0x0C);   //  115.2 k at 12 MHz with double clock (U2X0)
	#endif

	#ifdef BUZZER 
		initBuzzer();
		startBuzzerPWM();
		startBuzzerADC();
	#else
	#ifdef GAUGE
		initGaugeTimer();
	#else
		InitPID();	
		startPWM();
		startADC();
	#endif	
	#endif
	
	sei();  // start interrupts if not yet started
}



int main(void)
{
	int16_t cycleCount = 0;
	int16_t periodCount = 0;
	int16_t adcCount = 0;
	init();
	testLamps();
    while(1)
    {
		++ cycleCount;
		if (cycleCount == 0) {
			++ periodCount;
			if ( (periodCount & 0x0F) == 0) {
//				printfUsart("counter of period: %X \n", periodCount);
			}
		}
#ifdef GAUGE
		if (timerTick == 1){
			timerTick = 0;
			nextGaugeTick();
		}
#endif		
		if (adcTick == 1) {
			adcTick = 0;
			++ adcCount;
#ifdef BUZZER  
			calcNextBuzzerFrequency();
#else 		
			calcNextPWMDelay();
#endif			
		}
		#ifdef GAUGE
			enterIdleSleepMode();	
		#endif
    }
}
