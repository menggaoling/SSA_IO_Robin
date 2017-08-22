#include "LCB_Hw.h"
#include "Hw.h"


#define UART_485DE_PORT		    GPIOD
#define UART_485DE_PIN		    BIT5

#define UART_TX_PORT		    GPIOA
#define UART_TX_PIN		        BIT2

#define UART_RX_PORT		    GPIOA
#define UART_RX_PIN		        BIT3

#define UART_IRQN               USART2_IRQn
#define UART_PORT               USART2


__no_init volatile static UINT16 w_DelayCount;
static FNCT_UCHAR FNCT_UartRx;
static FNCT_VOID FNCT_UartTx;
__no_init static UCHAR mode;

void LCB_Hw_Init(UINT32 Baud)
{
    FlagStatus              tc_status;
    GPIO_InitTypeDef        gpio_init;
    USART_InitTypeDef       usart_init;
    USART_ClockInitTypeDef  usart_clk_init;

    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    /* ----------------- SETUP UART_PORT GPIO ---------------- */
    gpio_init.GPIO_Pin   = UART_TX_PIN;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(UART_TX_PORT, &gpio_init);

    gpio_init.GPIO_Pin   = UART_RX_PIN;
    gpio_init.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(UART_RX_PORT, &gpio_init);


    /* ----------------- INIT USART STRUCT ---------------- */
    usart_init.USART_BaudRate            = Baud;
    usart_init.USART_WordLength          = USART_WordLength_8b;
    usart_init.USART_StopBits            = USART_StopBits_1;
    usart_init.USART_Parity              = USART_Parity_No ;
    usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart_init.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(UART_PORT, &usart_init);

    usart_clk_init.USART_Clock           = USART_Clock_Disable;
    usart_clk_init.USART_CPOL            = USART_CPOL_Low;
    usart_clk_init.USART_CPHA            = USART_CPHA_2Edge;
    usart_clk_init.USART_LastBit         = USART_LastBit_Disable;
    USART_ClockInit(UART_PORT, &usart_clk_init);


    USART_ITConfig(UART_PORT, USART_IT_TXE, DISABLE);      //这个是发送区为空了就中断
    USART_ITConfig(UART_PORT, USART_IT_TC,  DISABLE);      //现采用TC标志，发送完毕中断
    USART_ITConfig(UART_PORT, USART_IT_RXNE, ENABLE);      //打开接收中断

    USART_Cmd(UART_PORT, ENABLE);

    //初始清标志
    tc_status  = USART_GetFlagStatus(UART_PORT, USART_FLAG_TC);
    while (tc_status == SET)
    {
        USART_ClearITPendingBit(UART_PORT, USART_IT_TC);
        USART_ClearFlag(UART_PORT, USART_IT_TC);
        UINT16 i = 50000;
        while(i--);
        tc_status = USART_GetFlagStatus(UART_PORT, USART_FLAG_TC);
    }

    
    NVIC_InitTypeDef NVIC_InitS;
    NVIC_InitS.NVIC_IRQChannel = UART_IRQN;
    NVIC_InitS.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitS.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitS.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitS);

    HW_Set_IO_Output(UART_485DE_PORT, UART_485DE_PIN);
    LCB_Hw_RS485_SetToRx();

    FNCT_UartRx = NULL;
    FNCT_UartTx = NULL;
    w_DelayCount = 0;
    if(Baud == 9600) mode = 0;
    else mode = 1;
}

void LCB_Hw_Send_Byte(UCHAR by_Data)
{
    USART_ITConfig(UART_PORT, USART_IT_TC, ENABLE);
    USART_SendData(UART_PORT, by_Data);
}

void LCB_Hw_Set_RxFuct(FNCT_UCHAR Fnct)
{
    FNCT_UartRx = Fnct;
}

void LCB_Hw_Set_TxFuct(FNCT_VOID Fnct)
{
    FNCT_UartTx = Fnct;
}

