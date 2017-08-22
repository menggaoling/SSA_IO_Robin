#ifndef __POWER_H__
#define __POWER_H__

#include "includes.h"
#include "hw.h"


#define C_PASSWORD              0x55AA5A5A
#define C_PASSWORD_RESET        0x0


void Power_Initial(void);
void Power_Set_Power_On(UINT32 w_Data);
UINT32 Power_Get_Power_On(void);
void Power_UI_On(void);
void Power_UI_Off(void);
void Power_USB1_On(void);
void Power_USB1_Off(void);
void Power_USB2_On(void);
void Power_USB2_Off(void);
void Power_USB3_On(void);
void Power_USB3_Off(void);
void Power_TV_On(void);
void Power_TV_Off(void);
void Power_10ms_Int(void);
void Power_Erp(void);

#endif

