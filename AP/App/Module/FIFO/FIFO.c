#include "FIFO.h"

//*****************************************************************************/
//Author: ouyangzh
//Create: 1-28-2016
//Version: V1
//
//*****************************************************************************/


#define FIFO_FLAG           0xD0
#define FIFO_START          0xD1
#define FIFO_END            0xD2


void FIFO_WriteAdrNext(FIFO_BASE *FIFO_Base);
void FIFO_ReadAdrNext(FIFO_BASE *FIFO_Base);


//*****************************************************************************/
//@brief      FIFO Initial
//@param[in]  FIFO_Base FIFO数据结构.
//@param[in]  pAdr FIFO数据Buff的首地址.
//@param[in]  Size FIFO数据Buff的个数.
//
//@return     NONE
//
//*****************************************************************************/
FIFO_ERROR FIFO_Initial(FIFO_BASE *FIFO_Base, UCHAR *pAdr, UINT16 Size)
{
    if(pAdr == NULL || Size < 3) return FIFO_ERROR_NOSPACE;                     //没有存储空间,直接返回无空间错误
    
    FIFO_Base->pBuff = pAdr;
    FIFO_Base->BuffSize = Size;
    FIFO_Base->ReadAdr  = FIFO_Base->pBuff;
    FIFO_Base->WriteAdr = FIFO_Base->pBuff;
    memset(FIFO_Base->pBuff, 0, FIFO_Base->BuffSize);
    
    return FIFO_ERROR_NONE;
}


//*****************************************************************************/
//@brief      FIFO Input
//
//@param[in]  FIFO_Base FIFO数据结构.
//@param[in]  pAdr 要写入数据的首地址.
//@param[in]  Size 要写入数据的个数.
//
//@return     FIFO_ERROR
//
//*****************************************************************************/
FIFO_ERROR FIFO_Input(FIFO_BASE *FIFO_Base, UCHAR *pAdr, UINT16 Size)
{
    if(FIFO_Base->pBuff == NULL) return FIFO_ERROR_NOSPACE;                     //没有存储空间,返回无空间错误
    if(FIFO_Base->WriteAdr == NULL) FIFO_Base->WriteAdr = FIFO_Base->pBuff;     //如果指针为零则初始化
    
    
    /*******************************空间计算***********************************/
    
    //计算数据存储所需空间
    UINT16 wDataSize = Size;
    for(UINT16 i = 0; i < Size; i++)
    {
        if(pAdr[i] == FIFO_START || pAdr[i] == FIFO_END || pAdr[i] == FIFO_FLAG)
        {
            wDataSize++;
        }
    }
    
    //Buff区剩余空间计算
    UINT16 wSize = 0;
    if(FIFO_Base->ReadAdr > FIFO_Base->WriteAdr)
    {
        wSize = FIFO_Base->ReadAdr - FIFO_Base->WriteAdr;
    }
    else
    {
        wSize = FIFO_Base->BuffSize - (FIFO_Base->WriteAdr - FIFO_Base->ReadAdr);
    }
    
    //比较剩余空间是否够用, +2(一个起始热及一个结束字节)
    if(wSize < wDataSize + 2)                                                   
    {
        return FIFO_ERROR_FULL;                                                 //返回剩余空间不够
    }
    
    
    /*******************************数据写入***********************************/
    *FIFO_Base->WriteAdr = FIFO_START;                                          //在第一个位置写入起始位
    FIFO_WriteAdrNext(FIFO_Base);                                               //写地址往下移一个
    
    for(UINT16 i = 0; i < Size; i++)                                            //写入数据
    {
        if(pAdr[i] == FIFO_START || pAdr[i] == FIFO_END || pAdr[i] == FIFO_FLAG)//需要展开的数据
        {
            *FIFO_Base->WriteAdr = FIFO_FLAG;
            FIFO_WriteAdrNext(FIFO_Base);
            *FIFO_Base->WriteAdr = pAdr[i] - FIFO_FLAG;
        }
        else
        {
            *FIFO_Base->WriteAdr = pAdr[i];                                     //直接保存的数据
        }
        
        FIFO_WriteAdrNext(FIFO_Base);                                           //写地址往下移一个
    }
    
    *FIFO_Base->WriteAdr = FIFO_END;                                            //写入结束位
    FIFO_WriteAdrNext(FIFO_Base);                                               //写地址往下移一个
    
    return FIFO_ERROR_NONE;
}

