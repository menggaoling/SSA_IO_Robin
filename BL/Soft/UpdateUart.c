/*******************************************************************************
20130321 修改for BF535 project
20130521 新增MCU_SendReboot/MCU_ReturnReboot command
         新增Sys_Reboot變數供延遲Restart使用
******************** (C) COPYRIGHT 2007 STMicroelectronics ********************/
//******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
//* File Name          : UpdateUart.c
//* Author             : MCD Application Team
//* Version            : V0.1
//* Date               : 05/21/2013
//* Description        : Used bootstrap loader privides a method to program the
//*                      flash memory.
//********************************************************************************

/* Includes ------------------------------------------------------------------*/
#include "bootloader.h"
#include "MyType.h"
//#include "MyStruct.h"
#include "stm32f10x_lib.h"
//#include "main.h"
#include "FlashUtility.h"
#include "UpdateUart.h"
#include  "stm32f10x_lib.h"


#ifdef  _Debug_BSL_
//#include "debuguart5.h"
#endif

#define AP_CMD    


void Delay1ms(u16 Delay1ms);

/*----------   Private define  -----------------------------------------------*/
static struct
{
    unsigned RX_EXTEND     : 1;        //==>接收資料擴展旗標
    unsigned RX_ERROR      : 1;        //==>接收資料錯誤旗標
    unsigned RX_FINISH     : 1;        //==>接收資料結東旗標
} UPDFLAG;

const U8 CrcTab[16]    = {0x00, 0x31, 0x62, 0x53, 0xC4, 0xF5, 0xA6, 0x97, 0xB9, 0x88, 0xDB, 0xEA, 0x7D, 0x4C, 0x1F, 0x2E};
const U8 FlashMark[16] = {0xA5, 0x5A, 0xAA, 0x55, 0x57, 0x13, 0x68, 0x24, 0xF0, 0xAC, 0xDE, 0x9B, 0x00, 0xFD, 0xF0, 0x0F};

/*---------- Private variables -----------------------------------------------*/

U8  UPD_RxData[UPD_RX_BUF_SIZE];
U8  UPD_TxData[UPD_TX_BUF_SIZE];
U16 UPD_REC_SIZE;

//TED

U16 APReboot_TIME;
#ifdef AP_CMD
U8  AP_RxData[11];
U8  AP_RX_OK;     
U16  AP_RX_TimeOut;
#endif
//END TED


U8  UPDate_Mode = 0x00;
U8  Erase_Count;
U8  Mark_Count;
U8  Sys_Reboot;
/*----------  Extern Variable  -----------------------------------------------*/
extern  U16 DelayTimeForEnterUserMode;

//*****************************************************************************
// Function Name  : UPD_UARTInit
// Description    :
// Input          : None
// Output         : None
// Return         : None
//*****************************************************************************
void UPD_UARTInit(void)
{
    USART_DeInit(UPD_PORT);
    UPDate_Mode        = 0x00;
    Sys_Reboot         = 0x00;
    UPDFLAG.RX_EXTEND  = 0;
    UPDFLAG.RX_ERROR   = 0;
    UPDFLAG.RX_FINISH  = 0;
  
    
    APReboot_TIME = 5000;//TED 5SEC
#ifdef AP_CMD    
    AP_RX_OK  = 0;      //TED
    AP_RX_TimeOut  = 50;//TED
#endif   
}


//*****************************************************************************
// Function Name  : UPD_UARTGpioConfig
// Description    :
// Input          : None
// Output         : None
// Return         : None
//*****************************************************************************
void UPD_UARTGpioConfig(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;

    /* Configure USART1 Tx (PA.9) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin   = UPD_UART_TX_GPIO_Pin;       // GPIO_Pin_9
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(UPD_UART_TX_GPIO, &GPIO_InitStructure);           // GPIOA

    /* Configure USART1 Rx (PA.10) as input floating */
    GPIO_InitStructure.GPIO_Pin   = UPD_UART_RX_GPIO_Pin;       // GPIO_Pin_10
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(UPD_UART_RX_GPIO, &GPIO_InitStructure);           // GPIOA
}


