#ifndef SBUS
#define SBUS

#include "usart.h"

#define RIGHT_VERT 1 //342-1706
#define RIGHT_HORIZ 0 //1700-342
#define LEFT_VERT 2 //342-1702
#define LEFT_HORIZ 3 //1706-370
#define RUDDR 4 //342-1702
#define ELEVDR 5 //342-1702
#define AILDR 6 //342-1702
#define GEAR 7 //342-1702



void tim2Init1(void);
void tim2Init2(void);
void tim3Init1(void);
void tim3Init2(void);
void tim4Init1(void);
void tim4Init2(void);
void tim5Init1(void);
void tim5Init2(void);

#endif