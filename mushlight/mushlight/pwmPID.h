
#ifndef triacPIDh
#define triacPIDh

#include<stdint.h>
#include <math.h>

int16_t lastADCVal;

#define desiredADCVal   400;    // ca. 2 V   , variable amplification in control-voltage


// Select 'double' or 'float' here:
typedef double real;


#define pidStepDelays  2  // unit is 1/42.18 sec, 
							// eg. 20 means approx 2 per second, 80 means 1 all 2 seconds

#define  pidStepsPerSecond    42.18 

void InitPID();

void resetPID();

void printPIDState();


void calcNextPWMDelay();

//	InitializePID(real kpTot, real kpP, real ki, real kd, real error_thresh, real step_time);
//InitializePID( -0.45, 1.1, 0.2, 0.2, 5, (pidStepDelays/pidStepsPerSecond));

#define kpTot  -0.45
#define  kpP 1.1
#define ki  0.2
#define kd  0.2
#define step_timeS   0.5   
#define integralCorrectionThreshold  10

#define maxCorrection  30    

#endif
