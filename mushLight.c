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
	startUSART(3);   //  115.2 k at 8 MHz
//  startUSART(8);					//   115.2 k at 16 Mhz
	
	startPWM();
	startADC();
	sei();  // start interrupts if not yet started
}



int main(void)
{
	InitPID();
	init();
    while(1)
    {
		if (adcTick == 1) {
			adcTick = 0;
			calcNextPWMDelay();
		}
    }
}
