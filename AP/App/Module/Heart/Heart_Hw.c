#include "Heart_Hw.h"


#define UART_TX_PORT		    GPIOC
#define UART_TX_PIN		        BIT12

#define UART_RX_PORT		    GPIOD
#define UART_RX_PIN		        BIT2

#define UART_IRQN               UART5_IRQn
#define UART_PORT               UART5
#define UART_BAUD               9600

#define HR_PORT                 GPIOB
#define HR_PIN                  BIT8


void Heart_Hw_Init(void)
{
    HW_Set_IO_InputFloat(HR_PORT, HR_PIN);
    HW_Set_IO_Input(UART_RX_PORT, UART_RX_PIN);                                 //当用普通IO口时,UART RX口也设为输入
    HW_Set_IO_Input(UART_TX_PORT, UART_TX_PIN);
    
    EXTI_InitTypeDef    EXTI_InitStructure;
    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource8);
    EXTI_InitStructure.EXTI_Line    = EXTI_Line8;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    /* Generate software interrupt: simulate a falling edge applied on EXTI line 8,9 */
    //EXTI_GenerateSWInterrupt(EXTI_Line8);
    
    
    NVIC_InitTypeDef    NVIC_InitStructure;
    
    /* Enable the EXTI9_5 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void Heart_Hw_Erp(void)
{
    HW_Set_IO_InputFloat(UART_RX_PORT, UART_RX_PIN);                            //当用普通IO口时,UART RX口也设为输入
    HW_Set_IO_InputFloat(UART_TX_PORT, UART_TX_PIN);
    HW_Set_IO_InputFloat(HR_PORT, HR_PIN);
    
    EXTI_InitTypeDef    EXTI_InitStructure;
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource8);
    EXTI_InitStructure.EXTI_Line    = EXTI_Line8;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = DISABLE;
    EXTI_Init(&EXTI_InitStructure);
    /* Generate software interrupt: simulate a falling edge applied on EXTI line 8,9 */
    EXTI_GenerateSWInterrupt(EXTI_Line8);
}





