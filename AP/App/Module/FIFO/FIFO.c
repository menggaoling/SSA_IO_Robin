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
//@param[in]  FIFO_Base FIFO���ݽṹ.
//@param[in]  pAdr FIFO����Buff���׵�ַ.
//@param[in]  Size FIFO����Buff�ĸ���.
//
//@return     NONE
//
//*****************************************************************************/
FIFO_ERROR FIFO_Initial(FIFO_BASE *FIFO_Base, UCHAR *pAdr, UINT16 Size)
{
    if(pAdr == NULL || Size < 3) return FIFO_ERROR_NOSPACE;                     //û�д洢�ռ�,ֱ�ӷ����޿ռ����
    
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
//@param[in]  FIFO_Base FIFO���ݽṹ.
//@param[in]  pAdr Ҫд�����ݵ��׵�ַ.
//@param[in]  Size Ҫд�����ݵĸ���.
//
//@return     FIFO_ERROR
//
//*****************************************************************************/
FIFO_ERROR FIFO_Input(FIFO_BASE *FIFO_Base, UCHAR *pAdr, UINT16 Size)
{
    if(FIFO_Base->pBuff == NULL) return FIFO_ERROR_NOSPACE;                     //û�д洢�ռ�,�����޿ռ����
    if(FIFO_Base->WriteAdr == NULL) FIFO_Base->WriteAdr = FIFO_Base->pBuff;     //���ָ��Ϊ�����ʼ��
    
    
    /*******************************�ռ����***********************************/
    
    //�������ݴ洢����ռ�
    UINT16 wDataSize = Size;
    for(UINT16 i = 0; i < Size; i++)
    {
        if(pAdr[i] == FIFO_START || pAdr[i] == FIFO_END || pAdr[i] == FIFO_FLAG)
        {
            wDataSize++;
        }
    }
    
    //Buff��ʣ��ռ����
    UINT16 wSize = 0;
    if(FIFO_Base->ReadAdr > FIFO_Base->WriteAdr)
    {
        wSize = FIFO_Base->ReadAdr - FIFO_Base->WriteAdr;
    }
    else
    {
        wSize = FIFO_Base->BuffSize - (FIFO_Base->WriteAdr - FIFO_Base->ReadAdr);
    }
    
    //�Ƚ�ʣ��ռ��Ƿ���, +2(һ����ʼ�ȼ�һ�������ֽ�)
    if(wSize < wDataSize + 2)                                                   
    {
        return FIFO_ERROR_FULL;                                                 //����ʣ��ռ䲻��
    }
    
    
    /*******************************����д��***********************************/
    *FIFO_Base->WriteAdr = FIFO_START;                                          //�ڵ�һ��λ��д����ʼλ
    FIFO_WriteAdrNext(FIFO_Base);                                               //д��ַ������һ��
    
    for(UINT16 i = 0; i < Size; i++)                                            //д������
    {
        if(pAdr[i] == FIFO_START || pAdr[i] == FIFO_END || pAdr[i] == FIFO_FLAG)//��Ҫչ��������
        {
            *FIFO_Base->WriteAdr = FIFO_FLAG;
            FIFO_WriteAdrNext(FIFO_Base);
            *FIFO_Base->WriteAdr = pAdr[i] - FIFO_FLAG;
        }
        else
        {
            *FIFO_Base->WriteAdr = pAdr[i];                                     //ֱ�ӱ��������
        }
        
        FIFO_WriteAdrNext(FIFO_Base);                                           //д��ַ������һ��
    }
    
    *FIFO_Base->WriteAdr = FIFO_END;                                            //д�����λ
    FIFO_WriteAdrNext(FIFO_Base);                                               //д��ַ������һ��
    
    return FIFO_ERROR_NONE;
}

//*****************************************************************************/
//@brief      FIFO Output
//
//@param[in]  FIFO_Base FIFO���ݽṹ.
//@param[in]  pAdr Ҫ�����ȡ���ݵ��׵�ַ.
//@param[in]  Size Ҫ�����ȡ���ݵ�������.
//
//@return     ��ȡ���ݵĸ���,�������û������,�򷵻���
//
//*****************************************************************************/
UINT16 FIFO_Output(FIFO_BASE *FIFO_Base, UCHAR *pAdr, UINT16 Size)
{
    if(FIFO_Base->pBuff == NULL) return 0;                                      //û�д洢�ռ�,������,��ʾ������
    if(FIFO_Base->ReadAdr == NULL) FIFO_Base->ReadAdr = FIFO_Base->pBuff;       //���ָ��Ϊ�����ʼ��
    if(FIFO_Base->ReadAdr == FIFO_Base->WriteAdr) return 0;                     //�ֻص���ʼָ���򷵻���,��ʾû������
    
    /*****************************�ҵ���ʼλ��*********************************/
    UCHAR *byFirstAdr = FIFO_Base->ReadAdr;                                     //�����ʼָ��,
    
    while(1)
    {
        if(*FIFO_Base->ReadAdr == FIFO_START)                                   //�ҵ���ʼλ���˳�,ִ�к�������
        {
            byFirstAdr = FIFO_Base->ReadAdr;                                    //������ʼλ�õ�ַ
            *FIFO_Base->ReadAdr = 0;                                            //������ʼλ�ñ�־
            break;
        }
        else
        {
            FIFO_ReadAdrNext(FIFO_Base);
            if(FIFO_Base->ReadAdr == FIFO_Base->WriteAdr) return 0;             //�ֻص���ʼָ���򷵻���,��ʾû������
        }
    }
    
    
    /*******************************��������***********************************/
    UINT16 wIndex = 0;
    memset(pAdr, 0, Size);                                                      //��ս�������
    
    while(1)
    {
        FIFO_ReadAdrNext(FIFO_Base);
        
        if(*FIFO_Base->ReadAdr == FIFO_END)                                     //�ҵ�����λ�򷵻�
        {
            *FIFO_Base->ReadAdr = 0;                                            //�������λ�ñ�־
            FIFO_ReadAdrNext(FIFO_Base);
            break;
        }
        else if(FIFO_Base->ReadAdr == byFirstAdr)                               //�ֻص�����ʼ��־λ�򷵻���,��ʾ�������ݱ��ƻ�,û�н�����
        {
            FIFO_ReadAdrNext(FIFO_Base);                                        //δ�ҵ�����λ��Ѷ�ȡ��ʼλ��ǰ��һ��λ��
            return 0;
        }
        else
        {
            if(wIndex < Size)
            {
                if(*FIFO_Base->ReadAdr == FIFO_FLAG)                            //��չ�������������½��
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
//@brief      FIFO_Peek ���FIFO���Ƿ�����������,���������ݳ���
//
//@param[in]  FIFO_Base FIFO���ݽṹ.
//@return     ��ȡ���ݵĸ���,�������û������,�򷵻���
//
//*****************************************************************************/
UINT16 FIFO_Peek(FIFO_BASE *FIFO_Base)
{
    if(FIFO_Base->pBuff == NULL) return 0;                                      //û�д洢�ռ�,������
    if(FIFO_Base->ReadAdr == NULL) FIFO_Base->ReadAdr = FIFO_Base->pBuff;       //���ָ��Ϊ�����ʼ��
    if(FIFO_Base->ReadAdr == FIFO_Base->WriteAdr) return 0;                     //�ֻص���ʼָ���򷵻���,��ʾû������
    
    UCHAR *by_AdrSave = FIFO_Base->ReadAdr;                                     //�����ʼָ��,��ȡ��֮�󽫶�ȡָ��ص�ԭֵ
    UCHAR *byFirstAdr = FIFO_Base->ReadAdr;                                     //�����ʼָ��,
    
    while(1)
    {
        if(*FIFO_Base->ReadAdr == FIFO_START)                                   //�ҵ���ʼλ���˳�,ִ�к�������
        {
            byFirstAdr = FIFO_Base->ReadAdr;                                    //������ʼλ�õ�ַ
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
        
        if(*FIFO_Base->ReadAdr == FIFO_END)                                     //�ҵ�����λ�򷵻�
        {
            FIFO_Base->ReadAdr = by_AdrSave;                                    //ָ�����»ص���ʼ����λ
            break;
        }
        else if(FIFO_Base->ReadAdr == byFirstAdr)                               //�ֻص�����ʼ��־λ�򷵻���,��ʾû����������
        {
            FIFO_ReadAdrNext(FIFO_Base);                                        //ָ�����»ص���ʼ��־λ,��ֹһ�����ݱ��ƻ���,һֱ����ͬһ����������
            return 0;
        }
        else
        {
            if(*FIFO_Base->ReadAdr == FIFO_FLAG)                                //��չ�������������½��
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