//*****************************************************************************
// Function Name  : UPD_SetBaudRate
// Description    :
// Input          : None
// Output         : None
// Return         : None
//*****************************************************************************
void UPD_SetBaudRate(void)
{
    USART_InitTypeDef   USART_InitStructure;

    /* USART resources configuration (Clock, GPIO pins and USART registers) ----*/
    /* USART configured as follow:
          - BaudRate = 115200 baud
          - Word Length = 8 Bits
          - One Stop Bit
          - No parity
          - Hardware flow control disabled (RTS and CTS signals)
          - Receive and transmit enabled
    */
    /* Disable USART */
    USART_Cmd(UPD_PORT, DISABLE);
    USART_ITConfig(UPD_PORT, USART_IT_TXE,  DISABLE);
    USART_ITConfig(UPD_PORT, USART_IT_RXNE, DISABLE);

    USART_InitStructure.USART_BaudRate   = UPD_UART_BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits   = USART_StopBits_1;
    USART_InitStructure.USART_Parity     = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_Clock      = USART_Clock_Disable;
    USART_InitStructure.USART_CPOL       = USART_CPOL_Low;
    USART_InitStructure.USART_CPHA       = USART_CPHA_2Edge;
    USART_InitStructure.USART_LastBit    = USART_LastBit_Enable;

    /* USART configuration */
    USART_Init(UPD_PORT, &USART_InitStructure);

    /* Enable USART */
    USART_Cmd(UPD_PORT, ENABLE);
}


//*****************************************************************************
// Function Name  : GetByte
// Description    :
// Input          : None
// Output         : None
// Return         : None
//*****************************************************************************
#if     0
U8  GetByte(U8 *RX_DAT)
{
    if ( USART_GetFlagStatus(UPD_PORT, USART_FLAG_RXNE) != RESET)
    {
        *RX_DAT = (U8) UPD_PORT->DR;
        return  DTrue;
    }
    else
    {
        return  DFalse;
    }
}
#else
U8  GetByte(USART_TypeDef  *USARTx)
{
    return  (U8) USARTx->DR;
}
#endif

//*****************************************************************************
// Function Name  : SendByte
// Description    :
// Input          : None
// Output         : None
// Return         : None
//*****************************************************************************
void  SendByte(U8 TX_DAT)
{
    USART_SendData(UPD_PORT, TX_DAT);
    while (USART_GetFlagStatus(UPD_PORT, USART_FLAG_TXE) == RESET)
        //while (USART_GetFlagStatus(UPD_PORT, USART_FLAG_TC) == RESET )
    {
        ;
    }
}


/*******************************************************************************
* Function Name  : GenerateSystemColdReset
* Description    : Generate System ColdReset
* Input          : None
* Output         : None
* Return         :
*******************************************************************************/
void  GenerateSystemColdReset(void)
{
#if     0
    SysTick_ITConfig(DISABLE);
    GPIO_ResetBits(GPIOD, GPIO_Pin_4);  // DResetArm_Active
    Delay1ms(200);
    GPIO_ResetBits(GPIOD, GPIO_Pin_6);  // IO_Clear_C_SAFE_EN();
    GPIO_ResetBits(GPIOE, GPIO_Pin_4);  // IO_Clear_E_STOP_EN();
    GPIO_ResetBits(GPIOB, GPIO_Pin_10); // IO_Clear_HR_EN();
    GPIO_ResetBits(GPIOA, GPIO_Pin_8 ); // IO_Clear_9V_EN();
    GPIO_ResetBits(GPIOE, GPIO_Pin_15); // IO_Clear_5V_EN();
    GPIO_ResetBits(GPIOC, GPIO_Pin_6);  // IO_Clear_3V3_IO_EN();
    GPIO_ResetBits(GPIOC, GPIO_Pin_13); // IO_Clear_DDR3_VTT_EN();
    GPIO_ResetBits(GPIOB, GPIO_Pin_2);  // IO_Clear_System_Power();
    GPIO_SetBits(GPIOA, GPIO_Pin_1);    // IO_Set_PMIC_HIGH();
    //    GPIO_ResetBits(GPIOA, GPIO_Pin_1);  // IO_Set_TV_VA_Power();
    GPIO_SetBits(GPIOD, GPIO_Pin_4);  // DResetArm_Active
    Delay1ms(200);
    Delay1ms(200);
    Delay1ms(100);
    NVIC_GenerateSystemReset();
#else
    GPIO_ResetBits(GPIOD, GPIO_Pin_4);  // DResetArm_Active
    Delay1ms(100);
    IWDG->KR  = 0xCCCC;
    IWDG->KR  = 0x5555;
    IWDG->PR  = 0x00;
    IWDG->RLR = 0x001;
#endif
}


