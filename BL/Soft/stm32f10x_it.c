/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : stm32f10x_it.c
* Author             : MCD Application Team
* Date First Issued  : Date               : 07/11/2008
* Description        : Main Interrupt Service Routines.
*                      This file provides template for all exceptions handler
*                      and peripherals interrupt service routine.
********************************************************************************
* History:
* 05/21/2007: V0.3
* 04/02/2007: V0.2
* 02/05/2007: V0.1
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes */
#include "stm32f10x_it.h"
#include "MyType.H"
#include "stm32f10x_pwr.h"

extern void SYSCLKConfig_STOP(void);
extern u8   by_SystemPowerStatus;

#define _Debug_NVIC_

/*******************************************************************************
* Function Name  : NMIException
* Description    : This function handles NMI exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NMI_Handler(void)
{
}
//miketsai test++
#include<stm32f10x_map.h>//miketsai ++
#include <stdio.h>

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Stack_Dump(unsigned int *hardfault_stack)
{
        unsigned int  r0 = ((unsigned long) hardfault_stack[0]);
        unsigned int  r1 = ((unsigned long) hardfault_stack[1]);
        unsigned int  r2 = ((unsigned long) hardfault_stack[2]);
        unsigned int  r3 = ((unsigned long) hardfault_stack[3]);
 
        unsigned int  r12 = ((unsigned long) hardfault_stack[4]);
        unsigned int  lr  = ((unsigned long) hardfault_stack[5]);
        unsigned int  pc  = ((unsigned long) hardfault_stack[6]);
        unsigned int  psr = ((unsigned long) hardfault_stack[7]);
 
        printf("\n\rHardFault_Handler");
        //printf ("\n\r[%s] Current Task(%s)\n", __FUNCTION__, getCurTaskName());  
        printf ("\n\rR0  = %x", r0);
        printf ("\n\rR1  = %x", r1);
        printf ("\n\rR2  = %x", r2);
        printf ("\n\rR3  = %x", r3);
        printf ("\n\rR12 = %x", r12);
        printf ("\n\rLR  = %x", lr);
        printf ("\n\rPC  = %x", pc);
        printf ("\n\rPSR = %x", psr);
        printf ("\n\rCFSR = %08x", (*((volatile unsigned long *)(0xE000ED28))));
        printf ("\n\rMMSR = %02x", (*((volatile unsigned char *)(0xE000ED28))));
        printf ("\n\rMMAR = %08x", (*((volatile unsigned long *)(0xE000ED34))));
        printf ("\n\rBFSR = %02x", (*((volatile unsigned char *)(0xE000ED29))));
        printf ("\n\rBFAR = %08x", (*((volatile unsigned long *)(0xE000ED38))));
        printf ("\n\rUFSR = %04x", (*((volatile unsigned short *)(0xE000ED2a))));
        printf ("\n\rHFSR = %08x", (*((volatile unsigned long *)(0xE000ED2C))));
        printf ("\n\rDFSR = %08x", (*((volatile unsigned long *)(0xE000ED30))));
        printf ("\n\rAFSR = %08x", (*((volatile unsigned long *)(0xE000ED3C))));
        printf ("\n\rSCB_SHCSR = %x\n", SCB->SysHandlerCtrl);
 
        /* Go to infinite loop when HardFault exception occurs */
        while(1);
 
}
//miketsai test++
#if 0

/*******************************************************************************
* Function Name  : HardFaultException
* Description    : This function handles Hard Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
typedef void(*TCallF)(void);
void HardFault_Handler(void)
{
    TCallF ProcessingFault;
    printf("\r\n HardFault_Handler....");
    ProcessingFault = (TCallF)(*(unsigned int*)0x800000C);
    ProcessingFault();

}
#else
/**
  * @brief  This function handles HardFault exception.
  * @param  None
  * @retval None
  */
