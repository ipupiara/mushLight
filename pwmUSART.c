
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <stdarg.h>

#include "pwmUSART.h"


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

#define  printStrSz  40
char  printStr [printStrSz];

void printfUsart( char *emsg, ...)
{

	va_list ap;

	va_start(ap, emsg);

	vsnprintf(printStr, printStrSz-1,  emsg, ap);
	
	addToUsart(printStr);

	va_end(ap);
	//	printf(emsg, ap);
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