/*******************************************************************************
* Function Name  : Delay1ms(u16 Delay1ms)
* Description    : Delay1ms
* Input          : Delay time per 1ms
* Output         : None
* Return         :
*******************************************************************************/
extern Delay_4NOP();
void Delay1ms(u16 Delay1ms)
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
* Function Name  : u8 Digital16_CRC8(u8 *ptr,u16 DATALENGTH)
* Description    : 數位通訊 CRC code
* Input          : *ptr ; DATALENGTH
* Output         : None
* Return         : CHECKDATA
*******************************************************************************/
U8  Digital16_CRC8(u8 *ptr, u16 DATALENGTH)
{
    U16 crcheckcnt;
    U8  crchecktemp, crchalf, CHECKDATA = 0;
    U8  preCHECKDATA;

    for(crcheckcnt = 0; crcheckcnt < DATALENGTH; crcheckcnt ++)
    {
        crchecktemp = *(ptr + crcheckcnt);
        crchalf     = (CHECKDATA / 16);
        CHECKDATA <<= 4;
        CHECKDATA  ^= CrcTab[crchalf ^ ( crchecktemp / 16)];
        crchalf     = (CHECKDATA / 16);
        CHECKDATA <<= 4;
        CHECKDATA  ^= CrcTab[crchalf ^ ( crchecktemp & 0x0f)];
#if 0
        if ((crcheckcnt % 16) == 0)
            printf("\n");

        if (CHECKDATA == 0x6c && preCHECKDATA == 0x2b)
        {
            printf("@@@@@@@@@@@@@@@@0x%02x ", *(ptr + crcheckcnt));
        }

        printf("0x%02x ", CHECKDATA);

        preCHECKDATA = CHECKDATA;
#endif
    }
    return CHECKDATA;
}


//CRC16-CCITT algorithm
U16 Com_Cal_CRC(U8 *pby_Buff, U16 wSize)
{
    U16 crc = 0xFFFF;
    
    if(pby_Buff && wSize)
    {
        while (wSize--)
        {
            crc  = (crc >> 8) | (crc << 8);
            crc ^= *pby_Buff++;
            crc ^= ((U8) crc) >> 4;
            crc ^= crc << 12;
            crc ^= (crc & 0xFF) << 5;
        }
    }
    
    return crc;
}




//*****************************************************************************
// Function Name  : SendUPDCommandData
// Description    :
// Input          : None
// Output         : None
// Return         : None
//*****************************************************************************
void  SendUPDCommandData(U16 MAX_LEN)
{
    U16     COUNT;
    U8      TX_DATA;
#ifdef  _Debug_BSL_
    //puts("\n> UPD_TxData: ");
#endif
    for ( COUNT = 0x00; COUNT < MAX_LEN; COUNT++ )
    {
        TX_DATA = UPD_TxData[COUNT];
        if (( COUNT != 0 ) && ( COUNT != (MAX_LEN - 1) ))
        {
            if ((TX_DATA > 0xDF) && (TX_DATA < 0xE4))//E0~E4 separate delivery 
            {
                SendByte(0xE0);
#ifdef  _Debug_BSL_
                //vPutHEX(0xE0);
#endif
                TX_DATA = TX_DATA - 0xE0;
                SendByte(TX_DATA);
            }
            else
            {
                SendByte(TX_DATA);
            }
        }
        else
        {
            SendByte(TX_DATA);
        }
#ifdef  _Debug_BSL_
        //vPutHEX(TX_DATA);
#endif
    }
}


