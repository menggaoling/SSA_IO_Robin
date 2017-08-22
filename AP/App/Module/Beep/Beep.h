#ifndef __BEEP_H
#define __BEEP_H

#include "Beep_Hw.h"
#include "stm32f10x_it.h"


void Beep_Initial(void);
void Beep_Enable(UCHAR bStatus);
void Beep_Proce(UCHAR by_Times, UCHAR by_On, UCHAR by_Off);
UCHAR Beep_Get_OK(void);
void Beep_1ms_Int(void);
void Beep_Erp(void);


#endif


