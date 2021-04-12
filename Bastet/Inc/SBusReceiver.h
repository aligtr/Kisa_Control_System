#ifndef SBUS
#define SBUS

#include "dma.h"
#include "usart.h"

#define RIGHT_VERT 1 //342-1706
#define RIGHT_HORIZ 0 //1700-342
#define LEFT_VERT 2 //342-1702
#define LEFT_HORIZ 3 //1706-370
#define RUDDR 4 //342-1702
#define ELEVDR 5 //342-1702
#define AILDR 6 //342-1702
#define GEAR 7 //342-1702



void receiveSBusDate(uint16_t * channels);
void uartRxDMATransferCallback(DMA_HandleTypeDef *dmaHandle);

#endif