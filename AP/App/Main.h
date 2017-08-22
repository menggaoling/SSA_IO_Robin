#ifndef  __MAIN_H
#define  __MAIN_H

#include <includes.h>
#include "stm32f10x_conf.h"
#include "stm32f10x_it.h"
#include "Beep.h"
#include "Heart.h"
#include "Fan.h"
#include "keyboard.h"
#include "LCB.h"
#include "Timer.h"
#include "Tv.h"
#include "System.h"
#include "Safekey.h"
#include "Led.h"
#include "Com.h"
#include "Power.h"
#include "I2C.h"
#include "WatchDog.h"
#include "LCB_Update.h"
#include "Information.h"
#include "Flash.h"


typedef enum
{
    TIMER_COMMON                = 0,
    TIMER_STANDARD,
    TIMER_SAFEKEY_CHECK_DELAY,
    TIMER_SAFEKEY_CLEAR_DIGITAL,
    TIMER_SAFEKEY_CLEAR,
    TIMER_IR_CLEAR_DIGITAL,
    TIMER_ENTER_UPDATE_DELAY,
    TIMER_SEND_ERROR_DELAY,
    TIMER_SLEEP_DELAY,
    TIMER_UPDATE_MCB_RESENT,
    TIMER_READ_MCB_RESENT,
    TIMER_RESET_SOC,
    TIMER_TEST,
} TIMER;

typedef enum
{
    ERP_NONE = 0,
    ERP_ENTER,
    ERP_WAIT_LEAVE,
    ERP_LEAVE,
} ERP_STATUS;


void Main_Init(void);
void Main_EnterBSLMode(void);
void Main_ComProcess(void);
void Main_SafeKeyProcess(void);
void Main_UpdateProcess(void);
void Main_KeyProcess(void);
void Main_ERP_Process(void);
UINT16 Main_GetStatus(void);
void Main_Reset_Log(UINT16 wNum);


#endif


