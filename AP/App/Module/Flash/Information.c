#include "Information.h"


__no_init static MEM_INFOR *MemInfor;


UCHAR Information_Init(void)
{
    UCHAR by_Out = 0;
    
    Flash_Init();
    MemInfor = (MEM_INFOR*)Flash_Get_Point();
    
    Flash_Buffer_Read_Mass(0);
    if(MemInfor->wBuff[C_INFOR_SIZE-2] != D_PASSWORD) 
    {
        memset(MemInfor, 0xFF, sizeof(MEM_INFOR));
        
        MemInfor->Year = RTC_DEFAULT_YEAR;
        MemInfor->Month = RTC_DEFAULT_MONTH;
        MemInfor->Date = RTC_DEFAULT_DATE;
        MemInfor->Hour = RTC_DEFAULT_HOUR;
        MemInfor->Minute = RTC_DEFAULT_MINUTE;
        MemInfor->Second = RTC_DEFAULT_SECOND;
        
        MemInfor->wBuff[C_INFOR_SIZE-2] = D_PASSWORD;
        
        Flash_Buffer_Write_Mass(0);
        by_Out = 1;
    }
    
    return by_Out;
}

MEM_INFOR* Information_Get_Point(void)
{
    return MemInfor;
}

void Information_Read_Data(void)
{
    Flash_Buffer_Read_Mass(0);
}

void Information_Write_Data(void)
{
    Flash_Buffer_Write_Mass(0);
}


