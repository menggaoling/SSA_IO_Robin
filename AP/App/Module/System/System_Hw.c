#include "System_Hw.h"
#include "stm32f10x_conf.h"
#include "Power.h"


//UART1 COM     PA9,  PA10
//UART2 LCB     PA2,  PA3
//UART3 RF      PB10, PB11   
//UART4 TV      PC10, PC11    
//UART5 Heart   PC12, PD2     


#define SLEEP_WAKEUP_PORT               GPIOA
#define SLEEP_WAKEUP_PIN                BIT11

#define SOC_RECOVERY_PORT               GPIOE
#define SOC_RECOVERY_PIN                BIT5

#define RESET_ARM_PORT                  GPIOD
#define RESET_ARM_PIN                   BIT4

#define INT_ARM_PORT                    GPIOD
#define INT_ARM_PIN                     BIT3


__no_init static UCHAR bResetFlag;
__no_init static UCHAR bWakeupFlag;
__no_init static UCHAR bEnterRecoveryFlag;
__no_init static UINT16 w_ResetArmCounter;
__no_init static UINT16 w_WakeupCounter;
__no_init static UINT16 w_EnterRecoveryCounter;


void System_Hw_Initial(UCHAR bFirst)
{
    EXTI_DeInit();
    RCC_Configuration();
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4000);
    
    HW_Set_IO_Output(SOC_RECOVERY_PORT, SOC_RECOVERY_PIN);                      //PE5
    HW_Set_IO_High(SOC_RECOVERY_PORT, SOC_RECOVERY_PIN);                        //keep high default
    
    HW_Set_IO_Output(SLEEP_WAKEUP_PORT, SLEEP_WAKEUP_PIN);
    HW_Set_IO_High(SLEEP_WAKEUP_PORT, SLEEP_WAKEUP_PIN);
    
    HW_Set_IO_Output(INT_ARM_PORT, INT_ARM_PIN);                                //MCU_nINT
    HW_Set_IO_Low(INT_ARM_PORT, INT_ARM_PIN);
    
    HW_Set_IO_Output(RESET_ARM_PORT, RESET_ARM_PIN);                            //PD4
    if(bFirst)
    {
        bResetFlag = TRUE;
        HW_Set_IO_Low(RESET_ARM_PORT, RESET_ARM_PIN);
    }
    else
    {
        bResetFlag = FALSE;
        HW_Set_IO_High(RESET_ARM_PORT, RESET_ARM_PIN);
    }
    
    
    bWakeupFlag = FALSE;
    bEnterRecoveryFlag = FALSE;
    w_ResetArmCounter = 0;
    w_WakeupCounter = 0;
    w_EnterRecoveryCounter = 0;
}

void RCC_Configuration(void)            // main.c L215
{
    /* RCC system reset(for debug purpose) */
    RCC_DeInit();

    /* Enable HSE */
    RCC_HSEConfig(RCC_HSE_ON);
    RCC_WaitForHSEStartUp();

    /* Wait till HSE is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);

    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);                        // 0x00000000 => AHB clock = SYSCLK=32MHz

    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1);                         // 0x00000000 => APB2 clock = HCLK=32MHz

    /* PCLK1(max 36MHz) = HCLK */
    RCC_PCLK1Config(RCC_HCLK_Div2);                         // 0x00000400 => APB1 clock = HCLK/2=16MHz

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);                      // 0x00000002 =>

    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* PLLCLK = 8MHz * 4 = 32 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_4);    // 0x00010000, 0x00080000 => HSE oscillator clock selected,

    /* Enable PLL */
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);


    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);              // 0x00000002 => SYSCLK: 32MHz

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08);
    
    RCC_ClocksTypeDef RCC_ClocksStatus;
    RCC_GetClocksFreq(&RCC_ClocksStatus);
    
    SysTick_Config(RCC_ClocksStatus.SYSCLK_Frequency / 1000);	                /* systick source clk = 32MHz*/
    
    /* TIM4 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);                        //for beep

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); 	/* Enable DMA clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}

void System_Hw_Wakeup(void)
{
    HW_Set_IO_Low(SLEEP_WAKEUP_PORT, SLEEP_WAKEUP_PIN);
    HW_Set_IO_High(INT_ARM_PORT, INT_ARM_PIN);
    
    w_WakeupCounter = 0;
    bWakeupFlag = TRUE;
}

void System_Hw_ResetArm(void)
{
    HW_Set_IO_Low(RESET_ARM_PORT, RESET_ARM_PIN);
    w_ResetArmCounter = 0;
    bResetFlag = TRUE;
}

void System_Hw_OSEnterRecoveryMode(void)
{
    HW_Set_IO_Low(SOC_RECOVERY_PORT, SOC_RECOVERY_PIN);
    Power_UI_Off();
    w_EnterRecoveryCounter = 0;
    bEnterRecoveryFlag = TRUE;
}

void System_Hw_1ms_Int(void)
{
    //Wakeup
    if(bWakeupFlag)
    {
        if(w_WakeupCounter >= 100)                                              //20ms
        {
            HW_Set_IO_High(SLEEP_WAKEUP_PORT, SLEEP_WAKEUP_PIN);
            HW_Set_IO_Low(INT_ARM_PORT, INT_ARM_PIN);
            bWakeupFlag = FALSE;
        }
        else
        {
            w_WakeupCounter++;
        }
    }
    else
    {
        w_WakeupCounter = 0;
    }
    
    
    //ResetArm
    if(bResetFlag)
    {
        if(w_ResetArmCounter >= 500)
        {
            HW_Set_IO_High(RESET_ARM_PORT, RESET_ARM_PIN);
            bResetFlag = FALSE;
        }
        else
        {
            w_ResetArmCounter++;
        }
    }
    else
    {
        w_ResetArmCounter = 0;
    }
    
    
    //EnterRecoveryFlag
    if(bEnterRecoveryFlag)
    {
        if(w_EnterRecoveryCounter >= 1000)                                      //1000ms
        {
            Power_UI_On();
            bEnterRecoveryFlag = FALSE;
        }
        else
        {
            w_EnterRecoveryCounter++;
        }
    }
    else
    {
        w_EnterRecoveryCounter = 0;
    }
}

void System_Hw_Erp(void)
{
    DINT();
    SysTick->CTRL = 0;                                                          //πÿ±’systick÷–∂œ
    PWR_ClearFlag(PWR_FLAG_WU);
    PWR_ClearFlag(PWR_FLAG_SB);
    
    HW_Set_IO_InputFloat(SOC_RECOVERY_PORT, SOC_RECOVERY_PIN);
    HW_Set_IO_InputFloat(SLEEP_WAKEUP_PORT, SLEEP_WAKEUP_PIN);
    HW_Set_IO_InputFloat(RESET_ARM_PORT, RESET_ARM_PIN);
    HW_Set_IO_InputFloat(INT_ARM_PORT, INT_ARM_PIN);
    
    for(UCHAR i=0; i<0xff; i++)
    {
        NOP();
    }
    
    EINT();
    PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFE);
}



