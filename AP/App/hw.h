#ifndef _HW_H
#define _HW_H

#include <includes.h>
#include "stm32f10x_conf.h"


void HW_Test_IO_Initial(void);
void HW_Test_IO_Process(void);
void HW_Empty_IO_Initial(void);
void HW_Empty_IO_Erp(void);
void HW_Set_IO_Output(GPIO_TypeDef *Port, UINT16 Pin);
void HW_Set_IO_OutputLow(GPIO_TypeDef *Port, UINT16 Pin);
void HW_Set_IO_Input(GPIO_TypeDef *Port, UINT16 Pin);
void HW_Set_IO_InputHigh(GPIO_TypeDef *Port, UINT16 Pin);
void HW_Set_IO_InputFloat(GPIO_TypeDef *Port, UINT16 Pin);
void HW_Set_IO_High(GPIO_TypeDef *Port, UINT16 Pin);
void HW_Set_IO_Low(GPIO_TypeDef *Port, UINT16 Pin);
void HW_Toggle_IO(GPIO_TypeDef *Port, UINT16 Pin);
UCHAR HW_Get_IO(GPIO_TypeDef *Port, UINT16 Pin);
UCHAR HW_Get_IO_Filter(GPIO_TypeDef *Port, UINT16 Pin);


#endif

