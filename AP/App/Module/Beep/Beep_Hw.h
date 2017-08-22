#ifndef __BEEP_HW_H
#define __BEEP_HW_H


#include "includes.h"
#include "hw.h"


#define BEEP_PORT       GPIOA
#define BEEP_PIN        BIT4


void Beep_Hw_Init(void);
void Beep_Hw_Start(void);
void Beep_Hw_Stop(void);
void Beep_Hw_Int(void);
void Beep_Hw_Erp(void);


#endif

