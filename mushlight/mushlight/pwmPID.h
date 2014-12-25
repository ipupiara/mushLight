
#ifndef triacPIDh
#define triacPIDh

#include<stdint.h>
#include <math.h>

int16_t lastADCVal;

#define desiredADCVal   110;


// Select 'double' or 'float' here:
typedef double real;


#define pidStepDelays  2  // unit is 1/42.18 sec, 
							// eg. 20 means approx 2 per second, 80 means 1 all 2 seconds

#define  pidStepsPerSecond    42.18 

void InitPID();

void resetPID();

void printPIDState();


void calcNextPWMDelay();


#endif
