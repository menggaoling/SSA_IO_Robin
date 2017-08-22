#include "RFID.h"
#include "stm32f10x.h"
#include "Heart_Hw.h"

UCHAR rfid_rx_data_index = 0;
UCHAR rfid_rx_data_count = 0;
UCHAR rfid_rx_data[100];
UCHAR rfid_test[50];
UCHAR rfid_rx_data_clear = 0;


void RFID_Init(void)
{
    Heart_Hw_Init();
    rfid_test[0] = 0x4;
    rfid_test[1] = 0x0;
    rfid_test[2] = 0x2;
    rfid_test[3] = 0x79;
    rfid_test[4] = 0x40;

    rfid_test[5] = 0x4;
    rfid_test[6] = 0x1;
    rfid_test[7] = 0x0;
    rfid_test[8] = 0x43;
    rfid_test[9] = 0x8A;

    rfid_test[10] = 0x4;
    rfid_test[11] = 0x1;
    rfid_test[12] = 0x1;
    rfid_test[13] = 0x52;
    rfid_test[14] = 0x3;

    rfid_test[15] = 0x4;
    rfid_test[16] = 0x1;
    rfid_test[17] = 0x2;
    rfid_test[18] = 0x60;
    rfid_test[19] = 0x98;

    rfid_test[20] = 0x4;
    rfid_test[21] = 0x1;
    rfid_test[22] = 0x3;
    rfid_test[23] = 0x71;
    rfid_test[24] = 0x11;

    rfid_test[25] = 0x4;
    rfid_test[26] = 0x1;
    rfid_test[27] = 0x4;
    rfid_test[28] = 0x5;
    rfid_test[29] = 0xae;

    rfid_test[30] = 0x4;
    rfid_test[31] = 0x1;
    rfid_test[32] = 0x5;
    rfid_test[33] = 0x14;
    rfid_test[34] = 0x27;

    rfid_rx_data_clear = 0;
}

void RFID_SendCommand(char *string, UINT16 str_size)
{
    while(str_size)
    {
        USART_SendData(USART3, (u16) *string++);		        /* 傳送訊息至 USART3*/
        //while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
        while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
        {
            ;
        }
        //USART_ClearFlag(USART3,USART_FLAG_TC);
        str_size--;
    }
}

void RFID_ReceiveCommand(char *string)
{
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        // 如果收到 A 字元，則發送 ok 字串
        if((char)USART_ReceiveData(USART3) == 'A')
        {
            //BT_SendCommand("ok \n");
        }
    }
}

void RFID_TxRx_Process()
{
    if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
    {
        USART_ClearITPendingBit(USART3, USART_IT_TXE);
    }

    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
        rfid_rx_data[rfid_rx_data_index % 200] = USART_ReceiveData(USART3);
        rfid_rx_data_index++;
        rfid_rx_data_count++;
    }
}

void RFID_Erp(void)
{
    Heart_Hw_Erp();
}


