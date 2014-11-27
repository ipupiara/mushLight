#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "pwmpwm.h"



void startPWM()
{
	// start 
	TCCR0A  = (1<<COM0A1) | (1<<WGM00) | (1<<WGM01);
	OCR0A  = 0x00;     // set duty cycle to 0 until pid regulator rises it
	TIMSK0 = 0x00;      // no interrupt needed
	TCCR0B  = (1<<CS01);   // (clk / 8)  °  (clk/255) approx 4-5 k cycles per sec, start running
	DDRB |= (1<<PB3);		// set PB3 (= OC0A)  as output
}




void setPWMDelay(int16_t newDelay)
{
	if (newDelay > 0xFF) {
		OCR0A = 0xFF;
	} else {
		OCR0A = (int8_t) newDelay;
	}
}

int16_t getPWMDelay()
{
	int16_t res = 0;
	res = (int16_t) OCR0A;
	return res;
}

