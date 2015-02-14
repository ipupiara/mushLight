#ifndef pwmpwmH
#define pwmpwmH


void startPWM();

void setPWMDelay(int16_t newDelay);

int16_t getPWMDelay();

void startBuzzerPWM();
void setBuzzerPWMWidth(int16_t  newWidth);
int16_t getBuzzerPWMWidth();

void initGaugeTimer();

void nextGaugeTick();

#endif