//*****************************************************************************
// Function Name  : GetUPDCommandData
// Description    :
// Input          : None
// Output         : None
// Return         : None
//*****************************************************************************
U8  GetUPDCommandData(void)
{
    U8      TX_Data, RX_Data;
    U16     RX_Count = 0;
#ifdef AP_CMD     
    U16     AP_RX_Count = 0;
    U8      AP_CMD_SBFG = 0;      //AP COMMAND START BIT FLAG
#endif
    if (USART_GetFlagStatus(UPD_PORT, USART_FLAG_RXNE) != RESET)
    {
        RX_Data = GetByte(UPD_PORT);
        //printf("Data:%x\r\n",RX_Data);
         
          
        if (RX_Data == UPD_DAT_START)
        {
            UPD_RxData[RX_Count] = RX_Data;
            RX_Count = 0x01;
        }
#ifdef AP_CMD         
        else if(RX_Data  == 0x55)//&& (UPDate_Mode == 0X01))
        {
          AP_RxData[AP_RX_Count] = RX_Data;
          AP_RX_Count = 1;
          AP_CMD_SBFG = 1;
        }
#endif
        else
        {
            return  DFalse;
        }
        
        
        
    }
    else
    {
        return  DFalse;
    }
    SysTick_CounterCmd(SysTick_Counter_Disable);
    DelayTimeForEnterUserMode = 0;
    SysTick_CounterCmd( SysTick_Counter_Enable );

    UPDFLAG.RX_ERROR  = 0;
    UPDFLAG.RX_EXTEND = 0;

#ifdef AP_CMD 
    //if(AP_CMD_SBFG)
        AP_RX_TimeOut = 50;//50mS TED
    //else
    //    AP_RX_TimeOut = 850;
      
#endif
    do
    {   
 #ifdef AP_CMD     //TED 
       //SysTickCount = SysTick_GetCounter();
       /* Timer 1mS process */
        if(SysTick_GetFlagStatus(SysTick_FLAG_COUNT))//&& AP_CMD_SBFG
        {
            if(AP_RX_TimeOut)
                    AP_RX_TimeOut--;
            else
            {
              AP_CMD_SBFG = 0;
              AP_RX_TimeOut = 0;//Time out leave 55 aa XX 指令 
            }
                
        //SysTick_SetReload(System_CLK.SYSCLK_Frequency/1000); //1uS:1000000;1ms:1000
                     
             SysTick_CounterCmd(SysTick_Counter_Clear); /* Enable the SysTick Counter */
        }
#endif      
        if (USART_GetFlagStatus(UPD_PORT, USART_FLAG_RXNE) != RESET)
        {
          
            RX_Data = GetByte(UPD_PORT);
            //puts("\n> Data: ");
            //vPutHEX(RX_Data);
            if (RX_Count < UPD_RX_BUF_SIZE)
            {
             
 #ifdef AP_CMD              
//Ted Create Reboot Cmd 55 AA 01 E7 00 02 00 00 00 XX XX Checksum = xxxx
               if(AP_CMD_SBFG)
               {
 
                   
                  if(AP_RX_Count == 1)
                 {
                     if(RX_Data == 0xAA)
                     {
                       AP_RxData[AP_RX_Count++] = RX_Data;
                       
                     }else
                     {
                       AP_RX_Count = 0;
                       AP_RX_TimeOut = 0;
                       AP_CMD_SBFG = 0;
                     }
                  }
                  else
                  {
                 
                    AP_RxData[AP_RX_Count++] = RX_Data;
                    
                    switch(AP_RX_Count)
                    {
                     
                      case 3://Write/Read
                          if(AP_RxData[2] != 0x01)
                          {
                            AP_RX_Count = 0;
                            AP_RX_TimeOut = 0;
                            AP_CMD_SBFG = 0;
                          }
                        break;
                       case 4://Cmd
                          if(AP_RxData[3] != 0xE7)
                          {
                            AP_RX_Count = 0;
                            AP_RX_TimeOut =0;
                            AP_CMD_SBFG = 0;
                          }
                        break;
                      case 6://Len
                           if(AP_RxData[5] != 0x02)
                          {
                            AP_RX_Count = 0;
                            AP_RX_TimeOut = 0;
                            AP_CMD_SBFG = 0;
                          }
                        break;
                      case 9://Data
                          APReboot_TIME = AP_RxData[7];        //HSB
                          APReboot_TIME +=AP_RxData[8]<<8;     //LSB
                          
                          
                        break;
                      case 11://Checksum
                            AP_RX_Count = 0;
                            AP_CMD_SBFG = 0;
                            AP_RX_OK = 1;
                            UPDFLAG.RX_FINISH = 1;
                            
                       break;
                    } 
                    
                    
                  } 
           
               }
               else
               {
#endif                

                        
  //END TED            
                AP_RX_TimeOut += 2;
                     
                if (UPDFLAG.RX_EXTEND == 0)
                {
                  
#if 1
                    if (RX_Data == UPD_DAT_EXTEND)  //Cmd =0xE0
					{
					   	UPDFLAG.RX_EXTEND = 1;
					}
                    else
#endif
                    {
#if 0
                        if(RX_Data == 0x0a)
                        {
                            RX_Count--;
                            UPD_RxData[RX_Count] = RX_Data;
                            RX_Count++;
                        }
                        else
#endif
                        //{
                          
                            UPD_RxData[RX_Count] = RX_Data;
                            RX_Count++;
                            if (RX_Data == UPD_DAT_END)   //End Head = 0XE2
                            {
                                if(UPD_RxData[UPD_CMD_INDEX] == MCU_WriteFlash)       //Index 01 value(Cmd) = 0X12 write flash
                                {
                                    //if(RX_Count == (PageSize + MCU_WriteFlashHeader))
                                    UPDFLAG.RX_FINISH = 1;
                                    //else
                                    //UPDFLAG.RX_FINISH = 0;
                                }
                                else
                                {
                                    UPDFLAG.RX_FINISH = 1;
                                }
                            }
                        //}
                    }
                }
#if 1
                else//UPDFLAG.RX_EXTEND == 1 execute
                {
                    UPD_RxData[RX_Count] = UPD_DAT_EXTEND + RX_Data;
                    UPDFLAG.RX_EXTEND = 0;
                    RX_Count++;
                }
#endif
               
#ifdef AP_CMD               
           }//ENDTED
#endif              
            }//Over Rx size
            else
            {
                UPDFLAG.RX_ERROR = 1;
            }
            DelayTimeForEnterUserMode = 0;
        }
 
#ifdef AP_CMD 
      if(AP_RX_TimeOut == 0)
        DelayTimeForEnterUserMode = 1001;
#endif
 
    }
    while ( (!UPDFLAG.RX_FINISH) && (DelayTimeForEnterUserMode < 1000) );

    if (UPDFLAG.RX_FINISH == 1)
    {
#ifdef  _Debug_BSL_
        U16     i;

        //puts("\n> UPD_RxData: ");
        //for (i = 0; i < RX_Count; i++)
        //{
        //    vPutHEX(UPD_RxData[i]);
        //}
#endif
        UPD_REC_SIZE = RX_Count;
        //puts("\n> RX_Count: ");
        //vPut2HEX(UPD_REC_SIZE);
        return  DTrue;
    }
    UPD_REC_SIZE = 0;
    return  DFalse;
}


