#include "Heart.h"


#define C_DEFAULT_MINUTE            60000
#define C_HEART_MAX                 240
#define C_HEART_MIN                 5
#define C_PULSE_MAX                 C_DEFAULT_MINUTE/C_HEART_MIN  // 1500
#define C_PULSE_MIN                 C_DEFAULT_MINUTE/C_HEART_MAX  // 250
#define C_MEM_SIZE                  20
#define C_HRPLOAR_OFFSET            5
#define C_DATA_COUNT                8
#define C_HRPOLAR_MIN               40


__no_init static UINT16 w_PolarPulseCount;                                      //1:polar  2:hr
__no_init static UINT16 w_HRPulseCount;
__no_init static UCHAR by_HandHeart;
__no_init static UCHAR by_PolarHeart;
__no_init static UCHAR aby_HRBuff[C_MEM_SIZE];
__no_init static UCHAR aby_PolarBuff[C_MEM_SIZE];
__no_init static struct
{
    unsigned PolarDetect        : 1;
    unsigned PolarStop          : 1;
    unsigned PolarBlink         : 1;
    unsigned PolarUpdate        : 1;
    unsigned HR_Detect          : 1;
    unsigned HR_Stop            : 1;
    unsigned HR_Blink           : 1;
    unsigned HR_Update          : 1;
}Heart;


UCHAR Heart_Calculate_MEM(UCHAR *pby_Buff, UCHAR by_HeartSave);
UCHAR Heart_Cal(UCHAR by_HR, UCHAR by_HRNew, UCHAR by_Count);


void Heart_Initial(void)
{
    Heart_Hw_Init();
    
    memset(&Heart, 0, sizeof(Heart));
    memset(aby_HRBuff, 0, sizeof(aby_HRBuff));
    memset(aby_PolarBuff, 0, sizeof(aby_PolarBuff));
    
    Heart.PolarStop = 1;
    Heart.HR_Stop = 1;
    by_HandHeart = 0;
    by_PolarHeart = 0;
}

void Heart_Polar_Int(void)
{
    if(Heart.PolarDetect)
    {
        if(w_PolarPulseCount > C_PULSE_MIN)
        {
            UINT16 wPolar = w_PolarPulseCount;
            
            if(wPolar > C_PULSE_MAX) wPolar = C_PULSE_MAX;

            wPolar = C_DEFAULT_MINUTE / wPolar;
            for(UCHAR i=(C_MEM_SIZE-1); i>0; i--)
            {
                aby_PolarBuff[i] = aby_PolarBuff[i-1];
            }
            
            aby_PolarBuff[0] = wPolar;
            
            Heart.PolarBlink = 1;
            Heart.PolarStop = 0;
            Heart.PolarUpdate = 1;
        }
    }
    else
    {
        Heart.PolarDetect = 1;
    }
    
    w_PolarPulseCount = 0;
}

void Heart_HR_Int(void)
{
    if(Heart.HR_Detect)
    {
        if(w_HRPulseCount > C_PULSE_MIN)
        {
            UINT16 wHR = w_HRPulseCount;
            
            if(wHR > C_PULSE_MAX) wHR = C_PULSE_MAX;
            
            wHR = C_DEFAULT_MINUTE / wHR;
            for(UCHAR i=(C_MEM_SIZE-1); i>0; i--)
            {
                aby_HRBuff[i] = aby_HRBuff[i-1];
            }
            
            aby_HRBuff[0] = wHR;
            
            Heart.HR_Blink = 1;
            Heart.HR_Stop = 0;
            Heart.HR_Update = 1;
        }
    }
    else
    {
        Heart.HR_Detect = 1;
    }
    
    w_HRPulseCount = 0;
}

UCHAR Heart_Get_Heart(void)
{
    return by_PolarHeart > 0 ? by_PolarHeart : by_HandHeart;
}

UCHAR Heart_Get_Blink(void)
{
    return (!Heart.PolarStop ? Heart.PolarBlink : Heart.HR_Blink);
}

UCHAR Heart_Get_Source(void)
{
    return (!Heart.PolarStop ? 0 : 1);
}

