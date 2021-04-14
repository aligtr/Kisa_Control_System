
#ifndef __MotorControl_H
#define __MotorControl_H
#include "main.h"


typedef enum
{
    STOP,
    RUN,
    FAIL
}MotorStatus_t;

typedef enum
{
    NONE=0x00,
    MOTOR_START=0x01,
    MOTOR_STOP=0x02,
    CHANGE_SPEED=0x03,
    SET_SPEED_PID=0x04,
    SET_TORQUE_PID=0x05,
    SET_FLUX_PID=0x06,
    CHANGE_TORQUE=0x07
}FrameType_t;

typedef enum
{
    //NONE=0x00,
    WFL=0x200,
    WFR=0x210,
    WRL=0x220,
    WRR=0x230,
    SFL=0x240,
    SFR=0x250,
    SRL=0x260,
    SRR=0x270
}motorID_t;

typedef struct 
{
    motorID_t id;
    FrameType_t frameType;
    uint8_t data[7];
}MCFrame_t;

typedef struct
{
    motorID_t motorID;
    FrameType_t command;
    MotorStatus_t status;
    int16_t refImpact;
    int16_t prevRefImpact;
    uint16_t speedPID[3];
    uint16_t torquePID[3];
    uint16_t fluxPID[3]; 
}Motor_t;


HAL_StatusTypeDef canFrameSend(MCFrame_t frame);
HAL_StatusTypeDef motorRealeseCommand(Motor_t motor);




#endif