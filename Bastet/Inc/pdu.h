#ifndef PDU_H
#define PDU_H    
#include "main.h"
#include "SBusReceiver.h"

typedef struct 
{
    uint16_t vel_mean;
    uint16_t dir_mean;
    uint16_t rx_mean;
    uint16_t smt_src1;
    uint16_t smt_src2;
    uint16_t smt_src3;
    uint16_t smt_src4;
    uint16_t ry_mean;
    uint16_t mode;
} PduData_t;


char checkDate(uint32_t date);
#endif