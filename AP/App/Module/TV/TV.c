#include "TV.h"
#include "TV_Hw.h"
#include "Power.h"
#include "FIFO.h"


#define TX_START                0xF5
#define RX_START                0xF5

#define C_TX_TIMEOUT            50      //ms
#define C_RX_TIMEOUT            50      //ms
#define C_KEEP_ONLINE           500     //ms
#define C_RETX_TIME             250     //ms
#define C_RETX_TIMES            3       //重发次数
#define C_DELAY_TX              250     //ms 接收与发送之间延时
#define C_LOST_TIME             5000    //ms
#define C_FIRST_LOST_TIME       10000   //ms
#define C_FIRST_DELAY_TIME      2000    //ms 5秒后才运行模块

#define TV_MAX_LENGTH	        30
#define TV_ADR_START            0
#define TV_ADR_STATUS           1
#define TV_ADR_IRCODE           2

#define TV_STATUS_CHECKSUM      0x01
#define TV_STATUS_POWER         0x80


typedef enum{RStart, RStatu, RIRCode, RCRC} RX_STEP;


__no_init static UCHAR by_TunerType;
__no_init static UCHAR by_PowerStatus;
__no_init static UCHAR aby_RxBuff[TV_MAX_LENGTH];                               //接收数据Buff
__no_init static UCHAR aby_TxBuff[TV_MAX_LENGTH];                               //待发送数据Buff
__no_init static UCHAR aby_TxSave[TV_MAX_LENGTH];                               //存储上次发送的数据
__no_init static UCHAR by_TxSaveLength;                                         //存储上次发送的数据长度
__no_init static UCHAR by_ReTxTimes;                                            //重发次数计时
__no_init static UINT16 w_ReTxTimerCounter;                                     //重发时间计时
__no_init static UCHAR by_RxPointer;                                            //当前接收位置
__no_init static UCHAR by_TxLength;                                             //待发送数据总长度
__no_init static UINT16 w_TxPointer;                                            //当前发送位置
__no_init static UCHAR by_TxTimeOut;                                            //发送超时计时
__no_init static UCHAR by_RxTimeOut;                                            //接收超时计时
__no_init static UINT16 w_LostConnectCounter;                                   //失去连接计时
__no_init static UINT16 w_FirstDelayCounter;                                    //5秒后才运行模块
__no_init static UINT16 w_KeepOnlineTimer;                                      //保持连接计时器
__no_init static UINT16 w_DelayTx;                                              //两次发送这间需要延延时间
__no_init static RX_STEP RxStep;                                                //接收步骤
__no_init static TV_ERROR TV_Error;                                             //TV通讯产生的错误
__no_init static UCHAR aby_FIFO_Buff[64];                                       //FIFO buff
__no_init static FIFO_BASE FIFO_Base;                                           //FIFO 控制类型
__no_init static struct
{
    unsigned TxBuzy         : 1;    //发送中
    unsigned RxStart        : 1;    //开始发送
    unsigned RxOK           : 1;    //接收ok
    unsigned Connected      : 1;    //是否与上控通讯成功
} Uart;


