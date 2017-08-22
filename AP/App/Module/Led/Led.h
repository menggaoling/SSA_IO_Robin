#ifndef __LED_H__
#define __LED_H__

#include "Led_hw.h"


typedef enum
{
    LED_NORMAL                  = 1,
    LED_SAFEKEY                 = 2,
} LED_TYPE;


#define LED_INTERVAL_1000MS     100
#define LED_INTERVAL_500MS      50
#define LED_INTERVAL_300MS      30


void Led_Initial(void);
void Led_Process(void);
void Led_Mode(LED_TYPE Mode);
void Led_10ms_Int(void);
void Led_Erp(void);


#endif


