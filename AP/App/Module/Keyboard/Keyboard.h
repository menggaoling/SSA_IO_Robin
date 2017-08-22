#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include "includes.h"


//单个按键值不能超过 62 = 0x3F - 1
#define KB_NONE             0x00
#define KB_NA               0xff
#define KB_HOLD_3S          0x80


#define KB_HOME             1
#define KB_DASH             2
#define KB_ENTER            3
#define KB_START            4
#define KB_STOP             5
#define KB_SPEED_UP         6
#define KB_SPEED_DOWN       7
#define KB_INCLINE_UP       8
#define KB_INCLINE_DOWN     9
#define KB_LEVEL_UP         10
#define KB_LEVEL_DOWN       11
#define KB_PAUSE            12
#define KB_DISPLAY_MODE     13

#define KB_0                20
#define KB_1                21
#define KB_2                22
#define KB_3                23
#define KB_4                24
#define KB_5                25
#define KB_6                26
#define KB_7                27
#define KB_8                28
#define KB_9                29

#define KB_CC_MUTE          30      //Closed Caption
#define KB_TV_CU            31      //channel up
#define KB_TV_CD            32      //channel down
#define KB_TV_VU            33      //volume up
#define KB_TV_VD            34      //volume up
#define KB_TV_ENTER         35
#define KB_TV_LAST          36



/*********************************组合按键值***********************************/
#define KB_CD_VU_VD         50
#define KB_VD_CU_CD         51
#define KB_CD_CU            52
#define KB_TIMEUP_TIMEDOWN  53

#define KB_ENTER_RECOVER    KB_CD_VU_VD + KB_HOLD_3S
#define KB_RESET_SYSTEM     KB_VD_CU_CD + KB_HOLD_3S
#define KB_RESET_SOC1       KB_CD_CU + KB_HOLD_3S		                        // compount K_TV_CD + K_TV_CU 	   // 110225	
#define KB_RESET_SOC2       KB_TIMEUP_TIMEDOWN + KB_HOLD_3S		                // compount K_TimeU + K_TimeD	   // 110225



void Keyboard_Initial(void);
void Keyboard_EnableBeep(UCHAR bStatus);
void Keyboard_ResetMask(void);
void Keyboard_Flag_20ms_Int(void);
void Keyboard_1ms_Int(void);
UCHAR Keyboard_Get_Code(void);
void Keyboard_Erp(void);


#endif





