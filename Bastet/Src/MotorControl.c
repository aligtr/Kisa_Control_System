#include "MotorControl.h"
#include "string.h"

extern CAN_HandleTypeDef hcan3;

void arrayCopy(uint8_t* src, uint8_t* dst,uint16_t size)
{
    uint16_t i;
    for(i=0;i<size;i++)
    {
        dst[i]=src[i];
    }
}

HAL_StatusTypeDef canFrameSend(MCFrame_t frame)
{
    uint8_t rez,i;
    CAN_TxHeaderTypeDef msgHeader;
    uint8_t data[8];
    if(HAL_CAN_GetTxMailboxesFreeLevel(&hcan3)!=0)
    {
        data[0]=frame.frameType;
        for(i=0;i<7;i++)
        {
            data[i+1]=frame.data[i];
        }
        msgHeader.StdId=frame.id;
        msgHeader.DLC=8;
        msgHeader.TransmitGlobalTime=DISABLE;
        msgHeader.RTR=CAN_RTR_DATA;
        msgHeader.IDE=CAN_ID_STD;
        return HAL_CAN_AddTxMessage(&hcan3,&msgHeader,&data,rez);
    }
    else
    {
        return HAL_ERROR;
    }
}

HAL_StatusTypeDef motorRealeseCommand(Motor_t motor)
{
    MCFrame_t frame;
    uint8_t i;
    for(i=0;i<8;i++)
        frame.data[i]=0;
    switch (motor.command)
    {
    case MOTOR_START:
        if(motor.status==MOTOR_START)
        {
            return HAL_OK;
        }
        break;
    case MOTOR_STOP:
        if(motor.status==MOTOR_STOP)
        {
            return HAL_OK;
        }
        break;
    case CHANGE_SPEED:
        if(motor.speed==motor.prevSpeed)
        {
            return HAL_OK;    
        }
        arrayCopy(&motor.speed,&frame.data,2);
        break;
    case SET_SPEED_PID:
        arrayCopy(&motor.speedPID,&frame.data,sizeof(uint16_t)*3);
        break;
    case SET_FLUX_PID:
        arrayCopy(&motor.fluxPID,&frame.data,sizeof(uint16_t)*3);
        break;
    case SET_TORQUE_PID:
        arrayCopy(&motor.torquePID,&frame.data,sizeof(uint16_t)*3);
        break;        
    }
    frame.id = motor.motorID;
    frame.frameType=motor.command;
    return canFrameSend(frame);
    
}

