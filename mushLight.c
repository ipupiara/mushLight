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

#include "triacPID"


#define usartBufferSize  200
uint8_t   putInBP;
uint8_t   takeOutBP;
uint8_t	  usartBuffer[usartBufferSize];


uint8_t   nextBP(uint8_t inp)
{
	uint8_t next = inp + 1;
	if (inp > usartBufferSize -1) {
		next = 0;
	}
	return next;
}


int8_t usartDataRegEmpty()
{
	int8_t res = 0;
	return res;
}

void putCharToUSARTDataReg(int8_t ch)
{
	UDR0 = ch;
}

void disableDataRegEmptyInterrupt()
{
	UCSR0B &=  !(1<<UDRIE0);
}

void enableDataRegEmptyInterrupt ()
{
	UCSR0B |=  (1<<UDRIE0);
}

int8_t  addCharToBuffer(char ch)
{
	int8_t needStart = 0; 
	int8_t res;

	cli();   // critical section !
 	if (takeOutBP == putInBP) {
		needStart = usartDataRegEmpty(); 
	} 
	if ( nextBP(putInBP) !=  takeOutBP ) {   
		putInBP = nextBP (putInBP);   
		res = 1;
		usartBuffer [putInBP]   = ch;
	}	else {
		res= 0;   
	}
	if (needStart  )  {
		enableDataRegEmptyInterrupt();
		takeOutBP = nextBP(takeOutBP);
		putCharToUSARTDataReg(usartBuffer[takeOutBP]);		
	} 	
	sei();
	return res;
}


ISR(USART_UDRE_vect)
{
	//  if highly timecritical methods can be made inline
	//  or even better changed to a #define 
	cli();
	if (usartDataRegEmpty())
	{
		if (takeOutBP != putInBP)  {
			takeOutBP = nextBP(takeOutBP);
			putCharToUSARTDataReg(usartBuffer[takeOutBP]);
		}   else  {
			disableDataRegEmptyInterrupt();
		}
	}
	sei();
}


void addToUsart(char* st)
{
	int8_t cnt;
	for (cnt = 0; cnt < strlen(st);  ++cnt) {
		addCharToBuffer(st[cnt]);
		//  for  a first test done this way, later we'll create a addString.. method 
		// check behaviour of above operator ++ in disassembled code
	}
}


void startUSART( unsigned int baud)
{
	// Set baud rate 
	
	UBRR0H = (unsigned char)(baud>>8);
	UBRR0L = (unsigned char)baud;

	// disable double speed and multi processor communication 
	UCSR0A =  UCSR0A &  !( (1 << U2X0) | (1<<MPCM0))  ;//  & 0b11111100) ;
	
	// Enable  transmitter  and DataRecEmpty interrupt
	UCSR0B=    (1<<TXEN0) | (1<<UDRIE0);
//	UCSR0B = 0b00011000;  // rx compl intr ena - tx compl intr ena - dreg empty intr ena - rx ena - tx ena - sz2 (size bit 2)  - 9. bit rx - 9. tx

	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);   //  8 bits,no parity,async mode, 1 stop bit
}




void startPWM()
{
	// start pwm
	TCCR0A  = (1<<COM0A1) | (1<<WGM00) | (1<<WGM01);
	OCR0A  = 0x00;     // set duty cycle to 0 until pid regulator rises it
	TIMSK0 = 0x00;      // no interrupt needed
	TCCR0B  = (1<<CS01);   // (clk / 8)  °  (clk/255) approx 4-5 k cycles per sec, start running
	DDRB |= (1<<PB3);		// set PB3 (= OC0A)  as output
}




int16_t lastAmpsADCVal;
#define pidStepDelays  2
int16_t  adcCnt;
int8_t   adcTick = 0;

ISR(ADC_vect)
{
	lastAmpsADCVal = ADC;
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
	
		// Timer 0    used for ADC triggering  in TriacRunning mode

	TCCR1A = 0x00 ;   // 
	OCR1A = 0x03F;  // counter top value, 0xFF means approx 10 ADC measures per sec
	TIMSK1  =  (1 << OCIE1A);
	TCNT1 = 0x0000 ;
	TCCR1B = (1< WGM12) ;   //  CTC, stop timer until ADC configured
	TCCR1C = 0x00;


	//  init ADC

	lastAmpsADCVal = 0;

	ADMUX = (1<<REFS0); //  AVCC as ref.voltage, right adjust, ADC0 input 


	ADCSRB = (1<<ADTS2) | (1<<ADTS0) ; 
	//  timer 1 comp match trigger

	ADCSRA  = (1<<ADEN) | (1<<ADATE) | (1<<ADIE) | (1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2);
	// ena adc, set autotrigger mode, int enable,  128 prescaler

	ADCSRA  = 0b00000111;  // disa ADC, ADATE, ADIE


	// start Timer 0 and hence also ADC
	TCCR1B |=  (1<< CS02)| (1<<CS00);    // set clk / 1024, timer started


	sei();  // start interrupts if not yet started

}



void init()
{
	startUSART(3);   //  115.2 k at 8 MHz
//  startUSART(8);					//   115.2 k at 16 Mhz
	
	startPWM();
	startADC();
}


/*
*
*
*		P I D    methods
*
*/





int main(void)
{

	init();
//	startPWM();
    while(1)
    {
		if (adcTick = 1) {
			adcTick = 0;
			calcNextPWMDelay();
		}
    }
}
