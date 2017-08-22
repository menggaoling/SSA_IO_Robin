#ifndef __TIMER_H
#define __TIMER_H

#include "includes.h"


#define T_LOOP      0x80


void Timer_Initial(void);
void Timer_100ms_Int(void);
void Timer_Clear(UCHAR by_Index);
UCHAR Timer_Counter(UCHAR by_Index, UINT16 w_Dat);


#endif