//*****************************************************************************/
//@brief      FIFO Output
//
//@param[in]  FIFO_Base FIFO数据结构.
//@param[in]  pAdr 要保存读取数据的首地址.
//@param[in]  Size 要保存读取数据的最大个数.
//
//@return     读取数据的个数,如果里面没数据了,则返回零
//
//*****************************************************************************/
UINT16 FIFO_Output(FIFO_BASE *FIFO_Base, UCHAR *pAdr, UINT16 Size)
{
    if(FIFO_Base->pBuff == NULL) return 0;                                      //没有存储空间,返回零,表示无数据
    if(FIFO_Base->ReadAdr == NULL) FIFO_Base->ReadAdr = FIFO_Base->pBuff;       //如果指针为零则初始化
    if(FIFO_Base->ReadAdr == FIFO_Base->WriteAdr) return 0;                     //又回到初始指针则返回零,表示没有数据
    
    /*****************************找到起始位置*********************************/
    UCHAR *byFirstAdr = FIFO_Base->ReadAdr;                                     //保存初始指针,
    
    while(1)
    {
        if(*FIFO_Base->ReadAdr == FIFO_START)                                   //找到起始位则退出,执行后续代码
        {
            byFirstAdr = FIFO_Base->ReadAdr;                                    //保存起始位置地址
            *FIFO_Base->ReadAdr = 0;                                            //清零起始位置标志
            break;
        }
        else
        {
            FIFO_ReadAdrNext(FIFO_Base);
            if(FIFO_Base->ReadAdr == FIFO_Base->WriteAdr) return 0;             //又回到初始指针则返回零,表示没有数据
        }
    }
    
    
    /*******************************读出数据***********************************/
    UINT16 wIndex = 0;
    memset(pAdr, 0, Size);                                                      //清空接收数组
    
    while(1)
    {
        FIFO_ReadAdrNext(FIFO_Base);
        
        if(*FIFO_Base->ReadAdr == FIFO_END)                                     //找到结束位则返回
        {
            *FIFO_Base->ReadAdr = 0;                                            //清零结束位置标志
            FIFO_ReadAdrNext(FIFO_Base);
            break;
        }
        else if(FIFO_Base->ReadAdr == byFirstAdr)                               //又回到了起始标志位则返回零,表示本次数据被破坏,没有结束码
        {
            FIFO_ReadAdrNext(FIFO_Base);                                        //未找到结束位则把读取初始位往前移一个位置
            return 0;
        }
        else
        {
            if(wIndex < Size)
            {
                if(*FIFO_Base->ReadAdr == FIFO_FLAG)                            //把展开过的数据重新结合
                {
                    pAdr[wIndex] = *FIFO_Base->ReadAdr;
                    FIFO_ReadAdrNext(FIFO_Base);
                    pAdr[wIndex++] += *FIFO_Base->ReadAdr;
                }
                else
                {
                    pAdr[wIndex++] = *FIFO_Base->ReadAdr;
                }
            }
            
            *FIFO_Base->ReadAdr = 0;
        }
    }
    
    return wIndex;
}

//*****************************************************************************/
//@brief      FIFO_Peek 检查FIFO中是否有完整数据,并返回数据长度
//
//@param[in]  FIFO_Base FIFO数据结构.
//@return     读取数据的个数,如果里面没数据了,则返回零
//
//*****************************************************************************/
UINT16 FIFO_Peek(FIFO_BASE *FIFO_Base)
{
    if(FIFO_Base->pBuff == NULL) return 0;                                      //没有存储空间,返回零
    if(FIFO_Base->ReadAdr == NULL) FIFO_Base->ReadAdr = FIFO_Base->pBuff;       //如果指针为零则初始化
    if(FIFO_Base->ReadAdr == FIFO_Base->WriteAdr) return 0;                     //又回到初始指针则返回零,表示没有数据
    
    UCHAR *by_AdrSave = FIFO_Base->ReadAdr;                                     //保存初始指针,读取完之后将读取指针回到原值
    UCHAR *byFirstAdr = FIFO_Base->ReadAdr;                                     //保存初始指针,
    
    while(1)
    {
        if(*FIFO_Base->ReadAdr == FIFO_START)                                   //找到起始位则退出,执行后续代码
        {
            byFirstAdr = FIFO_Base->ReadAdr;                                    //保存起始位置地址
            break;
        }
        else
        {
            FIFO_ReadAdrNext(FIFO_Base);
            if(FIFO_Base->ReadAdr == FIFO_Base->WriteAdr) return 0;
        }
    }
    
    
    UINT16 wIndex = 0;
    
    while(1)
    {
        FIFO_ReadAdrNext(FIFO_Base);
        
        if(*FIFO_Base->ReadAdr == FIFO_END)                                     //找到结束位则返回
        {
            FIFO_Base->ReadAdr = by_AdrSave;                                    //指针重新回到初始保存位
            break;
        }
        else if(FIFO_Base->ReadAdr == byFirstAdr)                               //又回到了起始标志位则返回零,表示没有完整数据
        {
            FIFO_ReadAdrNext(FIFO_Base);                                        //指针重新回到初始标志位,防止一组数据被破坏后,一直卡在同一组数据上面
            return 0;
        }
        else
        {
            if(*FIFO_Base->ReadAdr == FIFO_FLAG)                                //把展开过的数据重新结合
            {
                FIFO_ReadAdrNext(FIFO_Base);
            }
            
            wIndex++;
        }
    }
    
    return wIndex;
}

void FIFO_WriteAdrNext(FIFO_BASE *FIFO_Base)
{
    if(FIFO_Base->WriteAdr < (FIFO_Base->pBuff + FIFO_Base->BuffSize))
    {
        FIFO_Base->WriteAdr++;
    }
    else
    {
        FIFO_Base->WriteAdr = FIFO_Base->pBuff;
    }
}

void FIFO_ReadAdrNext(FIFO_BASE *FIFO_Base)
{
    if(FIFO_Base->ReadAdr < (FIFO_Base->pBuff + FIFO_Base->BuffSize))
    {
        FIFO_Base->ReadAdr++;
    }
    else
    {
        FIFO_Base->ReadAdr = FIFO_Base->pBuff;
    }
}