const UCHAR TvCommandTable[40][4] =
{
    {0x00, 'P', 'O', 'N'},      // TV_CMD_POWER_ON         
    {0x60, 'P', 'O', 'F'},      // TV_CMD_POWER_OFF        
    {0x08, 'S', 'R', 'C'},      // TV_CMD_SOURCE           
    {0x40, 'M', 'U', 'T'},      // TV_CMD_MUTE             
    {0x1A, 'M', 'E', 'M'},      // TV_CMD_MENU             
    {0x1C, 'E', 'N', 'T'},      // TV_CMD_ENTER            
    {0x18, 'E', 'X', 'T'},      // TV_CMD_EXIT             
    {0x1B, 'A', 'L', 'T'},      // TV_CMD_AUDIO_DEC_LEFT   
    {0x1D, 'A', 'R', 'T'},      // TV_CMD_AUDIO_INC_RIGHT  
    {0x19, 'C', 'U', 'P'},      // TV_CMD_CHANNEL_UP       
    {0x1E, 'C', 'D', 'N'},      // TV_CMD_CHANNEL_DOWN     
    {0x16, '0', '0', '0'},      // TV_CMD_NUMBER0          
    {0x0C, '0', '0', '1'},      // TV_CMD_NUMBER1          
    {0x0D, '0', '0', '2'},      // TV_CMD_NUMBER2          
    {0x0E, '0', '0', '3'},      // TV_CMD_NUMBER3          
    {0x0F, '0', '0', '4'},      // TV_CMD_NUMBER4          
    {0x10, '0', '0', '5'},      // TV_CMD_NUMBER5          
    {0x11, '0', '0', '6'},      // TV_CMD_NUMBER6          
    {0x12, '0', '0', '7'},      // TV_CMD_NUMBER7          
    {0x13, '0', '0', '8'},      // TV_CMD_NUMBER8          
    {0x14, '0', '0', '9'},      // TV_CMD_NUMBER9          
    {0x17, '1', '0', '0'},      // TV_CMD_NUMBER100        
    {0x15, 'R', 'T', 'N'},      // TV_CMD_CHANNEL_RETURN   
    {0x47, 'C', 'C', 'D'},      // TV_CMD_CAPTION_DISPLAY  
    {0x43, 'A', 'P', 'T'},      // TV_CMD_ASPECT_RATIO     
    {0x4F, 'V', 'C', 'P'},      // TV_CMD_VCHIP            
    {0x59, 'C', 'B', 'S'},      // TV_CMD_COMPOSITEIN      
    {0x5A, 'S', 'V', 'O'},      // TV_CMD_SVIDOE_IN        
    {0x5B, 'C', 'M', 'P'},      // TV_CMD_COMPONENT        
    {0x5C, 'V', 'G', 'A'},      // TV_CMD_VGAIN            
    {0x5D, 'T', 'N', 'R'},      // TV_CMD_TUNER_IN         
    {0x5E, 'H', 'D', 'M'},      // TV_CMD_HDMI_IN          
    {0x5F, 'S', 'T', 'A'},      // TV_CMD_STATUS           
    {0x48, 'E', 'P', 'G'},      // TV_CMD_EPG              
    {0x54, 'T', 'X', 'T'},      // TV_CMD_TEL_TEXT         
    {0x55, 'R', 'E', 'D'},      // TV_CMD_RED              
    {0x56, 'G', 'R', 'E'},      // TV_CMD_GREEN            
    {0x57, 'Y', 'E', 'L'},      // TV_CMD_YELLOW           
    {0x58, 'B', 'L', 'U'},      // TV_CMD_BLUE     
    {0xFB, 'A', 'D', 'I'}       // TV_CMD_ADI
    //{0x66, 'F', 'A', 'V'},      //=TV_CMD_CLOSE_CAPTION  
};


UCHAR TV_Get_Crc8(UCHAR *pby_Data, UINT16 by_Length);
void TV_Send(UCHAR *pString, UCHAR by_Length);
void TV_Rx_Int(UCHAR by_Data);
void TV_Tx_Int(void);


void TV_Initial(UCHAR bFirst)
{
    TV_Hw_Init();
    TV_Hw_Set_RxFuct(TV_Rx_Int);
    TV_Hw_Set_TxFuct(TV_Tx_Int);
    
    if(bFirst)
    {
        memset(aby_RxBuff,      0, sizeof(aby_RxBuff));
        memset(aby_TxBuff,      0, sizeof(aby_TxBuff));
        memset(aby_TxSave,      0, sizeof(aby_TxSave));
        memset(&Uart,           0, sizeof(Uart));
        memset(&TV_Error,       0, sizeof(TV_Error));
        memset(aby_FIFO_Buff, 0, sizeof(aby_FIFO_Buff));
        
        FIFO_Initial(&FIFO_Base, aby_FIFO_Buff, sizeof(aby_FIFO_Buff));
        
        RxStep = RStart;
        by_TxSaveLength = 0;
        by_ReTxTimes = 0;
        w_ReTxTimerCounter = 0;
        by_RxPointer = 0;
        by_TxLength = 0;
        w_TxPointer = 0;
        by_RxTimeOut = 0;
        by_TxTimeOut = 0;
        w_LostConnectCounter = 0;
        w_KeepOnlineTimer = 0;
        w_DelayTx = 0;
        by_TunerType = 0;
        by_PowerStatus = 0;
        w_FirstDelayCounter = 0;
        TV_SendCommand(TV_CMD_STATUS);
        TV_SendCommand(TV_CMD_ADI);
    }
}

