#include "pdu.h"

char checkDate(uint32_t date){
	if(date>100) return 0;
	else return 1;
}