#if defined ( __ICCARM__ )
void HardFault_Handler(void)
{
        /* Do not call any function before this sectino of code */
         asm("TST LR, #4\n"
                  "ITE EQ\n"
                  "MRSEQ R0, MSP\n"
                  "MRSNE R0, PSP\n"
                  "B HardFault_Stack_Dump");
}
#elif defined ( __CC_ARM   )
__asm void HardFault_GetStack()
{       
        /* Do not call any function before this sectino of code */
        TST LR, #4                ;
        ITE EQ                      ;       
        MRSEQ r0, msp  ;
        MRSNE r0, psp           ;
        B __cpp(HardFault_Stack_Dump)        ;
}
 
void HardFault_Handler(void)
{
        HardFault_GetStack();
}
#endif
#endif
/*******************************************************************************
* Function Name  : MemManageException
* Description    : This function handles Memory Manage exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MemManage_Handler(void)
{
	//printf("\r\n MemManage_Handler....");
}

/*******************************************************************************
* Function Name  : BusFaultException
* Description    : This function handles Bus Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BusFault_Handler(void)
{
}

/*******************************************************************************
* Function Name  : UsageFaultException
* Description    : This function handles Usage Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UsageFault_Handler(void)
{
}

/*******************************************************************************
* Function Name  : DebugMonitor
* Description    : This function handles Debug Monitor exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DebugMon_Handler(void)
{
}

/*******************************************************************************
* Function Name  : SVCHandler
* Description    : This function handles SVCall exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SVC_Handler(void)
{
}

/*******************************************************************************
* Function Name  : PendSVC
* Description    : This function handles PendSVC exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PendSV_Handler(void)
{
}

/*******************************************************************************
* Function Name  : SysTickHandler
* Description    : This function handles SysTick Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//static u8 Key_Delay=0;
static u8 LED_Loop=0;
static u8 LED_Loop1=0;
u16 Int_100msFlag;
u8 Interrupt_CheckSystem100msTimeup(u16 CheckBit )
{
    //My_DebugBlock=13;
    if ( !(Int_100msFlag & CheckBit) )
        return DFalse;
    else
        Int_100msFlag &= ~CheckBit;
    return DTrue;
}
volatile int ntime;
void SysTick_Handler(void)//==>每1us中斷一次
{ 
	if(ntime!=0)		//for delay_us
		ntime--;
}

/*******************************************************************************
* Function Name  : WWDG_IRQHandler
* Description    : This function handles WWDG interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WWDG_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : PVD_IRQHandler
* Description    : This function handles PVD interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PVD_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TAMPER_IRQHandler
* Description    : This function handles Tamper interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TAMPER_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : RTC_IRQHandler
* Description    : This function handles RTC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : FLASH_IRQHandler
* Description    : This function handles Flash interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : RCC_IRQHandler
* Description    : This function handles RCC interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXTI0_IRQHandler
* Description    : This function handles External interrupt Line 0 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI0_IRQHandler(void)
{
    // KEYPAD  Wake-up => nWAKE_UP#
    if( EXTI_GetITStatus(EXTI_Line0) != RESET )
    {
        EXTI_ClearITPendingBit(EXTI_Line0);   				
    }	
}

/*******************************************************************************
* Function Name  : EXTI1_IRQHandler
* Description    : This function handles External interrupt Line 1 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXTI2_IRQHandler
* Description    : This function handles External interrupt Line 2 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXTI3_IRQHandler
* Description    : This function handles External interrupt Line 3 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXTI4_IRQHandler
* Description    : This function handles External interrupt Line 4 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI4_IRQHandler(void)
{
//elead_daniel 100507   stopmode
    if(EXTI_GetITStatus(EXTI_Line4) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line4);
//      GPIO_SetBits(GPIOA,GPIO_Pin_11);		// PA11 SET 	
//      by_SystemPowerStatus = 0;
    }
}

/*******************************************************************************
* Function Name  : DMAChannel1_IRQHandler
* Description    : This function handles DMA Stream 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMAChannel2_IRQHandler
* Description    : This function handles DMA Stream 2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMAChannel3_IRQHandler
* Description    : This function handles DMA Stream 3 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMAChannel4_IRQHandler
* Description    : This function handles DMA Stream 4 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel4_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMAChannel5_IRQHandler
* Description    : This function handles DMA Stream 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel5_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMAChannel6_IRQHandler
* Description    : This function handles DMA Stream 6 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel6_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMAChannel7_IRQHandler
* Description    : This function handles DMA Stream 7 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel7_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : ADC_IRQHandler
* Description    : This function handles ADC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : USB_HP_CAN_TX_IRQHandler
* Description    : This function handles USB High Priority or CAN TX interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_HP_CAN_TX_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : USB_LP_CAN_RX0_IRQHandler
* Description    : This function handles USB Low Priority or CAN RX0 interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_LP_CAN_RX0_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : CAN_RX1_IRQHandler
* Description    : This function handles CAN RX1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_RX1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : CAN_SCE_IRQHandler
* Description    : This function handles CAN SCE interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_SCE_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXTI9_5_IRQHandler
* Description    : This function handles External lines 9 to 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI9_5_IRQHandler(void)
{
#if 0
    //My_DebugBlock=14;

    if(EXTI_GetITStatus(EXTI_Line5) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line5);
    }
    if(EXTI_GetITStatus(EXTI_Line8) != RESET)	//==>無線心跳
    {
        Heart_Reload_HR2_Int();
        EXTI_ClearITPendingBit(EXTI_Line8);		
    }
    if( EXTI_GetITStatus(EXTI_Line9) != RESET )
    {
        EXTI_ClearITPendingBit(EXTI_Line9);    				
    }
#endif	
}
	
/*******************************************************************************
* Function Name  : TIM1_BRK_IRQHandler
* Description    : This function handles TIM1 Break interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_BRK_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM1_UP_IRQHandler
* Description    : This function handles TIM1 overflow and update interrupt
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_UP_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM1_TRG_COM_IRQHandler
* Description    : This function handles TIM1 Trigger and Commutation interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_TRG_COM_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM1_CC_IRQHandler
* Description    : This function handles TIM1 capture compare interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_CC_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles TIM2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_IRQHandler(void)  /* TIM2_CH2 toggling with frequency = 1ms */
{	
#if 0
    if(TIM_GetITStatus(TIM2,TIM_IT_CC2) != RESET)
    {
		//My_DebugBlock=15;		
		if(DCheckSpiToArmEntry && GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12))		//==> 當SPI動作且封包傳輸結束(CS# => 1)關閉 SPI RX 中斷
        {   
            SPI_ITConfig(SPI2,SPI_IT_RXNE,DISABLE);		//==>關閉 RX 中斷
            if (NewSPI_CheckRX())
            {
                DResetSpiToArmFlag_DataReady
            }
        }
        Digital_AutoReply();				//Handle digital data come from LCB
        SocUart_PackageHandler();            //Handle uart package from Soc
        Heart_Timer_Counter_Int();          //Count HR
        NewTv_DelayTime_Int();
        Key_Scan_Int();                     //Scan key pad
        if(LED_Loop >= 50)                  //Add SPI watchdog timer every 5ms
        {
            LED_Loop=0;
            NewSPI_WatchDogTime();
        }
        else 
		{	
			LED_Loop++;
		}
		
        if(LED_Loop1 >= 100)                //Add timer every 100ms
        {
            LED_Loop1=0;
            Int_100msFlag = 0xffff;
            Timer_Flag_Int();
        }
        else
		{ 
			LED_Loop1++;
		}
        TIM_ClearITPendingBit(TIM2,TIM_IT_CC2);
        TIM_SetCompare2(TIM2,TIM_GetCapture2(TIM2)+100);
    }
