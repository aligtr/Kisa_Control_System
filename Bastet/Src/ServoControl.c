#include "ServoControl.h"

extern CAN_HandleTypeDef hcan3;

HAL_StatusTypeDef canServoFrameSend(Servo_t servos)
{
    uint8_t rez,i;
    CAN_TxHeaderTypeDef msgHeader;
	uint16_t angleTransfer[4];
    uint8_t data[8];
    if(HAL_CAN_GetTxMailboxesFreeLevel(&hcan3)!=0)
    {   
        if (servos.frameType == CALIBRATION){
            data[0]=servos.frameType;
            msgHeader.DLC=1;
        }
        else{
            angleTransfer[0]=(uint16_t)((servos.servoTarget.targetFrontLeft+3.14)*4096/2/3.14);
            angleTransfer[1]=(uint16_t)((servos.servoTarget.targetFrontRight+3.14)*4096/2/3.14);
            angleTransfer[2]=(uint16_t)((servos.servoTarget.targetRearLeft+3.14)*4096/2/3.14);
            angleTransfer[3]=(uint16_t)((servos.servoTarget.targetRearRight+3.14)*4096/2/3.14);
            data[0]=angleTransfer[0]>>8;
            data[1]=angleTransfer[0];
            data[2]=angleTransfer[1]>>8;
            data[3]=angleTransfer[1];
            data[4]=angleTransfer[2]>>8;
            data[5]=angleTransfer[2];
            data[6]=angleTransfer[3]>>8;
            data[7]=angleTransfer[3];
            msgHeader.DLC=8;
        }

        msgHeader.StdId=servos.frameID;        
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
