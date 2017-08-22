#include "Timer.h"


#define TIMER_MAX_NUM       32

__no_init static UINT32 w_Flag;
__no_init static UINT16 aw_Timer[TIMER_MAX_NUM];


void Timer_Initial(void)
{
    w_Flag = 0;
    memset(aw_Timer, 0, sizeof(aw_Timer));
}

void Timer_100ms_Int(void)
{
    w_Flag = 0xFFFFFFFF;
}

void Timer_Clear(UCHAR by_Index)
{
    if(by_Index < TIMER_MAX_NUM) aw_Timer[by_Index] = 0;
}

UCHAR Timer_Counter(UCHAR by_Index, UINT16 w_Dat)
{
    UINT32 w_Mask = 0;
    UCHAR  by_Out = 0;
    UCHAR  by_Channel = 0;

    by_Channel = by_Index & 0x7f;

    if(by_Channel < TIMER_MAX_NUM)                                              //小于最大TIMER_MAX_NUM个定时器
    {
        w_Mask = 0x01 << by_Channel;

        if(w_Flag & w_Mask)                                                     //每过100ms执行一次当前定时器.
        {
            w_Flag &= ~w_Mask;                                                  //相应标志位被清零
            if(aw_Timer[by_Channel] < 0xffff) aw_Timer[by_Channel]++;           //每执行一次相应的定时器就加1
            if(aw_Timer[by_Channel] == w_Dat)                                   //等于目标值返回1,保证每计数一次定时器，只会触发一次
            {
                by_Out = 1;                                                     //只有相等才会输出真
                if(by_Index & T_LOOP) aw_Timer[by_Channel] = 0;                 //循环计数
            }
        }
    }

    return(by_Out);
}



