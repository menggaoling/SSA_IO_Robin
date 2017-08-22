#include "Keyboard_Hw.h"
#include "hw.h"


#define KEYBOARD_SCAN0_PORT		GPIOE
#define KEYBOARD_SCAN0_PIN		BIT7

#define KEYBOARD_SCAN1_PORT		GPIOE
#define KEYBOARD_SCAN1_PIN		BIT8

#define KEYBOARD_SCAN2_PORT		GPIOE
#define KEYBOARD_SCAN2_PIN		BIT9

#define KEYBOARD_SCAN3_PORT		GPIOE
#define KEYBOARD_SCAN3_PIN		BIT10

#define KEYBOARD_SCAN4_PORT		GPIOE
#define KEYBOARD_SCAN4_PIN		BIT11

#define KEYBOARD_SCAN5_PORT		GPIOE
#define KEYBOARD_SCAN5_PIN		BIT12

#define KEYBOARD_SCAN6_PORT		GPIOE
#define KEYBOARD_SCAN6_PIN		BIT13

#define KEYBOARD_SCAN7_PORT		GPIOE
#define KEYBOARD_SCAN7_PIN		BIT14


//#define KEYBOARD_DATA0_PORT     GPIOD
//#define KEYBOARD_DATA0_PIN	    BIT7

#define KEYBOARD_DATA1_PORT     GPIOD
#define KEYBOARD_DATA1_PIN	    BIT8

#define KEYBOARD_DATA2_PORT     GPIOD
#define KEYBOARD_DATA2_PIN	    BIT9

#define KEYBOARD_DATA3_PORT     GPIOD
#define KEYBOARD_DATA3_PIN	    BIT10

#define KEYBOARD_DATA4_PORT     GPIOD
#define KEYBOARD_DATA4_PIN	    BIT11

#define KEYBOARD_DATA5_PORT     GPIOD
#define KEYBOARD_DATA5_PIN	    BIT12

#define KEYBOARD_DATA6_PORT     GPIOD
#define KEYBOARD_DATA6_PIN	    BIT13

#define KEYBOARD_DATA7_PORT     GPIOD
#define KEYBOARD_DATA7_PIN	    BIT14


void Keyboard_HW_Initial(void)
{
    HW_Set_IO_OutputLow(KEYBOARD_SCAN0_PORT, KEYBOARD_SCAN0_PIN);
    HW_Set_IO_OutputLow(KEYBOARD_SCAN1_PORT, KEYBOARD_SCAN1_PIN);	
    HW_Set_IO_OutputLow(KEYBOARD_SCAN2_PORT, KEYBOARD_SCAN2_PIN);
    HW_Set_IO_OutputLow(KEYBOARD_SCAN3_PORT, KEYBOARD_SCAN3_PIN);		
    HW_Set_IO_OutputLow(KEYBOARD_SCAN4_PORT, KEYBOARD_SCAN4_PIN);
    HW_Set_IO_OutputLow(KEYBOARD_SCAN5_PORT, KEYBOARD_SCAN5_PIN);
    HW_Set_IO_OutputLow(KEYBOARD_SCAN6_PORT, KEYBOARD_SCAN6_PIN);
    HW_Set_IO_OutputLow(KEYBOARD_SCAN7_PORT, KEYBOARD_SCAN7_PIN);
    
    HW_Set_IO_High(KEYBOARD_SCAN0_PORT, KEYBOARD_SCAN0_PIN);
    HW_Set_IO_High(KEYBOARD_SCAN1_PORT, KEYBOARD_SCAN1_PIN);	
    HW_Set_IO_High(KEYBOARD_SCAN2_PORT, KEYBOARD_SCAN2_PIN);
    HW_Set_IO_High(KEYBOARD_SCAN3_PORT, KEYBOARD_SCAN3_PIN);		
    HW_Set_IO_High(KEYBOARD_SCAN4_PORT, KEYBOARD_SCAN4_PIN);
    HW_Set_IO_High(KEYBOARD_SCAN5_PORT, KEYBOARD_SCAN5_PIN);
    HW_Set_IO_High(KEYBOARD_SCAN6_PORT, KEYBOARD_SCAN6_PIN);
    HW_Set_IO_High(KEYBOARD_SCAN7_PORT, KEYBOARD_SCAN7_PIN);
    
    //HW_Set_IO_InputHigh(KEYBOARD_DATA0_PORT, KEYBOARD_DATA0_PIN);
    HW_Set_IO_InputHigh(KEYBOARD_DATA1_PORT, KEYBOARD_DATA1_PIN);
    HW_Set_IO_InputHigh(KEYBOARD_DATA2_PORT, KEYBOARD_DATA2_PIN);
    HW_Set_IO_InputHigh(KEYBOARD_DATA3_PORT, KEYBOARD_DATA3_PIN);
    HW_Set_IO_InputHigh(KEYBOARD_DATA4_PORT, KEYBOARD_DATA4_PIN);
    HW_Set_IO_InputHigh(KEYBOARD_DATA5_PORT, KEYBOARD_DATA5_PIN);
    HW_Set_IO_InputHigh(KEYBOARD_DATA6_PORT, KEYBOARD_DATA6_PIN);
    HW_Set_IO_InputHigh(KEYBOARD_DATA7_PORT, KEYBOARD_DATA7_PIN);
}

