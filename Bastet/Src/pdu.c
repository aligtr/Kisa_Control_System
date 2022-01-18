#include "pdu.h"

char checkDate(uint32_t date){
	if ((date>300) && (date<1800)) return 1;
	else return 0;
}

uint8_t checkLevel(uint16_t data)
{
	if(data>1000)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}