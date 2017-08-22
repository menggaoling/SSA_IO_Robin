/*Bootload version V5*/
/* Includes */
#include  "stm32f10x_lib.h"
#include  "General.h"
#include "MyType.H"
#include  "stm32f10x_it.h"
#include "cortexm3_macro.h"
#include "UpdateUart.h"
#include "core_cm3.h"
#include <stdio.h>

/* Macro define*/
#define ApplicationAddress          ((U32)0x08004000)
#define DFlashMark1					0x55AA5AA5
#define DFlashMark2					0x24681357
#define DFlashMark3					0x9BDEACF0
#define DFlashMark4					0x0FF0FF00


void PollingDelay_1mS(unsigned int DelayTime);

typedef void (*pFunction)(void);

/*Value define*/
U16         DelayTimeForEnterUserMode = 0;
U32         JumpAddress;
U32         JumpAddress_1;
pFunction   Jump_To_Application;
extern  unsigned int GU32_FaultMark;
//extern  unsigned int w_Power_On;
extern  U8  Sys_Reboot;
extern volatile int ntime;

#pragma location = "ConstSection1"
__root const unsigned int ENDMARK_[] = { DFlashMark1, DFlashMark2, DFlashMark3, DFlashMark4 };
__no_init unsigned int GU32_FaultMark@0x20007FF0;
__no_init unsigned int  w_Power_On@0x20002000;

//**********************************************************
// Function Name: Delay_4NOP
// Description: Delay 4 NOP cycle
//**********************************************************
void Delay_4NOP(void)
{
    ;
    ;
    ;
    ;
}

/*******************************************************************************
* Function Name  : Power1msDelay
* Description    : Generates idle timer 1ms.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Power1msDelay(u16 Delay1ms)
{
    u16  ttime1, ttime2, ttime3;

    for(ttime1 = 0; ttime1 < Delay1ms; ttime1++)
    {
        for(ttime2 = 0; ttime2 < 50; ttime2++)
        {
            for(ttime3 = 0; ttime3 < 25; ttime3++)
            {
                Delay_4NOP();
                Delay_4NOP();
                Delay_4NOP();
                Delay_4NOP();
            }
        }
    }
}

/*******************************************************************************
* Function Name  : delay_us
* Description    : Generates idle timer 1us.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void delay_us(int time_us)
{
    ntime = time_us;
    SysTick_CounterCmd(SysTick_Counter_Enable);   /* Enable the SysTick Counter */
    while(ntime != 0);
    SysTick_CounterCmd(SysTick_Counter_Disable);  /* Disable the SysTick Counter */
    SysTick_CounterCmd(SysTick_Counter_Clear);    /* Enable the SysTick Counter */
}

/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)            // main.c L215
{
    /* RCC system reset(for debug purpose) */
    RCC_DeInit();
   

    /* Enable HSE */
    RCC_HSEConfig(RCC_HSE_ON);
    RCC_WaitForHSEStartUp();

    /* Wait till HSE is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
    {
    }

    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 2 wait state */
    FLASH_SetLatency( FLASH_Latency_1 );

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
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);              // 0x00000002 => SYSCLK: 32MHz
    
    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
    
      /* Setting SysTick CLK source -- TED*/
    RCC_ClocksTypeDef System_CLK;
    // RCC_SYSCLKSource_PLLCLK
    RCC_GetClocksFreq(&System_CLK);
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);        //32MHz
    SysTick_SetReload(System_CLK.SYSCLK_Frequency/1000);    //1uS:1000000;1ms:1000
    //SysTick_CounterCmd(SysTick_Counter_Disable);             //SysTick_Counter_Disable,SysTick_Counter_Enable,SysTick_Counter_Clear
    SysTick_CounterCmd(SysTick_Counter_Clear);
    //SysTick_ITConfig(ENABLE);
    
   
}
//******************************************************************************
//* Function Name  : NVIC_Configuration
//* Description    : Configure the nested vectored interrupt controller.
//* Input          : None
//* Output         : None
//* Return         : None
//******************************************************************************
void NVIC_Configuration(void)
{
  

    
    NVIC_InitTypeDef    NVIC_InitStructure;
    NVIC_DeInit();//TED
//    NVIC_SCBDeInit();//TED
#ifdef  VECT_TAB_RAM
    /* Set the Vector Table base location at 0x20000000 */
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH  */
    /* Set the Vector Table base location at 0x08000000 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif
    /* Configure one bit for preemption priority (PreemptionPriority 1bit) (SubPriority 3bit) */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    NVIC_InitStructure.NVIC_IRQChannel                   = EXTI15_10_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    
   
    //NVIC_SystemHandlerConfig(SystemHandler_SysTick,ENABLE);
    //NVIC_SETPRIMASK();//all interrupt disable for piority mask
    //NVIC_RESETPRIMASK();// /all interrupt enable for piority mask...Test OK
     
     
   
    
   

}