void Keyboard_Hw_ScanOut(UCHAR by_Index)
{
    switch(by_Index)
    {
    case 0:
        {
            HW_Set_IO_High(KEYBOARD_SCAN7_PORT, KEYBOARD_SCAN7_PIN);            //把扫描口置高
            HW_Set_IO_OutputLow(KEYBOARD_SCAN0_PORT, KEYBOARD_SCAN0_PIN);       //设置为输出
            HW_Set_IO_Low(KEYBOARD_SCAN0_PORT, KEYBOARD_SCAN0_PIN);             //置输出口为低
            break;
        }
    case 1:
        {
            HW_Set_IO_High(KEYBOARD_SCAN0_PORT, KEYBOARD_SCAN0_PIN);
            HW_Set_IO_OutputLow(KEYBOARD_SCAN1_PORT, KEYBOARD_SCAN1_PIN);
            HW_Set_IO_Low(KEYBOARD_SCAN1_PORT, KEYBOARD_SCAN1_PIN);
            break;
        }
    case 2:
        {
            HW_Set_IO_High(KEYBOARD_SCAN1_PORT, KEYBOARD_SCAN1_PIN);
            HW_Set_IO_OutputLow(KEYBOARD_SCAN2_PORT, KEYBOARD_SCAN2_PIN);
            HW_Set_IO_Low(KEYBOARD_SCAN2_PORT, KEYBOARD_SCAN2_PIN);
            break;
        }
    case 3:
        {
            HW_Set_IO_High(KEYBOARD_SCAN2_PORT, KEYBOARD_SCAN2_PIN);
            HW_Set_IO_OutputLow(KEYBOARD_SCAN3_PORT, KEYBOARD_SCAN3_PIN);
            HW_Set_IO_Low(KEYBOARD_SCAN3_PORT, KEYBOARD_SCAN3_PIN);
            break;
        }
    case 4:
        {
            HW_Set_IO_High(KEYBOARD_SCAN3_PORT, KEYBOARD_SCAN3_PIN);
            HW_Set_IO_OutputLow(KEYBOARD_SCAN4_PORT, KEYBOARD_SCAN4_PIN);
            HW_Set_IO_Low(KEYBOARD_SCAN4_PORT, KEYBOARD_SCAN4_PIN);
            break;
        }
    case 5:
        {
            HW_Set_IO_High(KEYBOARD_SCAN4_PORT, KEYBOARD_SCAN4_PIN);
            HW_Set_IO_OutputLow(KEYBOARD_SCAN5_PORT, KEYBOARD_SCAN5_PIN);
            HW_Set_IO_Low(KEYBOARD_SCAN5_PORT, KEYBOARD_SCAN5_PIN);
            break;
        }
    case 6:
        {
            HW_Set_IO_High(KEYBOARD_SCAN5_PORT, KEYBOARD_SCAN5_PIN);
            HW_Set_IO_OutputLow(KEYBOARD_SCAN6_PORT, KEYBOARD_SCAN6_PIN);
            HW_Set_IO_Low(KEYBOARD_SCAN6_PORT, KEYBOARD_SCAN6_PIN);
            break;
        }
    case 7:
        {
            HW_Set_IO_High(KEYBOARD_SCAN6_PORT, KEYBOARD_SCAN6_PIN);
            HW_Set_IO_OutputLow(KEYBOARD_SCAN7_PORT, KEYBOARD_SCAN7_PIN);
            HW_Set_IO_Low(KEYBOARD_SCAN7_PORT, KEYBOARD_SCAN7_PIN);
            break;
        }
    default:break;
    }
}

