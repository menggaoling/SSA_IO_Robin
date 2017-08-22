#include "System.h"
#include "System_Hw.h"
#include "stm32f10x_conf.h"
#include "Timer.h"
#include "Beep.h"
#include "Tv.h"
#include "Heart.h"
#include "LCB.h"
#include "Led.h"
#include "Com.h"
#include "keyboard.h"
#include "Power.h"
#include "LCB_Update.h"
#include "Safekey.h"


void System_Initial(UCHAR bFirst)
{
    System_Hw_Initial(bFirst);
}

void System_Reset(void)
{
    NVIC_SystemReset();
}

void System_1ms(void)
{
    TV_1ms_Int();
    LCB_1ms_Int();
    Beep_1ms_Int();
    System_Hw_1ms_Int();
    Keyboard_1ms_Int();            //Scan key pad
    Heart_1ms_Int();
    Safekey_1ms_Int();
    
    static UCHAR by_Time10ms = 0;
    if(by_Time10ms++ >= 10)
    {
        by_Time10ms = 0;
        Power_10ms_Int();
        Led_10ms_Int();
        Com_10msInt();
        LCB_Update_10msInt();
    }

    static UCHAR by_Time20ms = 0;
    if(by_Time20ms++ >= 20)
    {
        by_Time20ms = 0;
        Keyboard_Flag_20ms_Int();
    }

    static UCHAR by_Time50ms = 0;
    if(by_Time50ms++ >= 50)
    {
        by_Time50ms = 0;
    }

    static UCHAR by_Time100ms = 0;
    if(by_Time100ms++ >= 100)
    {
        by_Time100ms = 0;
        Timer_100ms_Int();
    }
}

void System_ResetArm(void)
{
    System_Hw_ResetArm();
}

void System_OSEnterRecoveryMode(void)
{
    System_Hw_OSEnterRecoveryMode();
}

void System_Wakeup(void)
{
    System_Hw_Wakeup();
}

void System_Erp(void)
{
    System_Hw_Erp();
}

int _write(int fd, char* ptr, int len)                                          //relocation printf function
{
    for(int i=0; i<len; i++)
    {
        ITM_SendChar(ptr[i]);
    }
    
    return len;
}




