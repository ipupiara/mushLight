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
int16_t  t0Count;
int16_t  adcCount;

void startTimer()
{
#ifdef BUZZER
#else	
		// start Timer 1 and hence also ADC
		TCCR1B |=  (1<< CS02)| (1<<CS00);    // set clk / 1024, timer started
#endif		
}

void stopTimer()
{
#ifdef BUZZER	
#else
	// start Timer 1 and hence also ADC
	TCCR1B &=  ~((1<< CS02)| (1<<CS00));    // set clk / 1024, timer started
#endif	
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
	uint8_t   TIFRmask;
	stopTimer();  //
	TIFRmask = ~(1<< OCF1A);   //  for debuging
	TIFR1  &=  TIFRmask;  // during debugging found out, that it is better to do this
	++t1Count;
	ADCSRA  |= (1<<ADSC);   // start one ADC cycle
}


ISR(TIMER0_COMPA_vect)
{    // needed for ADC so far..
	uint8_t   TIFRmask;
	stopTimer();  //
	TIFRmask = ~(1<< OCF1A);   //  for debuging
	TIFR1  &=  TIFRmask;  // during debugging found out, that it is better to do this
	++t1Count;
	ADCSRA  |= (1<<ADSC);   // start one ADC cycle
}


void i


void initADC()
{
	lastADCVal = 0;
	adcTick = 0;
	adcCount = 0;

	ADMUX = (1<<REFS0); //  AVCC as ref.voltage, right adjust, ADC0 input
	DIDR0 = (1<<ADC0D);  // disable digital input

	ADCSRB = 0x00 ;

	ADCSRA  = (1<<ADEN) | (1<<ADIE) | (1<<ADPS1) | (1<<ADPS2);  // | (1<<ADPS0)  now 64 prescaler wihout ps0
	// ena adc, set single conversion mode, int enable,  128 prescaler

	//	ADCSRA  = 0b00000111;  // disa ADC, ADATE, ADIE
	
}


void startADC()
{
	// Timer 1    prepare  for ADC triggering  in TriacRunning mode

	t1Count  = 0;

	TCCR1A = 0x00 ;   // 
	OCR1B = 0x02FF;  // counter top value, 0x2FF means approx 2-3 ADC measures per sec
	TIMSK1  = (1<<OCIE1A);  //  interrupt needed for ADC trigger
	TCNT1 = 0x0000 ;
	TCCR1B = (1<< WGM12) ;   //  CTC, stop timer until ADC configured
	TCCR1C = 0x00;


	//  init ADC
	initADC();

	// start Timer 1 and hence also ADC
	TCCR1B |=  (1<< CS02)| (1<<CS00);    // set clk / 1024, timer started
	sei();
}



void startBuzzerADC()
{
	t0Count = 0;
	
	TCCR0A = 0x00 ;   //
	OCR0B = 0x02FF;  // counter top value, 0x2FF means approx 2-3 ADC measures per sec
	TIMSK0  = (1<<OCIE0A);  //  interrupt needed for ADC trigger
	TCNT0 = 0x00 ;
	TCCR0B = (1<< WGM01) ;   //  CTC, stop timer until ADC configured

	//  init ADC
	initADC();

	// start Timer 1 and hence also ADC
	TCCR0B |=  (1<< CS02)| (1<<CS00);    // set clk / 1024, timer started
	sei();
}