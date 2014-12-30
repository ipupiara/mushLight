#include <avr/io.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/atomic.h>


#include "pwmadc.h"
#include "pwmPID.h"

int8_t  adcCnt;
int16_t  t1Count;
int16_t  adcCount;

void startTimer()
{
		// start Timer 1 and hence also ADC
		TCCR1B |=  (1<< CS02)| (1<<CS00);    // set clk / 1024, timer started
}

void stopTimer()
{
	// start Timer 1 and hence also ADC
	TCCR1B &=  ~(1<< CS02)| (1<<CS00);    // set clk / 1024, timer started
}


ISR(ADC_vect)
{
	lastADCVal = ADC;
	++ adcCnt;
	++ adcCount;

	if (adcCnt == pidStepDelays)  {
		adcCnt = 0;
		adcTick = 1;
	}
	
	startTimer();  // used during debugging
	// call pid and set next correction done asynchronously in main tread
}



ISR(TIMER1_COMPA_vect)
{    // needed for ADC so far..
	++t1Count;
	ADCSRA  |= (1<<ADSC);   // start one ADC cycle
	stopTimer();  // 
}


void startADC()
{
	adcTick = 0;
	adcCount = 0;
	t1Count  = 0;
	
		// Timer 1    prepare  for ADC triggering  in TriacRunning mode

	TCCR1A = 0x00 ;   // 
	OCR1B = 0x02FF;  // counter top value, 0x2FF means approx 2-3 ADC measures per sec
	TIMSK1  = (1<<OCIE1A);  //  interrupt needed for ADC trigger
	TCNT1 = 0x0000 ;
	TCCR1B = (1<< WGM12) ;   //  CTC, stop timer until ADC configured
	TCCR1C = 0x00;


	//  init ADC

	lastADCVal = 0;

	ADMUX = (1<<REFS0); //  AVCC as ref.voltage, right adjust, ADC0 input 
	DIDR0 = (1<<ADC0D);  // disable digital input

	ADCSRB = 0x00 ; 
	//  timer 1 comp match trigger

	ADCSRA  = (1<<ADEN) | (1<<ADIE) | (1<<ADPS1) | (1<<ADPS2);  // | (1<<ADPS0)  now 64 prescaler wihout ps0
	// ena adc, set single conversion mode, int enable,  128 prescaler

//	ADCSRA  = 0b00000111;  // disa ADC, ADATE, ADIE


	// start Timer 1 and hence also ADC
	TCCR1B |=  (1<< CS02)| (1<<CS00);    // set clk / 1024, timer started
	sei();
}