//*****************************************************************************
// Function Name  : UPD_CheckAndPackageCommand
// Description    :
// Input          : None
// Output         : None
// Return         : None
//*****************************************************************************
void  UPD_CheckAndPackageCommand( void )
{
    union
    {
        unsigned int DWORD;     //0x?? ?? ?? ??
        char BYTE[4];
    } byData;
    
    U16     TX_Count = 0;
    U32     RX_ADDR;
    U32     RX_LEN;
    U8      i;
    //U16     W_CRC;

    
    if (UPDFLAG.RX_FINISH == 1)
    {
 #ifdef AP_CMD      
//TED
       //Response 55 AA 01 E7 00 00 00 XX XX
      if(AP_RX_OK)
      {
        AP_RX_OK = 0;
        TX_Count = 0;
        
//       APReboot_TIME = 
        UPD_TxData[TX_Count++] =0x55;
        UPD_TxData[TX_Count++] =0xAA;
        UPD_TxData[TX_Count++] =0x01;
        UPD_TxData[TX_Count++] =0xE7;
        UPD_TxData[TX_Count++] =0x00;
        UPD_TxData[TX_Count++] =0x00; 
        UPD_TxData[TX_Count++] =0x00;
        U16 checksum = Com_Cal_CRC(UPD_TxData, TX_Count);
        UPD_TxData[TX_Count++] =checksum;     //LSB
        UPD_TxData[TX_Count++] =checksum>>8;  //HSB
  
        Sys_Reboot = 0xAA;
        SendUPDCommandData(TX_Count);
        UPDFLAG.RX_FINISH = 0; 
         return;
          
      }
#endif       
        
//END TED
       
        TX_Count = UPD_HEAD_INDEX;
        UPD_TxData[TX_Count++] = UPD_DAT_START;
        switch(UPD_RxData[UPD_CMD_INDEX])
        {
        case    MCU_EraseFlash:
            UPD_TxData[TX_Count++] = MCU_ReturnEraseFlash;
            byData.BYTE[0] = UPD_RxData[UPD_ADR0_INDEX];
            byData.BYTE[1] = UPD_RxData[UPD_ADR1_INDEX];
            byData.BYTE[2] = UPD_RxData[UPD_ADR2_INDEX];
            byData.BYTE[3] = UPD_RxData[UPD_ADR3_INDEX];
            RX_ADDR = byData.DWORD;
            byData.BYTE[0] = UPD_RxData[UPD_LEN0_INDEX];
            byData.BYTE[1] = UPD_RxData[UPD_LEN1_INDEX];
            byData.BYTE[2] = UPD_RxData[UPD_LEN2_INDEX];
            byData.BYTE[3] = UPD_RxData[UPD_LEN3_INDEX];
            RX_LEN = byData.DWORD;
#ifdef  _Debug_BSL_
            puts("\n> Erase_ADDR: ");
            vPut4HEX(RX_ADDR);
            puts("\n> Erase_LEN: ");
            vPut4HEX(RX_LEN);
#endif

#if 1
            if (FlashErase(RX_ADDR, RX_LEN) == DTrue)
            {
                UPD_TxData[TX_Count++] = 0x01;
            }
            else
            {
                UPD_TxData[TX_Count++] = 0x00;
            }
#else
            UPD_TxData[TX_Count++] = 0x01;
#endif
            UPDFLAG.RX_FINISH = 0;
            break;

        case    MCU_WriteFlash:
            UPD_TxData[TX_Count++] = MCU_ReturnWriteFlash;
            byData.BYTE[0] = UPD_RxData[UPD_ADR0_INDEX];
            byData.BYTE[1] = UPD_RxData[UPD_ADR1_INDEX];
            byData.BYTE[2] = UPD_RxData[UPD_ADR2_INDEX];
            byData.BYTE[3] = UPD_RxData[UPD_ADR3_INDEX];
            UPD_TxData[TX_Count++] = UPD_RxData[UPD_ADR0_INDEX];
            UPD_TxData[TX_Count++] = UPD_RxData[UPD_ADR1_INDEX];
            UPD_TxData[TX_Count++] = UPD_RxData[UPD_ADR2_INDEX];
            UPD_TxData[TX_Count++] = UPD_RxData[UPD_ADR3_INDEX];
            RX_ADDR = byData.DWORD;

            if (RX_ADDR == 0x8004000)
            {
              #ifdef  _Debug_BSL_
                puts("\n ==============================================");
              #endif
            }

            RX_LEN = (U32)(UPD_REC_SIZE - MCU_WriteFlashHeader);
            byData.DWORD = RX_LEN;
            UPD_TxData[TX_Count++] = byData.BYTE[0];
            UPD_TxData[TX_Count++] = byData.BYTE[1];
            UPD_TxData[TX_Count++] = byData.BYTE[2];
            UPD_TxData[TX_Count++] = byData.BYTE[3];

            RX_LEN = byData.DWORD;
#ifdef   _Debug_BSL_
            puts("\n> Write_ADDR: ");
            vPut4HEX(RX_ADDR);
            puts("\n> Write_LEN: ");
            vPut4HEX(RX_LEN);
#endif

#if 1
            if(FlashWrite(RX_ADDR, RX_LEN, &UPD_RxData[UPD_ADR3_INDEX + 1]) == DTrue)
            {
               #ifdef _Debug_BSL_
                puts("\n> FlashWrite: success");
               #endif 
            }
            else
            {
               #ifdef _Debug_BSL_
                puts("\n> FlashWrite: fail");
               #endif
            }
#endif
            UPD_TxData[TX_Count++] = Digital16_CRC8((U8 *)RX_ADDR, (U16) RX_LEN);

            UPDFLAG.RX_FINISH = 0;
            break;

        case    MCU_ReadFlash:
            UPD_TxData[TX_Count++] = MCU_ReturnReadFlash;
            byData.BYTE[0] = UPD_RxData[UPD_ADR0_INDEX];
            byData.BYTE[1] = UPD_RxData[UPD_ADR1_INDEX];
            byData.BYTE[2] = UPD_RxData[UPD_ADR2_INDEX];
            byData.BYTE[3] = UPD_RxData[UPD_ADR3_INDEX];
            UPD_TxData[TX_Count++] = UPD_RxData[UPD_ADR0_INDEX];
            UPD_TxData[TX_Count++] = UPD_RxData[UPD_ADR1_INDEX];
            UPD_TxData[TX_Count++] = UPD_RxData[UPD_ADR2_INDEX];
            UPD_TxData[TX_Count++] = UPD_RxData[UPD_ADR3_INDEX];
            RX_ADDR = byData.DWORD;
            byData.BYTE[0] = UPD_RxData[UPD_LEN0_INDEX];
            byData.BYTE[1] = UPD_RxData[UPD_LEN1_INDEX];
            byData.BYTE[2] = 0x00;
            byData.BYTE[3] = 0x00;
            RX_LEN = byData.DWORD;
#ifdef  _Debug_BSL_
            puts("\n> Read_ADDR: ");
            vPut4HEX(RX_ADDR);
            puts("\n> Read_LEN: ");
            vPut4HEX(RX_LEN);
#endif
            FlashRead(RX_ADDR, (U16) RX_LEN, &UPD_TxData[TX_Count]);
            TX_Count  = RX_LEN + 6;
#ifdef  _Debug_BSL_
            puts("\n> TX_Count: ");
            vPut2HEX(TX_Count);
#endif
            UPDFLAG.RX_FINISH = 0;
            break;

        case    MCU_ReadProgramState:
            UPD_TxData[TX_Count++] = MCU_ReturnProgramState;
            UPD_TxData[TX_Count++] = UPDate_Mode;
            UPDFLAG.RX_FINISH = 0;
            break;

        case    MCU_FlashUnlock:
            UPD_TxData[TX_Count++] = MCU_ReturnFlashState;
            ProgramLock = 0x00;
            for (i = 0; i < 7; i++)
            {
                if (UPD_RxData[i + 2] != CU8_BslMessage[i + 12])
                {
                    ProgramLock = 0x01;
                }
            }
            if (ProgramLock == 0x00)
            {
                // Clear Flash Erase Status
                EnterFlashProgramming();
            }
            UPD_TxData[TX_Count++] = !(ProgramLock);
            UPDFLAG.RX_FINISH = 0;
            break;

        case    MCU_WriteCheckCode:
            UPD_TxData[TX_Count++] = MCU_ReturnCheckCode;
            FlashWrite(DFlashMarkAddress, 0x10, &UPD_RxData[UPD_CMD_INDEX + 1]);
            FlashRead(DFlashMarkAddress, 0x10, &UPD_TxData[TX_Count]);
            Erase_Count = 0x00;
            Mark_Count  = 0x00;
            for (i = 0; i < DFlashMarkSize; i++)
            {
                if (UPD_TxData[i + 2] == 0xFF)
                {
                    Erase_Count++;
                }
                else if (UPD_TxData[i + 2] == FlashMark[i])
                {
                    Mark_Count++;
                }
            }
            if (Erase_Count == (DFlashMarkSize - 1))
            {
                UPDate_Mode = 0xFF;
            }
#ifdef  _Debug_BSL_
            else
            {
                puts("\n> Erase_Count: ");
                //vPutHEX(Erase_Count);
            }
#endif
            if (Mark_Count == (DFlashMarkSize - 1))
            {
                UPDate_Mode = 0x01;
            }
#ifdef  _Debug_BSL_
            else
            {
                puts("\n> Mark_Count: ");
                //vPutHEX(Mark_Count);
                for (i = 0; i < DFlashMarkSize; i++)
                {

                }
            }
#endif
            TX_Count  = 0x10 + 2;
            UPDFLAG.RX_FINISH = 0;
            break;
#if     0
        case    MCU_ReadProduceID:
            UPD_TxData[TX_Count++] = MCU_ReturnProduceID;
            UPD_TxData[TX_Count++] = CU8_BslMessage[11];
            UPD_TxData[TX_Count++] = CU8_BslMessage[10];
            UPDFLAG.RX_FINISH = 0;
            break;

        case    MCU_ReadMcuType:
            UPD_TxData[TX_Count++] = MCU_ReturnMcuType;
            UPD_TxData[TX_Count++] = CU8_BslMessage[1];
            UPD_TxData[TX_Count++] = CU8_BslMessage[0];
            UPDFLAG.RX_FINISH = 0;
            break;

        case    MCU_ReadMcuID:
            UPD_TxData[TX_Count++] = MCU_ReturnMcuID;
            UPD_TxData[TX_Count++] = CU8_BslMessage[3];
            UPD_TxData[TX_Count++] = CU8_BslMessage[2];
            UPDFLAG.RX_FINISH = 0;
            break;
#else
        case    MCU_ReadBSLInfo:
            UPD_TxData[TX_Count++] = MCU_ReturnBSLInfo;
            for (i = 0; i < 20; i++)
            {
                UPD_TxData[TX_Count++] = CU8_BslMessage[i];
            }
            UPDFLAG.RX_FINISH = 0;
            break;

        case    MCU_ReadAPVersion:
            UPD_TxData[TX_Count++] = MCU_ReturnAPVersion;
            FlashRead(DAPVersionAddress, 0x10, &UPD_TxData[TX_Count]);
            TX_Count  = 0x10 + 2;
            UPDFLAG.RX_FINISH = 0;
            break;
#endif
        case    MCU_ReadUpdateMode:
            UPD_TxData[TX_Count++] = MCU_ReturnUpdateMode;
            UPD_TxData[TX_Count++] = 0x01;
            UPDFLAG.RX_FINISH = 0;
            break;
#if     0
        case    MCU_IsRequestUpdate:
            UPD_TxData[TX_Count++] = MCU_ReturnRequestUpdate;
            UPD_TxData[TX_Count++] = 0x55;
            UPD_TxData[TX_Count++] = 0xCC;
            UPD_TxData[TX_Count++] = 0x00;
            UPD_TxData[TX_Count++] = 0xFF;
            Sys_Reboot         = 0xAA;
            UPDFLAG.RX_FINISH  = 0;
            break;
#else
        case    MCU_SendReboot:         // BL_R02+s,支援MCU_SendReboot命令

            //puts("\n> MCU_SendReboot");
          
            UPD_TxData[TX_Count++] = MCU_ReturnReboot;
            UPD_TxData[TX_Count++] = 0x01;
            Sys_Reboot         = 0xAA;
            UPDFLAG.RX_FINISH  = 0;
            SysTick_CounterCmd(SysTick_Counter_Disable);
            DelayTimeForEnterUserMode = 0;
            SysTick_CounterCmd( SysTick_Counter_Enable );
            break;                      // BL_R02+e
#endif
        default:
            UPD_TxData[TX_Count++] = MCU_ReturnInvalidCommand;
            UPDFLAG.RX_FINISH = 0;
            break;
        }
        
        UPD_TxData[TX_Count++] = UPD_DAT_END;
        SendUPDCommandData(TX_Count);
    }
}

