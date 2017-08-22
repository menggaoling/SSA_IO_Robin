#include "WatchDog.h"
#include "hw.h"


//#define WATCHDOG_ENABLE


void Watchdog_Initial(void)
{
#ifdef WATCHDOG_ENABLE
    /* IWDG timeout equal to 250 ms (the timeout may varies due to LSI frequency
    dispersion) */
    /* Enable write access to IWDG_PR and IWDG_RLR registers */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    
    /* ����ʱ�ӷ�Ƶ,40K/256=156HZ(6.4ms)*/
    IWDG_SetPrescaler(IWDG_Prescaler_256);
    
    /* ι��ʱ�� 3s/6.4MS = 468  3s��λϵͳ.ע�ⲻ�ܴ���0xfff*/
    IWDG_SetReload(468);
    
    /* Reload IWDG counter */
    IWDG_ReloadCounter();
    
    /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
    IWDG_Enable();
#endif
}

void Watchdog_Close(void)
{
#ifdef WATCHDOG_ENABLE
    
#endif
}

void Watchdog_Reset(void)
{
#ifdef WATCHDOG_ENABLE
    IWDG_ReloadCounter();
#endif
}


