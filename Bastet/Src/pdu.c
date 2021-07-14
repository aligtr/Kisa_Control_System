#include "pdu.h"

char checkDate(uint32_t date){
	if ((date>200) && (date<1000)) return 1;
	else return 0;
}

uint8_t checkLevel(uint16_t data)
{
	if(data>300)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}