UCHAR Keyboard_Hw_ReadData(void)
{
    UCHAR by_Out = 0;
    
    //if(!HW_Get_IO(KEYBOARD_DATA0_PORT, KEYBOARD_DATA0_PIN)) BITSET(by_Out, BIT0);
    if(!HW_Get_IO(KEYBOARD_DATA1_PORT, KEYBOARD_DATA1_PIN)) BITSET(by_Out, BIT1);
    if(!HW_Get_IO(KEYBOARD_DATA2_PORT, KEYBOARD_DATA2_PIN)) BITSET(by_Out, BIT2);
    if(!HW_Get_IO(KEYBOARD_DATA3_PORT, KEYBOARD_DATA3_PIN)) BITSET(by_Out, BIT3);
    if(!HW_Get_IO(KEYBOARD_DATA4_PORT, KEYBOARD_DATA4_PIN)) BITSET(by_Out, BIT4);
    if(!HW_Get_IO(KEYBOARD_DATA5_PORT, KEYBOARD_DATA5_PIN)) BITSET(by_Out, BIT5);
    if(!HW_Get_IO(KEYBOARD_DATA6_PORT, KEYBOARD_DATA6_PIN)) BITSET(by_Out, BIT6);
    if(!HW_Get_IO(KEYBOARD_DATA7_PORT, KEYBOARD_DATA7_PIN)) BITSET(by_Out, BIT7);
    
    return (by_Out);
}

