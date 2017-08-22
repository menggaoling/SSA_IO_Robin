#include "Beep_Hw.h"
#include "stm32f10x_conf.h"


#define C_BEEP_PWM

__no_init static UCHAR by_BeepStatu;


void Beep_Hw_Init(void)
{
    HW_Set_IO_Output(BEEP_PORT, BEEP_PIN);
    HW_Set_IO_Low(BEEP_PORT, BEEP_PIN);
    
#ifdef C_BEEP_PWM
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef        NVIC_InitStructure;

    /* Time base configuration  TIM4*/
    TIM_TimeBaseStructure.TIM_Period        = 1;
    TIM_TimeBaseStructure.TIM_Prescaler     = 5924;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    TIM_PrescalerConfig(TIM4, 0xAFF, TIM_PSCReloadMode_Immediate);	 /* Prescaler configuration */
    TIM_Cmd(TIM4, ENABLE);											/* TIM4 enable counter */

    /* Enable the TIM4 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif
    by_BeepStatu = 0;
}

void Beep_Hw_Start(void)
{
    if(by_BeepStatu == 0)
    {
        by_BeepStatu = 1;
#ifdef C_BEEP_PWM
        TIM_ITConfig(TIM4, TIM_IT_CC2, ENABLE);
#else
        HW_Set_IO_High(BEEP_PORT, BEEP_PIN);
#endif
    }
}

void Beep_Hw_Stop(void)
{
    if(by_BeepStatu == 1)
    {
        by_BeepStatu = 0;
#ifdef C_BEEP_PWM
        TIM_ITConfig(TIM4, TIM_IT_CC2, DISABLE);
        HW_Set_IO_Low(BEEP_PORT, BEEP_PIN);
#else
        HW_Set_IO_Low(BEEP_PORT, BEEP_PIN);
#endif
    }
}

void Beep_Hw_Int(void)
{
#ifdef C_BEEP_PWM
    static bool bFlag = true;

    if(TIM_GetITStatus(TIM4, TIM_IT_CC2) != RESET)
    {
        if(bFlag)
        {
            HW_Set_IO_High(BEEP_PORT, BEEP_PIN);
        }
        else
        {
            HW_Set_IO_Low(BEEP_PORT, BEEP_PIN);
        }

        bFlag = !bFlag;

        TIM_ClearITPendingBit(TIM4, TIM_IT_CC2);
        TIM_SetCompare2(TIM4, TIM_GetCapture2(TIM4) + 100);
    }
#endif
}

void Beep_Hw_Erp(void)
{
#ifdef C_BEEP_PWM
    NVIC_InitTypeDef        NVIC_InitStructure;
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_DeInit(TIM4);
#endif
    HW_Set_IO_InputFloat(BEEP_PORT, BEEP_PIN);
}