/*******************************************************************************
* Function Name  : System_Initial
* Input          : None
* Output         : System Initial
* Return         : None
*******************************************************************************/
void System_Initial(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;

    EXTI_DeInit(); 	//==>先關閉中斷,避免因中斷產生導致的錯誤 (如USB燒入)
    RCC_Configuration();/* System Clocks Configuration */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_USART1, ENABLE);

    // Check if switch from user mode
    if(GU32_FaultMark != 0x5AA5F00F)
    {
         // Following is Not from user mode
         // start v2.0
         // PORT A : PMIC (A1), 9V (A8)
         GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
         GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1 | GPIO_Pin_8;
         GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
         GPIO_Init(GPIOA, &GPIO_InitStructure);
         //IO_Set_PMIC_High();
         //IO_Clear_9V_EN();

         // PORT C : 3.3V (C6), DDR3 (C13)
         GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_13;
         GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
         GPIO_Init(GPIOC, &GPIO_InitStructure);
         IO_Clear_3V3_IO_EN();
         IO_Clear_DDR3_VTT_EN();
         // PORT D
         GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_6 | GPIO_Pin_7;
         GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
         GPIO_Init(GPIOD, &GPIO_InitStructure);
         POWER_USB1_PORT_EN();//V4
         POWER_USB2_PORT_EN();
         POWER_USB3_PORT_EN();
         //IO_Set_RF_CE_Hi();
         //IO_Set_RF_Reset_Low();
         //IO_Clear_C_SAFE_EN();   //Turn off C-SAFE power
         // PORT E : 5v (E15), ESTOP (E4)
         GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4 | GPIO_Pin_15;
         GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
         GPIO_Init(GPIOE, &GPIO_InitStructure);
         IO_Clear_E_STOP_EN();

         // PORT B : System Power (B2)
         GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2|GPIO_Pin_0;
         GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
         GPIO_Init(GPIOB, &GPIO_InitStructure);
         POWER_USB_PORT_EN();     //V4
         IO_Set_System_Power();   //Set System power
          
         /*
         //Power on system
         Power1msDelay(2);
         IO_Set_3V3_IO_EN();
         IO_Set_DDR3_VTT_EN();
         Power1msDelay(2);
         IO_Set_5V_EN();
         IO_Set_9V_EN();
         Power1msDelay(3);
         IO_Set_E_STOP_EN();
         IO_Set_PMIC_High();
         */

         // Set Power control
         Power1msDelay(5000);
         IO_Set_3V3_IO_EN();
         IO_Set_DDR3_VTT_EN();
         Power1msDelay(10);
         IO_Set_5V_EN();
         Power1msDelay(10);
         IO_Set_9V_EN();
         Power1msDelay(10);
         LCM_SW_Hi();         // Turn on LCM.
         IO_Set_PMIC_High();  //Changed PMIC on/off control method from pulse trigger to level trigger.
         Power1msDelay(10);
         // end v2.0
   
        
        
        
    }
    NVIC_Configuration();
    //Enable uart1
    UPD_UARTInit();
    UPD_UARTGpioConfig();   // Set GPIO of Update UART port
    UPD_SetBaudRate();      // Set BaudRate of Update UART port (115200 bps-->9600 BPS)
    DResetArm_DeActive();

}
//******************************************************************************
//* Function Name  : EnterUserMode
//* Description    : Used Mode Function(Added by Used)
//* Input          : None
//* Output         : None
//* Return         : None
//******************************************************************************
void EnterUserMode(void)
{
    // start v2.0
    //if(ApplicationAddress==0x08004000)
    //{
    printf("Enter User Mode\n");
    JumpAddress = *(vu32 *) (ApplicationAddress + 4);
    JumpAddress_1 = *(vu32 *) ApplicationAddress;
    if( JumpAddress >= 0x08004000 && JumpAddress <= 0x0803FFFF && \
            JumpAddress_1 >= 0x20002000 && JumpAddress_1 <= 0x20005FFF) //
    {
        Jump_To_Application = (pFunction) JumpAddress;

        /* Initialize user application's Stack Pointer */
        __MSR_MSP(*(vu32 *) ApplicationAddress);
        Jump_To_Application();
    }
    //}
    // end v2.0
}
//*******************************************************************************
//* Function Name  : CheckFlashMark
//* Description    : Check Flash mark
//* Input          : None
//* Output         : None
//* Return         : None
//*******************************************************************************
BOOL CheckFlashMark(void)
{
    if( (ENDMARK_[0] == DFlashMark1) && (ENDMARK_[1] == DFlashMark2) && \
            (ENDMARK_[2] == DFlashMark3) && (ENDMARK_[3] == DFlashMark4) )
        return 1;
    return 0;
}


