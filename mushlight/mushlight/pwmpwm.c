#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "pwmpwm.h"
#include "pwmadc.h"



void startPWM()
{
	// start 
	TCCR0A  = (1<<COM0A1) | (1<<WGM00) | (1<<WGM01);          // fast pwm on oc0a  with top = ocr0a
	OCR0A  = 0x00;     // set duty cycle to 0 until pid regulator rises it
	TIMSK0 = 0x00;      // no interrupt needed
	DDRD |= (1<<PD6);		// set PB3 (= OC0A)  as output
	TCCR0B  = (1<<CS01); //  ? | (1<<WGM02); ?  // (clk / 8)   approx 4-5 k cycles per sec, fast pwm, start running
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


void startBuzzerPWM()
{
	// start
	TCCR0A  = (1<<COM0A1) | (1<<WGM00) | (1<<WGM01);          // fast pwm on oc0a  with top = ocr0a
	OCR0A  = 0x00;     // set duty cycle to 0 until pid regulator rises it
	TIMSK0 = 0x00;      // no interrupt needed
	DDRD |= (1<<PD6);		// set PB3 (= OC0A)  as output
	TCCR0B  = (1<<CS01); //  ? | (1<<WGM02); ?  // (clk / 8)   approx 4-5 k cycles per sec, fast pwm, start running
	
}


void setBuzzerPWMWidth(int16_t  newWidth)
{
	if (newWidth > 0xFFFF) {
		OCR0A = 0xFF;
		} else {
		OCR0A = (int8_t) newWidth;
	}	
}

int16_t getBuzzerPWMWidth()
{
	int16_t res = 0;
	res = (int16_t) OCR0A;
	return res;	
}




//////////  Gauge methods

#define amtLights  6      // max 8


enum directions {
			upward = 1,
			downward
};

int8_t stepsAmtLeft;
int8_t direction;
int8_t actualLight;

int8_t  lampTranslate [amtLights + 1] = {0,6,5,4,3,7,8};

void switchOnLamp(int8_t lamp)
{
	int8_t  la;
	la = lampTranslate[lamp];
	la = la -1;
	int8_t msk = (1 << la);
	PORTD |= msk;
}

void switchOffLamp(int8_t lamp)
{
	int8_t  la;
	la = lampTranslate[lamp];
	la = la -1;
	PORTD &= ~(1 << la);
}


int8_t randomNum(int8_t maximum)
{
	int ra = (rand() % maximum) + 1 ;
	return ra;
}

int8_t randomLamp()
{
	return randomNum (amtLights);
}

void randomJob ()
{
	if (stepsAmtLeft == 0) {
		stepsAmtLeft = randomLamp() ;
		if (actualLight == 0) {
			direction = upward;
		} else if (actualLight == amtLights) {
			direction = downward;
		} else {
			direction = randomNum(2) ;
		}
	} 
	if (stepsAmtLeft > 0) {
		if (direction == upward)  {
			if (actualLight < amtLights) {
				++ actualLight;
				switchOnLamp(actualLight);
				if (actualLight == amtLights){
					stepsAmtLeft = 1;
				}

			}
		} else {
			if (direction == downward) {
				if (actualLight > 0 ) {
					switchOffLamp(actualLight);
					-- actualLight;
					if (actualLight == 0){
						stepsAmtLeft = 1;
					}
				}
			}
			
		}
		--stepsAmtLeft;
	}
}


void initGaugeTimer()
{
	PORTD = 0x00;
	PIND  = 0x00;
	DDRD = 0xFF;   // all outputs
	PORTD = 0x00;
	
	actualLight = 0;
	direction = upward;
	stepsAmtLeft = 0;

	srand(0xAA55);   // until better solution is found

	setGaugeTimer();	
}


void nextGaugeTick()
{
	randomJob();
}

void testLamps()
{
	int8_t i1;
	for (i1 = 1; i1 <= 6; ++ i1) {
		switchOnLamp(i1);
	}
	for (i1 = 6; i1 > 0; --i1)   {
		switchOffLamp(i1);
	}
}