void TV_SendCommand(TV_COM_TYPE Cmd)
{
    if(Cmd > TV_CMD_LEN) return;
    if(by_TunerType == 0xFD && Cmd == TV_CMD_MUTE) return;
    
    UCHAR by_Data = Cmd;
    FIFO_Input(&FIFO_Base, &by_Data, 1);
}

UCHAR TV_Process(void)
{
    UCHAR by_Out = 0;
    
    if(w_FirstDelayCounter < C_FIRST_DELAY_TIME) return 0;
    
    if(Uart.RxOK)
    {
//        printf("TV_Rx: ");
//        for(UCHAR i=0; i<4; i++)
//        {
//            printf("%02x ", aby_RxBuff[i]);
//        }
//        printf("\n");
        
        if(aby_TxSave[0] == TV_CMD_ADI)
        {
            if((aby_RxBuff[TV_ADR_IRCODE] == 0xFA) || 
               (aby_RxBuff[TV_ADR_IRCODE] == 0xFB) || 
               (aby_RxBuff[TV_ADR_IRCODE] == 0xFC) || 
               (aby_RxBuff[TV_ADR_IRCODE] == 0xFD))
            {
                by_TunerType = aby_RxBuff[TV_ADR_IRCODE];
            }
        }
        
        if(aby_RxBuff[TV_ADR_STATUS] & TV_STATUS_POWER)
        {
            by_PowerStatus = 0x01;
        }
        else
        {
            by_PowerStatus = 0x00;
        }
        
        memset(aby_RxBuff, 0, sizeof(aby_RxBuff));
        Uart.RxOK = 0;
        by_ReTxTimes = 0;
        w_DelayTx = 0;
        by_Out = 1;
    }
    
    
    if(w_DelayTx >= C_DELAY_TX)
    {
        if(by_ReTxTimes == 0)
        {
            by_TxSaveLength = FIFO_Output(&FIFO_Base, aby_TxSave, sizeof(aby_TxSave));
            
            if(by_TxSaveLength)                                                 //数值不能大于255,大于就出错
            {
                //by_ReTxTimes = C_RETX_TIMES;                                  //重发功能
                TV_Send(aby_TxSave, by_TxSaveLength);
            }
            else
            {
                if(w_KeepOnlineTimer > C_KEEP_ONLINE)                           //自动保持连接
                {
                    w_KeepOnlineTimer = 0;
                    //aby_TxSave[0] = TV_CMD_STATUS;
                    //TV_Send(aby_TxSave, 1);
                }
            }
        }
        else
        {
            if(by_TxSaveLength && w_ReTxTimerCounter >= C_RETX_TIME)
            {
                if(by_ReTxTimes) by_ReTxTimes--;                                //重发次数减1
                TV_Send(aby_TxSave, by_TxSaveLength);
            }
        }
    }
    
    return by_Out;
}

void TV_Send(UCHAR *pString, UCHAR by_Length)
{
    if(by_Length == 1)
    {
        if(pString[0] > TV_CMD_LEN) return;
        
        memset(aby_TxBuff, 0, sizeof(aby_TxBuff));
        aby_TxBuff[0] = TX_START;
        
        for(UCHAR i=1; i<=3; i++)
        {
            aby_TxBuff[i] = TvCommandTable[pString[0]][i];
        }
        
        aby_TxBuff[4] = TV_Get_Crc8(aby_TxBuff, 4);
        by_TxLength = 5;
        w_ReTxTimerCounter = 0;
        w_DelayTx = 0;
        by_TxTimeOut = 0;
        Uart.TxBuzy = 1;
        
//        printf("TV_Tx:%02x,", aby_TxBuff[0]);
//        for(UCHAR i=1; i<4; i++)
//        {
//            printf("%02x", aby_TxBuff[i]);
//            printf("(%c),", aby_TxBuff[i]);
//        }
//        printf("%02x\n", aby_TxBuff[4]);
        
        TV_Tx_Int();
    }
}