void LCB_Hw_ISR_Handler(void)
{
    if(USART_GetITStatus(UART_PORT, USART_IT_RXNE) != RESET)
    {
        UCHAR COMRxData = (UCHAR)(USART_ReceiveData(UART_PORT) & 0xFF);
        USART_ClearITPendingBit(UART_PORT, USART_IT_RXNE);
        if(FNCT_UartRx != NULL) (*FNCT_UartRx)((UCHAR)COMRxData);
    }

    if(USART_GetITStatus(UART_PORT, USART_IT_TC) != RESET)
    {
        USART_ITConfig(UART_PORT, USART_IT_TC, DISABLE);
        USART_ClearITPendingBit(UART_PORT, USART_IT_TC);
        if(FNCT_UartTx != NULL) (*FNCT_UartTx)();
    }
}

void LCB_Hw_RS485_SetToRx(void)
{
    HW_Set_IO_Output(UART_485DE_PORT, UART_485DE_PIN);
    HW_Set_IO_Low(UART_485DE_PORT, UART_485DE_PIN);
}

void LCB_Hw_RS485_SetToTx(void)
{
    HW_Set_IO_Output(UART_485DE_PORT, UART_485DE_PIN);
    HW_Set_IO_High(UART_485DE_PORT, UART_485DE_PIN);
}

void LCB_Hw_WakeUp_LCB(void)
{
    USART_DeInit(UART_PORT);
    
    NVIC_InitTypeDef NVIC_InitS;
    NVIC_InitS.NVIC_IRQChannel = UART_IRQN;
    NVIC_InitS.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitS.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitS.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitS);
    
    LCB_Hw_RS485_SetToTx();
    
    HW_Set_IO_Output(UART_TX_PORT, UART_TX_PIN);
    HW_Set_IO_Low(UART_TX_PORT, UART_TX_PIN);
    LCB_Hw_Delay(600);                                                          //标准为500ms
    HW_Set_IO_High(UART_TX_PORT, UART_TX_PIN);
    LCB_Hw_Delay(50);
    
    LCB_Hw_Init(9600);
}

void LCB_Hw_Delay(UINT16 i) //unit:1ms
{
    w_DelayCount = i;
    while(w_DelayCount > 0);
}

void LCB_Hw_1ms_Int(void)
{
    if(w_DelayCount > 0) w_DelayCount--;
}

UCHAR LCB_Hw_Get_Rx(void)
{
//    USART_DeInit(UART_PORT);
//    
//    NVIC_InitTypeDef NVIC_InitS;
//    NVIC_InitS.NVIC_IRQChannel = UART_IRQN;
//    NVIC_InitS.NVIC_IRQChannelPreemptionPriority = 0;
//    NVIC_InitS.NVIC_IRQChannelSubPriority = 0;
//    NVIC_InitS.NVIC_IRQChannelCmd = DISABLE;
//    NVIC_Init(&NVIC_InitS);

    LCB_Hw_RS485_SetToRx();
    HW_Set_IO_Input(UART_RX_PORT, UART_RX_PIN);

    return HW_Get_IO_Filter(UART_RX_PORT, UART_RX_PIN);
}

void LCB_Hw_Erp(void)
{
    USART_DeInit(UART_PORT);
    
    NVIC_InitTypeDef NVIC_InitS;
    NVIC_InitS.NVIC_IRQChannel = UART_IRQN;
    NVIC_InitS.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitS.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitS.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitS);
    
    HW_Set_IO_InputFloat(UART_485DE_PORT, UART_485DE_PIN);
    HW_Set_IO_InputFloat(UART_TX_PORT, UART_TX_PIN);
    HW_Set_IO_InputFloat(UART_RX_PORT, UART_RX_PIN);

//    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource5);
//    EXTI_GenerateSWInterrupt(EXTI_Line5);
//    EXTI_ClearITPendingBit(EXTI_Line5);
//    
//    EXTI_InitTypeDef EXTI_InitS;
//    
//    EXTI_InitS.EXTI_Line    = EXTI_Line5;
//    EXTI_InitS.EXTI_Mode    = EXTI_Mode_Interrupt;
//    EXTI_InitS.EXTI_Trigger = EXTI_Trigger_Rising;
//    EXTI_InitS.EXTI_LineCmd = ENABLE;
//    EXTI_Init(&EXTI_InitS);
    
    //中断向量,与Keyboard 中 EXTI9_5_IRQn 用的是同一个向量.所以不需要重复打开
}


UCHAR LCB_HW_GET_MODE(void)
{
  return mode;
}


