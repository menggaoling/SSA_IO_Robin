#include "hw.h"



void HW_Test_IO_Initial(void)
{
    HW_Set_IO_Output(GPIOC, BIT0);
    HW_Set_IO_Low(GPIOC, BIT0);
    
    HW_Set_IO_Input(GPIOB, BIT1+BIT10+BIT11);
    HW_Set_IO_Input(GPIOC, BIT1+BIT4);
}

void HW_Test_IO_Process(void)
{
    static UCHAR by_Statu = 0;
    
    UCHAR by_StatuNew = 0;
    
    if(HW_Get_IO(GPIOB, BIT1))  BITSET(by_StatuNew, BIT0);
    if(HW_Get_IO(GPIOB, BIT10)) BITSET(by_StatuNew, BIT1);
    if(HW_Get_IO(GPIOB, BIT11)) BITSET(by_StatuNew, BIT2);
    if(HW_Get_IO(GPIOC, BIT1))  BITSET(by_StatuNew, BIT3);
    if(HW_Get_IO(GPIOC, BIT4))  BITSET(by_StatuNew, BIT4);
    
    if(by_StatuNew != by_Statu)
    {
        by_Statu = by_StatuNew;
        HW_Toggle_IO(GPIOC, BIT0);
    }
}

void HW_Empty_IO_Initial(void)
{
    /************************Set IO to Output**********************************/
    //PA
    HW_Set_IO_Output(GPIOA, BIT1+BIT8);
    
    //PB
    HW_Set_IO_Output(GPIOB, BIT7);
    
    //PC
    HW_Set_IO_Output(GPIOC, BIT6+BIT7+BIT12+BIT13+BIT14+BIT15);
    
    //PD
    //HW_Set_IO_Output(GPIOD, BIT0+BIT1);
    
    //PE
    HW_Set_IO_Output(GPIOE, BIT0+BIT1+BIT2+BIT3+BIT6+BIT15);

    /************************Set IO to Low*************************************/
    //PA
    HW_Set_IO_Low(GPIOA, BIT1+BIT8);
    
    //PB
    HW_Set_IO_Low(GPIOB, BIT7);
    
    //PC
    HW_Set_IO_Low(GPIOC, BIT6+BIT7+BIT12+BIT13+BIT14+BIT15);
    
    //PD
    //HW_Set_IO_Low(GPIOD, BIT0+BIT1);
    
    //PE
    HW_Set_IO_Low(GPIOE, BIT0+BIT1+BIT2+BIT3+BIT6+BIT15);
}

void HW_Empty_IO_Erp(void)
{
    /************************Set IO to Input**********************************/
    //PA
    HW_Set_IO_InputFloat(GPIOA, BIT1+BIT8+BIT13+BIT14+BIT15);
    
    //PB
    HW_Set_IO_InputFloat(GPIOB, BIT1+BIT3+BIT4+BIT7+BIT10+BIT11+BIT12+BIT13+BIT14+BIT15);
    
    //PC
    HW_Set_IO_InputFloat(GPIOC, BIT0+BIT1+BIT4+BIT6+BIT7+BIT13+BIT14+BIT15);
    
    //PD
    //HW_Set_IO_InputFloat(GPIOD, BIT0+BIT1+BIT7);
    
    //PE
    HW_Set_IO_InputFloat(GPIOE, BIT0+BIT1+BIT2+BIT3+BIT6+BIT15);
}


//*****************************************************************//
void HW_Set_IO_Output(GPIO_TypeDef *Port, UINT16 Pin)
{
    GPIO_InitTypeDef GPIO_InitS;
    GPIO_InitS.GPIO_Pin = Pin;
    GPIO_InitS.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitS.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(Port, &GPIO_InitS);
}

void HW_Set_IO_OutputLow(GPIO_TypeDef *Port, UINT16 Pin)
{
    GPIO_InitTypeDef GPIO_InitS;
    GPIO_InitS.GPIO_Pin = Pin;
    GPIO_InitS.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitS.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(Port, &GPIO_InitS);
}

void HW_Set_IO_Input(GPIO_TypeDef *Port, UINT16 Pin)
{
    GPIO_InitTypeDef GPIO_InitS;
    GPIO_InitS.GPIO_Pin = Pin;
    GPIO_InitS.GPIO_Mode = GPIO_Mode_IPD;//下拉输入,
    GPIO_Init(Port, &GPIO_InitS);
}

void HW_Set_IO_InputHigh(GPIO_TypeDef *Port, UINT16 Pin)
{
    GPIO_InitTypeDef GPIO_InitS;
    GPIO_InitS.GPIO_Pin = Pin;
    GPIO_InitS.GPIO_Mode = GPIO_Mode_IPU;//上拉输入,
    GPIO_Init(Port, &GPIO_InitS);
}

void HW_Set_IO_InputFloat(GPIO_TypeDef *Port, UINT16 Pin)
{
    GPIO_InitTypeDef GPIO_InitS;
    GPIO_InitS.GPIO_Pin = Pin;
    GPIO_InitS.GPIO_Mode = GPIO_Mode_IN_FLOATING;//上拉输入,
    GPIO_Init(Port, &GPIO_InitS);
}

//可以几个PIN一起写1
void HW_Set_IO_High(GPIO_TypeDef *Port, UINT16 Pin)
{
    GPIO_SetBits(Port, Pin);
}

//可以几个PIN一起写0
void HW_Set_IO_Low(GPIO_TypeDef *Port, UINT16 Pin)
{
    GPIO_ResetBits(Port, Pin);
}

//一次只可切换一个
void HW_Toggle_IO(GPIO_TypeDef *Port, UINT16 Pin)
{
    if(GPIO_ReadOutputDataBit(Port, Pin))
        GPIO_ResetBits(Port, Pin);
    else
        GPIO_SetBits(Port, Pin);
}

//一次只可读一个IO口
UCHAR HW_Get_IO(GPIO_TypeDef *Port, UINT16 Pin)
{
    return GPIO_ReadInputDataBit(Port, Pin);
}


#define FILTER_TIMES        5

UCHAR HW_Get_IO_Filter(GPIO_TypeDef *Port, UINT16 Pin)
{
    UCHAR  by_Loop = 0;
    UCHAR  by_CounterTimes = 5;                                                 //总的循环次数不超过5次
    UINT32 w_Out = HW_Get_IO(Port, Pin);

    do
    {
        by_Loop = 0;
        if(by_CounterTimes) by_CounterTimes--;
        for(UCHAR i = 0; i < FILTER_TIMES; i++)
        {
            if(w_Out != HW_Get_IO(Port, Pin))
            {
                by_Loop = 1;
                w_Out = HW_Get_IO(Port, Pin);
                break;
            }
        }
    }
    while(by_Loop && by_CounterTimes);

    return (w_Out ? 1 : 0);
}