void Keyboard_Hw_Erp(void)
{
    EXTI_DeInit();
    
    HW_Set_IO_OutputLow(KEYBOARD_SCAN0_PORT, KEYBOARD_SCAN0_PIN);
    HW_Set_IO_OutputLow(KEYBOARD_SCAN1_PORT, KEYBOARD_SCAN1_PIN);	
    HW_Set_IO_OutputLow(KEYBOARD_SCAN2_PORT, KEYBOARD_SCAN2_PIN);
    HW_Set_IO_OutputLow(KEYBOARD_SCAN3_PORT, KEYBOARD_SCAN3_PIN);
    HW_Set_IO_OutputLow(KEYBOARD_SCAN4_PORT, KEYBOARD_SCAN4_PIN);
    HW_Set_IO_OutputLow(KEYBOARD_SCAN5_PORT, KEYBOARD_SCAN5_PIN);
    HW_Set_IO_OutputLow(KEYBOARD_SCAN6_PORT, KEYBOARD_SCAN6_PIN);
    HW_Set_IO_OutputLow(KEYBOARD_SCAN7_PORT, KEYBOARD_SCAN7_PIN);
    
    HW_Set_IO_Low(KEYBOARD_SCAN0_PORT, KEYBOARD_SCAN0_PIN);
    HW_Set_IO_Low(KEYBOARD_SCAN1_PORT, KEYBOARD_SCAN1_PIN);	
    HW_Set_IO_Low(KEYBOARD_SCAN2_PORT, KEYBOARD_SCAN2_PIN);
    HW_Set_IO_Low(KEYBOARD_SCAN3_PORT, KEYBOARD_SCAN3_PIN);
    HW_Set_IO_Low(KEYBOARD_SCAN4_PORT, KEYBOARD_SCAN4_PIN);
    HW_Set_IO_Low(KEYBOARD_SCAN5_PORT, KEYBOARD_SCAN5_PIN);
    HW_Set_IO_Low(KEYBOARD_SCAN6_PORT, KEYBOARD_SCAN6_PIN);
    HW_Set_IO_Low(KEYBOARD_SCAN7_PORT, KEYBOARD_SCAN7_PIN);
    
    //HW_Set_IO_InputFloat(KEYBOARD_DATA0_PORT, KEYBOARD_DATA0_PIN);
    HW_Set_IO_InputFloat(KEYBOARD_DATA1_PORT, KEYBOARD_DATA1_PIN);
    HW_Set_IO_InputFloat(KEYBOARD_DATA2_PORT, KEYBOARD_DATA2_PIN);
    HW_Set_IO_InputFloat(KEYBOARD_DATA3_PORT, KEYBOARD_DATA3_PIN);
    HW_Set_IO_InputFloat(KEYBOARD_DATA4_PORT, KEYBOARD_DATA4_PIN);
    HW_Set_IO_InputFloat(KEYBOARD_DATA5_PORT, KEYBOARD_DATA5_PIN);
    HW_Set_IO_InputFloat(KEYBOARD_DATA6_PORT, KEYBOARD_DATA6_PIN);
    HW_Set_IO_InputFloat(KEYBOARD_DATA7_PORT, KEYBOARD_DATA7_PIN);
    
    /* 定义PB.7,为外部中断7,输入通道（EXIT7） */
    //GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource7);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource8);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource9);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource10);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource11);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource12);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource13);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource14);
    
    //EXTI_GenerateSWInterrupt(EXTI_Line7);
    EXTI_GenerateSWInterrupt(EXTI_Line8);
    EXTI_GenerateSWInterrupt(EXTI_Line9);
    EXTI_GenerateSWInterrupt(EXTI_Line10);
    EXTI_GenerateSWInterrupt(EXTI_Line11);
    EXTI_GenerateSWInterrupt(EXTI_Line12);
    EXTI_GenerateSWInterrupt(EXTI_Line13);
    EXTI_GenerateSWInterrupt(EXTI_Line14);
    
    //EXTI_ClearITPendingBit(EXTI_Line7);
    EXTI_ClearITPendingBit(EXTI_Line8);
    EXTI_ClearITPendingBit(EXTI_Line9);
    EXTI_ClearITPendingBit(EXTI_Line10);
    EXTI_ClearITPendingBit(EXTI_Line11);
    EXTI_ClearITPendingBit(EXTI_Line12);
    EXTI_ClearITPendingBit(EXTI_Line13);
    EXTI_ClearITPendingBit(EXTI_Line14);
    
    
    
    EXTI_InitTypeDef EXTI_InitS;
    //打开第7通道外部中断
//    EXTI_InitS.EXTI_Line    = EXTI_Line7;
//    EXTI_InitS.EXTI_Mode    = EXTI_Mode_Interrupt;
//    EXTI_InitS.EXTI_Trigger = EXTI_Trigger_Rising;
//    EXTI_InitS.EXTI_LineCmd = ENABLE;
//    EXTI_Init(&EXTI_InitS);
    
    EXTI_InitS.EXTI_Line    = EXTI_Line8;
    EXTI_InitS.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitS.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitS.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitS);
    
    EXTI_InitS.EXTI_Line    = EXTI_Line9;
    EXTI_InitS.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitS.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitS.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitS);
    
    EXTI_InitS.EXTI_Line    = EXTI_Line10;
    EXTI_InitS.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitS.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitS.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitS);
    
    EXTI_InitS.EXTI_Line    = EXTI_Line11;
    EXTI_InitS.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitS.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitS.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitS);
    
    EXTI_InitS.EXTI_Line    = EXTI_Line12;
    EXTI_InitS.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitS.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitS.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitS);
    
    EXTI_InitS.EXTI_Line    = EXTI_Line13;
    EXTI_InitS.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitS.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitS.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitS);
    
    EXTI_InitS.EXTI_Line    = EXTI_Line14;
    EXTI_InitS.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitS.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitS.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitS);
    
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    PWR_WakeUpPinCmd(ENABLE);
}