void TV_Tx_Int(void)
{
    if(w_TxPointer < by_TxLength)
    {
        TV_Hw_Send_Byte(aby_TxBuff[w_TxPointer++]);
    }
    else
    {
        by_TxLength = 0;
        w_TxPointer = 0;
        Uart.TxBuzy = 0;
    }
}

void TV_Rx_Int(UCHAR by_Data)
{
    UCHAR by_Temp = by_Data;
    
    if(!Uart.RxOK)
    {
        switch(RxStep)
        {
        default:
        case RStart:
            Uart.RxOK = 0;
            if(by_Temp == RX_START)
            {
                Uart.RxStart = 1;
                by_RxTimeOut = 0;
                by_RxPointer = 0;
                memset(aby_RxBuff, 0, sizeof(aby_RxBuff));
                aby_RxBuff[by_RxPointer++] = by_Temp;
                RxStep = RStatu;
            }
            break;
        case RStatu:
            aby_RxBuff[by_RxPointer++] = by_Temp;
            RxStep = RIRCode;
            break;
        case RIRCode:
            aby_RxBuff[by_RxPointer++] = by_Temp;
            RxStep = RCRC;
            break;
        case RCRC:
            Uart.RxStart = 0;
            aby_RxBuff[by_RxPointer] = by_Temp;
            UCHAR by_CRC = TV_Get_Crc8(aby_RxBuff, by_RxPointer);
            if(by_CRC == aby_RxBuff[by_RxPointer])
            {
                TV_Error.TV_TimeOut = 0;
                Uart.RxOK = 1;
                Uart.Connected = 1;
            }
            else
            {
                TV_Error.TV_CRC = 1;
                Uart.RxOK = 0;
            }
            
            RxStep = RStart;
            break;
        }
    }
    
    w_LostConnectCounter = 0;                                                   //losttime 重新计数,只要有接收中断,则认为上控在尝试连接
}

void TV_1ms_Int(void)
{
    if(Uart.TxBuzy)                                                             //防止发送超时锁死
    {
        if(by_TxTimeOut++ > C_TX_TIMEOUT)
        {
            by_TxTimeOut = 0;
            Uart.TxBuzy = 0;
            memset(aby_TxBuff, 0, sizeof(aby_TxBuff));                          //清空发送数据BUFF
        }
    }
    else
    {
        by_TxTimeOut = 0;
    }
    
    if(Uart.RxStart)
    {
        if(++by_RxTimeOut > C_RX_TIMEOUT)                                       //接收超时
        {
            TV_Error.TV_Rx_Timeout = 1;
            Uart.RxStart = 0;
            RxStep = RStart;
            by_RxPointer = 0;
        }
    }
    else
    {
        by_RxTimeOut = 0;
    }
    
    
    if(w_DelayTx < 0xFFFF) w_DelayTx++;
    if(w_KeepOnlineTimer < 0xFFFF) w_KeepOnlineTimer++;
    if(w_ReTxTimerCounter < 0xFFFF) w_ReTxTimerCounter++;
    if(w_FirstDelayCounter < 0xFFFF) w_FirstDelayCounter++;
    
    if(w_LostConnectCounter < (Uart.Connected ? C_LOST_TIME : C_FIRST_LOST_TIME))    //第一次启动lost时间10秒, 连接成功后lost时间5秒
    {
        w_LostConnectCounter++;
    }
    else
    {
        TV_Error.TV_TimeOut = 1;
    }
}

UCHAR TV_Get_Crc8(UCHAR *pby_Data, UINT16 by_Length)
{
    UCHAR by_Out = 0;
    
    for(UCHAR by_Loop = 0; by_Loop < by_Length; by_Loop++)
    {
        by_Out += pby_Data[by_Loop];
    }
    
    return by_Out;
}

UCHAR TV_TVTunerType(void)
{
    return by_TunerType;
}

UCHAR TV_TVTunerPowerStatus(void)
{
    return by_PowerStatus;
}

void TV_Erp(void)
{
    TV_Hw_Erp();
}




