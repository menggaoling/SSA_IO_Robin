#ifndef __FAN_HW_H
#define __FAN_HW_H


#include "includes.h"


typedef enum
{
    FAN_CLOSE = 0,
    FAN_LSP,
    FAN_MSP,
    FAN_HSP,
    FAN_NUM,
} FAN_CTRL;


void Fan_Hw_Init(void);
void Fan_Hw_SetControl(FAN_CTRL FanCtrl);
void Fan_Hw_Erp(void);


#endif


