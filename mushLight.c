/*
 * mushLight.c
 *
 * Created: 11/9/2014 12:45:12 PM
 *  Author: ixchel
 */ 


#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/atomic.h>




int16_t lastAmpsADCVal;

ISR(ADC_vect)
{
	lastAmpsADCVal = ADC;
//	++ adcCnt;

//	if (adcCnt == pidStepDelays)  {
//		adcCnt = 0;
//		adcTick = 1;
//	}
	// call pid and set next correction
}



ISR(TIMER0_COMPA_vect)
{    // needed for ADC so far..
}


#define usartBufferSize  200
uint8_t   topBP;
uint8_t   bottomBP;

//ISR(usart)
//		cli
//   if bottom != top
//			take char at bottom 
//           inc buttom
//			sti
//			put char into outbuffer
//		
//
//  )

int8_t  addCharToBuffer(char ch)
{
//			cli
// if (next = top) needStart = true
//	if ( next(top) !=  bottom   
//			top = next (top);   res = 1
//			buff [top]   = ch
//      else res= 0
//			sti
//  if needStart 
//				cli
//				set buttom char
//			inc buttom
//				sti
//			put bottom char to outbuffwe
// 
//		return res
}

uint8_t   nextBP(uint8_t inp)
{
	uint8_t next = inp + 1;
	if (inp > usartBufferSize -1) {
		next = 0;
	}
	return next;
}

void addToUsart(char* st)
{
}


void startPWM()
{
	// start pwm on value 0
}

void startADC()
{
	//
}

int8_t usartBuffer[usartBufferSize];

void startUSART()
{
}

void init()
{
	startPWM();



// Timer 0    used for ADC triggering  in TriaRunning mode

TCCR0A = (1< WGM01);   // CTC

OCR0A = 0xFF;  // counter top value, 0xFF means approx 42.18 ADC measures and write to mem per sec

TCNT0 = 0x00 ;

//		TCCR0B = 0b00000101  ; // CTC on CC0A , set clk / 1024, timer started
//		TIMSK0  = 0b00000010;  // ena  interrupts, and let run ADC
// 		not yet start Timer0 and ADC, to be tested
TCCR0B = 0b00000000  ; // CTC on CC0A , not yet started
TIMSK0  = 0b00000000;




	//  init ADC

	ADCSRA  = 0b00000111;  // disa ADC, ADATE, ADIE

	lastAmpsADCVal = 0;

	sei();  // start interrupts if not yet started
	
}



int main(void)
{

	init();
//	startPWM();
    while(1)
    {
        //TODO:: Please write your application code 
    }
}
