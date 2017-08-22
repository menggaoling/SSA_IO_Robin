#include "Beep.h"


__no_init static UCHAR bBuzzerEnable;
__no_init static UCHAR by_OnTime;
__no_init static UCHAR by_OffTime;
__no_init static UCHAR by_BeepTimes;
__no_init static UCHAR by_BeepOK;


void Beep_Initial(void)
{
    Beep_Hw_Init();
    by_OnTime = 0;
    by_OffTime = 0;
    by_BeepTimes = 0;
    by_BeepOK = 1;
    bBuzzerEnable = 1;
}

void Beep_Enable(UCHAR bStatus)
{
    if(bStatus == 0)
    {
        bBuzzerEnable = 0;
        Beep_Hw_Stop();
    }
    else
    {
        bBuzzerEnable = 1;
    }
}

void Beep_Proce(UCHAR by_Times, UCHAR by_On, UCHAR by_Off)
{
    by_BeepTimes = by_Times;
    by_OnTime = by_On * 50;
    by_OffTime = by_Off * 50;
    by_BeepOK = 0;
}

void Beep_1ms_Int(void)
{
    static UCHAR by_OnTimeCounter = 0;
    static UCHAR by_OffTimeCounter = 0;
    
    if(bBuzzerEnable == 1)
    {
        if(by_BeepTimes > 0)
        {
            if(by_OnTimeCounter == 0 && by_OffTimeCounter == 0)
            {
                by_OnTimeCounter = by_OnTime;
                by_OffTimeCounter = by_OffTime;
            }
        }
        else
        {
            by_BeepOK = 1;
        }
        
        
        if(by_OnTimeCounter)
        {
            by_OnTimeCounter--;
            Beep_Hw_Start();
        }
        else if(by_OffTimeCounter)
        {
            by_OffTimeCounter--;
            Beep_Hw_Stop();
            
            if(by_OffTimeCounter == 0)
            {
                by_BeepTimes--;
            }
        }
    }
}

UCHAR Beep_Get_OK(void)
{
    return bBuzzerEnable == 1 ? by_BeepOK : 1;
}

void Beep_Erp(void)
{
    Beep_Hw_Erp();
}