#endif	
}
/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles TIM3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM3_IRQHandler(void)
{
#if 0
	/* TIM3_CH2 toggling with frequency = 1ms */
	if(TIM_GetITStatus(TIM3,TIM_IT_CC2) != RESET)
	{	
		Heart_Counter_Int();			
		TIM_ClearITPendingBit(TIM3,TIM_IT_CC2);
		TIM_SetCompare2(TIM3,TIM_GetCapture2(TIM3)+100);
	}
#endif	
}

/*******************************************************************************
* Function Name  : TIM4_IRQHandler
* Description    : This function handles TIM4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
volatile int buzzer_times=0;
volatile int odds=0;
void TIM4_IRQHandler(void)
{
#if 0
	u8 pin_status=0;

	if(TIM_GetITStatus(TIM4,TIM_IT_CC2) != RESET)
	{
		if(buzzer_times != 0)
		{
			if((odds%2)==0)
			{
				GPIO_SetBits(GPIOA, GPIO_Pin_4);
				odds++;
			}
			else
			{
				GPIO_ResetBits(GPIOA, GPIO_Pin_4);
				buzzer_times--;
				odds=0;
			}
		}
		else
		{
			TIM_ITConfig(TIM4,TIM_IT_CC2,DISABLE);
		}	
		TIM_ClearITPendingBit(TIM4,TIM_IT_CC2);
		TIM_SetCompare2(TIM4,TIM_GetCapture2(TIM4)+100);
	}
#endif	
}
/*******************************************************************************
* Function Name  : I2C1_EV_IRQHandler
* Description    : This function handles I2C1 Event interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C1_EV_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : I2C1_ER_IRQHandler
* Description    : This function handles I2C1 Error interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C1_ER_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : I2C2_EV_IRQHandler
* Description    : This function handles I2C2 Event interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C2_EV_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : I2C2_ER_IRQHandler
* Description    : This function handles I2C2 Error interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C2_ER_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : SPI1_IRQHandler
* Description    : This function handles SPI1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : SPI2_IRQHandler
* Description    : This function handles SPI2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI2_IRQHandler(void)
{
//#ifdef SocSPI
    //NewSPI_TxRxInformation();
//#endif    
}

/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1_IRQHandler(void)
{ 
    //SocUart_TxRx_Handler();
}
/*******************************************************************************
* Function Name  : USART2_IRQHandler
* Description    : This function handles USART2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART2_IRQHandler(void)//==>數位通訊傳輸
{
    //My_DebugBlock=16;
    //Digital_UartTxRx_Information();
}

/*******************************************************************************
* Function Name  : USART3_IRQHandler
* Description    : This function handles USART3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART3_IRQHandler(void)
{
	#if 0
	//BT_TxRx_Process();
	#else
	//RFID_TxRx_Process();
	#endif
}

/*******************************************************************************
* Function Name  : UART4_IRQHandler
* Description    : This function handles UART4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART4_IRQHandler(void)
{
    //My_DebugBlock=18;
    //NewTv_TxRX_Information();
}

/*******************************************************************************
* Function Name  : UART5_IRQHandler
* Description    : This function handles UART5 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART5_IRQHandler(void)
{
    //My_DebugBlock=19;
    //RF_TxRX_Information();
}

/*******************************************************************************
* Function Name  : EXTI15_10_IRQHandler
* Description    : This function handles External lines 15 to 10 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI15_10_IRQHandler(void)
{
    if( EXTI_GetITStatus(EXTI_Line12) != RESET )
    {
        //My_DebugBlock=20;
        EXTI_ClearITPendingBit(EXTI_Line12);
#if 0   // 	disable Usb update
        if ( GPIO_ReadInputDataBit( GPIOA, GPIO_Pin_12 ) == Bit_RESET )
        {
            NVIC_GenerateSystemReset();
        }
#endif 		
    }	

	// KEYPAD  Wake-up
    if( EXTI_GetITStatus(EXTI_Line10) != RESET )
    {
        EXTI_ClearITPendingBit(EXTI_Line10);    				
    }
    if( EXTI_GetITStatus(EXTI_Line11) != RESET )
    {
        EXTI_ClearITPendingBit(EXTI_Line11);    				
    }
    if( EXTI_GetITStatus(EXTI_Line13) != RESET )
    {
        EXTI_ClearITPendingBit(EXTI_Line13);    				
    }
    if( EXTI_GetITStatus(EXTI_Line14) != RESET )
    {
        EXTI_ClearITPendingBit(EXTI_Line14);   				
    }	
}

/*******************************************************************************
* Function Name  : RTCAlarm_IRQHandler
* Description    : This function handles RTC Alarm interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTCAlarm_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : USBWakeUp_IRQHandler
* Description    : This function handles USB WakeUp interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBWakeUp_IRQHandler(void)
{
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
