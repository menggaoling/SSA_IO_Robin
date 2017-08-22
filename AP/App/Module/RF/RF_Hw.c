#include "RF_Hw.h"


#define UART_TX_PORT		    GPIOB
#define UART_TX_PIN		        BIT10

#define UART_RX_PORT		    GPIOB
#define UART_RX_PIN		        BIT11

#define UART_IRQN               USART3_IRQn
#define UART_PORT               USART3
#define UART_BAUD               9600


static FNCT_UCHAR FNCT_UartRx;
static FNCT_VOID FNCT_UartTx;


void RF_Hw_Init(void)
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
    usart_init.USART_BaudRate            = UART_BAUD;
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
    NVIC_InitS.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitS.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitS.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitS);

    FNCT_UartRx = NULL;
    FNCT_UartTx = NULL;
}

void RF_Hw_Send_Byte(UCHAR by_Data)
{
    USART_ITConfig(UART_PORT, USART_IT_TC, ENABLE);
    USART_SendData(UART_PORT, by_Data);
}

void RF_Hw_Set_RxFuct(FNCT_UCHAR Fnct)
{
    FNCT_UartRx = Fnct;
}

void RF_Hw_Set_TxFuct(FNCT_VOID Fnct)
{
    FNCT_UartTx = Fnct;
}

void RF_Hw_ISR_Handler(void)
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

void RF_Hw_ERP(void)
{
    NVIC_InitTypeDef        NVIC_InitStructure;
    
    NVIC_InitStructure.NVIC_IRQChannel = UART_IRQN;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    USART_DeInit(UART_PORT);
    
    HW_Set_IO_Input(UART_TX_PORT, UART_TX_PIN);
    HW_Set_IO_Input(UART_RX_PORT, UART_RX_PIN);
}


