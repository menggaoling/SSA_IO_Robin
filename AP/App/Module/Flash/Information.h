#ifndef _INFORMATION_H_
#define _INFORMATION_H_

#include <includes.h>
#include "Flash.h"


#define D_PASSWORD              0x55aa
#define C_INFOR_SIZE            (C_BUFF_SIZE/2)


typedef union
{
    UINT16 wBuff[C_INFOR_SIZE];
    struct
    {
        UINT16 Year;
        UCHAR  Month;
        UCHAR  Date;
        UCHAR  Hour;
        UCHAR  Minute;
        UCHAR  Second;
    };
} MEM_INFOR;


/**************************/
//default time 2015/1/1/0:00:00
/**************************/
#define RTC_DEFAULT_YEAR        2015
#define RTC_DEFAULT_MONTH       1
#define RTC_DEFAULT_DATE        1
#define RTC_DEFAULT_HOUR        0
#define RTC_DEFAULT_MINUTE      0
#define RTC_DEFAULT_SECOND      0


UCHAR Information_Init(void);
MEM_INFOR* Information_Get_Point(void);
void Information_Read_Data(void);
void Information_Write_Data(void);


#endif