void PollingDelay_1mS(unsigned int DelayTime)
{
   unsigned long  SysTickCount;//u32
   unsigned int   DelayCt;
   unsigned char  Status;
          DelayCt = 0;
          SysTick_CounterCmd(SysTick_Counter_Clear); /* Enable the SysTick Counter */
          SysTickCount = SysTick_GetCounter();
          SysTick_CounterCmd(SysTick_Counter_Enable); /* Enable the SysTick Counter */
          do
          {
              Status = SysTick_GetFlagStatus(SysTick_FLAG_COUNT);
              SysTickCount = SysTick_GetCounter();
              if(Status == SET)
              {
                  if(DelayCt ++ >= DelayTime )
                    break;
                  //SysTick_SetReload(System_CLK.SYSCLK_Frequency/1000); //1uS:1000000;1ms:1000
                  SysTick_CounterCmd(SysTick_Counter_Clear); /* Enable the SysTick Counter */
              }
                

            }while (1);//(Status != SET);//不斷查詢標誌位，當載入初值與計數器相等時，標誌位置位。
            SysTick_CounterCmd(SysTick_Counter_Disable); /* Enable the SysTick Counter */
            SysTick_CounterCmd(SysTick_Counter_Clear); /* Enable the SysTick Counter */
             return;  
}
/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void main(void)
{
    
  
#if 1
    EXTI_DeInit();
    if ((GU32_FaultMark != 0x5AA5F00F) && CheckFlashMark())
    {
        EnterUserMode();
    }
#endif

    System_Initial();

    printf("Start BL main\n");

    while(1)
    {
        if (GetUPDCommandData() == DTrue)
        {
            UPD_CheckAndPackageCommand();
        }
        
        if (Sys_Reboot == 0xAA)          //reset mcu after 5 secs
        { 
          
            PollingDelay_1mS(APReboot_TIME);
          
            //delay_us(5000000);
            // if (DelayTimeForEnterUserMode > 5000){
            GU32_FaultMark = 0x00000001;
            Sys_Reboot     = 0;
            SysTick_CounterCmd(SysTick_Counter_Disable);
           // w_Power_On = 0x00000000;//0x55AA5A5A;
            NVIC_GenerateSystemReset();
            //NVIC_GenerateCoreReset();
            //EXTI_DeInit();
            //EnterUserMode();
            //}
        }
    }
}
