#include "Led.h"


__no_init UCHAR  by_FlashCurrentCount;
__no_init UCHAR  by_FlashTargetCount;
__no_init UCHAR  by_ErpMode;
__no_init UCHAR  by_Flag10ms;
__no_init UINT16 w_TimerCount;
__no_init UINT16 w_LedFlashTime;
__no_init UINT16 w_LedIntervalTime;


static void Led_Set_Mode(UINT16 wFlashTime, UCHAR byFlashCount);


void Led_Initial(void)
{
    Led_Hw_Initial();
    by_Flag10ms = 0;
    by_ErpMode = 0;
    w_TimerCount = 0;
    Led_Mode(LED_NORMAL);
}

void Led_Mode(LED_TYPE Mode)
{
    switch(Mode)
    {
    default:
    case LED_NORMAL:
        Led_Set_Mode(LED_INTERVAL_500MS, 1);
        break;
    case LED_SAFEKEY:
        Led_Set_Mode(LED_INTERVAL_300MS, 5);
        break;
    }
}

void Led_Set_Mode(UINT16 wFlashTime, UCHAR byFlashCount)
{
    UCHAR byCountTemp = byFlashCount * 2 - 1;
    if( w_LedFlashTime != wFlashTime || by_FlashTargetCount != byCountTemp )
    {
        w_LedFlashTime = wFlashTime;
        by_FlashTargetCount = byCountTemp;
        by_FlashCurrentCount = 0;
        w_LedIntervalTime = w_LedFlashTime == LED_INTERVAL_500MS ? LED_INTERVAL_500MS : LED_INTERVAL_1000MS;
        Led_Hw_Set();
    }
}

void Led_Process(void)
{
    if(by_Flag10ms)
    {
        by_Flag10ms = 0;
        if(by_ErpMode) return;
        if(by_FlashCurrentCount < by_FlashTargetCount)
        {
            if(++w_TimerCount >= w_LedFlashTime)
            {
                w_TimerCount = 0;
                ++by_FlashCurrentCount;
                Led_Hw_Toggle();
            }
        }
        else if(by_FlashCurrentCount >= by_FlashTargetCount)
        {
            if(++w_TimerCount >= w_LedIntervalTime)                             //隔1秒后再次重复
            {
                w_TimerCount = 0;
                by_FlashCurrentCount = 0;
                Led_Hw_Set();
            }
            else
            {
                Led_Hw_Clr();
            }
        }
    }
}

void Led_10ms_Int(void)
{
    by_Flag10ms = 1;
}

void Led_Erp(void)
{
    by_ErpMode = 1;
    Led_Hw_Erp();
}
