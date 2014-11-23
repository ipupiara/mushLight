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
		needStart = usartDataRegEmpty();  //
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
		putCharToUSARTDataReg(usartBuffer[takeOutBP]);
		takeOutBP = next(takeOutBP);
	} 	
	sei();
	return res;
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


ISR(usart)
{
	//  if highly timecritical methods can be made inline
	//  or even better changed to a #define 
	cli();
	if (usartDataRegEmpty())
	{
		if (takeOutBP != putInBP)  {
			putCharToUSARTDataReg(usartBuffer[takeOutBP]);
			takeOutBP = next(takeOutBP);
		}   else  {
			disableDataRegEmptyInterrupt();
		}
	}
	sei();
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
	// start pwm on value 0
}




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


void startADC()
{
	//
}



void init()
{
	startUSART(3);   //  115.2 k at 8 MHz
//  startUSART(8);					//   115.2 k at 16 Mhz
	
	startPWM();
	startADC();



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
