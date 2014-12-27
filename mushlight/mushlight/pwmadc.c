#include <avr/io.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/atomic.h>


#include "pwmadc.h"
#include "pwmPID.h"

int16_t  adcCnt;


ISR(ADC_vect)
{
	lastADCVal = ADC;
	++ adcCnt;

	if (adcCnt == pidStepDelays)  {
		adcCnt = 0;
		adcTick = 1;
	}
	// call pid and set next correction
}



ISR(TIMER1_COMPA_vect)
{    // needed for ADC so far..
}


void startADC()
{
	adcTick = 0;
		// Timer 1    used for ADC triggering  in TriacRunning mode

	TCCR1A = 0x00 ;   // 
	OCR1B = 0x02FF;  // counter top value, 0x2FF means approx 10 ADC measures per sec
//	TIMSK1  =  (1 << OCIE1A);
	TIMSK1  = 0x00;  // no interrupt needed for ADC trigger
	TCNT1 = 0x0000 ;
	TCCR1B = (1< WGM12) ;   //  CTC, stop timer until ADC configured
	TCCR1C = 0x00;


	//  init ADC

	lastADCVal = 0;

	ADMUX = (1<<REFS0); //  AVCC as ref.voltage, right adjust, ADC0 input 
	DIDR0 = (1<<ADC0D);  // disable digital input

	ADCSRB = (1<<ADTS2) | (1<<ADTS0) ; 
	//  timer 1 comp match trigger

	ADCSRA  = (1<<ADEN) | (1<<ADATE) | (1<<ADIE) | (1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2);
	// ena adc, set autotrigger mode, int enable,  128 prescaler

//	ADCSRA  = 0b00000111;  // disa ADC, ADATE, ADIE


	// start Timer 1 and hence also ADC
	TCCR1B |=  (1<< CS02)| (1<<CS00);    // set clk / 1024, timer started
}

