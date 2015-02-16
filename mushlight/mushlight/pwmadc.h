#ifndef  pwmadcH
#define pwmadcH

void startADC();

int8_t   adcTick;
int8_t   timerTick;

void startBuzzerADC();

void enterIdleSleepMode();

void setGaugeTimer();

//#define BUZZER
#define GAUGE



#endif
