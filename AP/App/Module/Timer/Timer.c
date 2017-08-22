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

    if(by_Channel < TIMER_MAX_NUM)                                              //С�����TIMER_MAX_NUM����ʱ��
    {
        w_Mask = 0x01 << by_Channel;

        if(w_Flag & w_Mask)                                                     //ÿ��100msִ��һ�ε�ǰ��ʱ��.
        {
            w_Flag &= ~w_Mask;                                                  //��Ӧ��־λ������
            if(aw_Timer[by_Channel] < 0xffff) aw_Timer[by_Channel]++;           //ÿִ��һ����Ӧ�Ķ�ʱ���ͼ�1
            if(aw_Timer[by_Channel] == w_Dat)                                   //����Ŀ��ֵ����1,��֤ÿ����һ�ζ�ʱ����ֻ�ᴥ��һ��
            {
                by_Out = 1;                                                     //ֻ����ȲŻ������
                if(by_Index & T_LOOP) aw_Timer[by_Channel] = 0;                 //ѭ������
            }
        }
    }

    return(by_Out);
}