void Heart_1ms_Int(void)
{
    static UCHAR by_TimerPolar = 0;
    
    if(w_PolarPulseCount++ > C_PULSE_MAX)
    {
        if(!Heart.PolarStop)
        {
            by_PolarHeart = 0;
            Heart.PolarStop = 1;
        }
        
        Heart.PolarDetect = 0;
        w_PolarPulseCount = 0;
        //memset(aby_PolarBuff, 0, sizeof(aby_PolarBuff));
    }
    
    
    static UCHAR by_TimerHR = 0;
    if(w_HRPulseCount++ > C_PULSE_MAX)
    {
        if(!Heart.HR_Stop)
        {
            by_HandHeart = 0;
            Heart.HR_Stop = 1;
        }
        
        Heart.HR_Detect = 0;
        w_HRPulseCount = 0;
        //memset(aby_HRBuff, 0, sizeof(aby_HRBuff));
    }
    
    
    if(Heart.PolarBlink)
    {
        if(by_TimerPolar++ > 250)
        {
            Heart.PolarBlink = 0;
        }
    }
    else
    {
        by_TimerPolar = 0;
    }
    
    
    if(Heart.HR_Blink)
    {
        if(by_TimerHR++ > 250)
        {
            Heart.HR_Blink = 0;
        }
    }
    else
    {
        by_TimerHR = 0;
    }
}

void Heart_Process(void)
{
    if(Heart.PolarUpdate)
    {
        Heart.PolarUpdate = 0;
        
        if(!Heart.PolarStop)
        {
            by_PolarHeart = Heart_Calculate_MEM(aby_PolarBuff, by_PolarHeart);
        }
        else
        {
            by_PolarHeart = 0;
        }
    }
    
    if(Heart.HR_Update)
    {
        Heart.HR_Update = 0;
        
        if(!Heart.HR_Stop)
        {
            by_HandHeart = Heart_Calculate_MEM(aby_HRBuff, by_HandHeart);
        }
        else
        {
            by_HandHeart = 0;
        }
    }
}

UCHAR Heart_Calculate_MEM(UCHAR *pby_Buff, UCHAR by_HeartSave)
{
    UCHAR by_Out = 0;
    UCHAR by_Count = 0;
    UCHAR by_Max_Count = 0;
    UINT16 w_Max_Total = 0;
    UINT16 w_Total = 0;
    
    if(pby_Buff[0] == 0 && pby_Buff[1] == 0 && pby_Buff[2] == 0)
    {
        by_Out = 0;
        by_Max_Count = C_MEM_SIZE;
    }
    else
    {
        for(UCHAR by_Num=0; by_Num<C_MEM_SIZE; by_Num++)
        {
            by_Count = 0;
            w_Total = 0;
            if(pby_Buff[by_Num] < 40) continue;
            
            for(UCHAR by_Loop = 0;by_Loop < C_MEM_SIZE;by_Loop++)
            {
                if(pby_Buff[by_Loop] <= (pby_Buff[by_Num]+C_HRPLOAR_OFFSET) &&
                   pby_Buff[by_Loop] >= (pby_Buff[by_Num]-C_HRPLOAR_OFFSET))
                {
                    ++by_Count;
                    w_Total += pby_Buff[by_Loop];
                }
            }
            
            if(by_Count > by_Max_Count)
            {
                by_Max_Count = by_Count;
                w_Max_Total = w_Total;
            }
        }
    }
    
    
    UCHAR by_Temp = w_Max_Total / by_Max_Count;
    by_Out = by_HeartSave;
    
    if(by_Max_Count > C_DATA_COUNT)
    {
        by_Out = by_Temp;
    }
    else
    {
        if(by_Out > 40)
        {
            if(by_Temp > by_Out) ++by_Out;
            if(by_Temp < by_Out) --by_Out;
        }
        else
        {
            by_Out = (700+by_Temp) / 11;
        }
    }
    
    return by_Out;
}

void Heart_Erp(void)
{
    Heart_Hw_Erp();
}




