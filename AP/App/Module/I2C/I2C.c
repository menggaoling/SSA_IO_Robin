#include "I2C.h"
#include "hw.h"


#define I2C_WP_PORT                 GPIOC
#define I2C_WP_PIN                  BIT2

#define I2C_SDA_PORT                GPIOA
#define I2C_SDA_PIN                 BIT0

#define I2C_SCL_PORT                GPIOC
#define I2C_SCL_PIN                 BIT3


#define I2C_Set_SDA_Out()           HW_Set_IO_Output(I2C_SDA_PORT, I2C_SDA_PIN)
#define I2C_Set_SDA_In()            HW_Set_IO_Input(I2C_SDA_PORT, I2C_SDA_PIN)
#define I2C_Set_SDA()	            HW_Set_IO_High(I2C_SDA_PORT, I2C_SDA_PIN)
#define I2C_Clear_SDA()	            HW_Set_IO_Low(I2C_SDA_PORT, I2C_SDA_PIN)
#define I2C_Get_SDA()               (HW_Get_IO(I2C_SDA_PORT, I2C_SDA_PIN) ? 1 : 0)

#define I2C_Set_SCL_Out()           HW_Set_IO_Output(I2C_SCL_PORT, I2C_SCL_PIN)
#define I2C_Set_SCL_In()            HW_Set_IO_Input(I2C_SCL_PORT, I2C_SCL_PIN)
#define I2C_Set_SCL()	            HW_Set_IO_High(I2C_SCL_PORT, I2C_SCL_PIN)
#define I2C_Clear_SCL()	            HW_Set_IO_Low(I2C_SCL_PORT, I2C_SCL_PIN)

#define I2C_Set_WP_Out()            HW_Set_IO_Output(I2C_WP_PORT, I2C_WP_PIN)
#define I2C_Set_WP_In()             HW_Set_IO_Input(I2C_WP_PORT, I2C_WP_PIN)
#define I2C_Set_WP_Read()           HW_Set_IO_High(I2C_WP_PORT, I2C_WP_PIN)
#define I2C_Set_WP_WR()             HW_Set_IO_Low(I2C_WP_PORT, I2C_WP_PIN)


#define I2COK                       1
#define I2CFAIL                     2


__no_init static struct
{
    unsigned  I2C_Error     :1;
}b;


static void I2C_Start(void);
static void I2C_Stop(void);
static void I2C_ACK(void);
static void I2C_NACK(void);
static void I2C_Delay(void);
static void I2C_WriteByte(UCHAR WriteData);
static UCHAR I2C_ReadByte(void);
static UCHAR I2C_Read_Mass(UCHAR by_Address, UCHAR by_Length, UCHAR by_Offset, UCHAR *Aby_TX_Buffer);
static UCHAR I2C_Write_Mass(UCHAR by_Adress, UCHAR by_Len, UCHAR by_Offset, UCHAR *Aby_TX_Buffer);


void I2C_Initail(void)
{
    I2C_Set_WP_Out();
    I2C_Set_WP_Read();
    I2C_Set_SDA_Out();
    I2C_Clear_SDA();
    I2C_Set_SCL_Out();
    I2C_Clear_SCL();
}

void I2C_Error_Clear(void)
{
    b.I2C_Error = 0;
}

UCHAR I2C_Get_Error(void)
{
    return(b.I2C_Error);
}

void I2C_Delay(void)
{
    UINT16 i;
    for(i = 0; i < 50; i++)
    {
        asm("nop");
    }
}

/*********start************************
SCK  ---\____
SDA  -\______
***************************************/
void I2C_Start(void)
{
    I2C_Set_SDA_Out();	                                                        //Set SDA 输入，由于有上拉所以输入为高
    I2C_Set_SCL_Out();	                                                        //Set SCL 输入，由于有上拉所以输入为高
    I2C_Set_SCL();
    I2C_Set_SDA();
    I2C_Delay();                                                                //根据传输速率得到延迟时间
    I2C_Delay();
    I2C_Clear_SDA();                                                            //test
    I2C_Delay();
    I2C_Clear_SCL();                                                            //test0
    I2C_Delay();
}

/*********stop************************
SCK  ____
SDA  ______
***************************************/
void I2C_Stop(void)
{
    I2C_Clear_SDA();	                                                        //set SDA 输出
    I2C_Delay();
    I2C_Set_SCL();
    I2C_Delay();
    I2C_Set_SDA();	                                                            //set SDA 输出
    I2C_Delay();
    I2C_Delay();
    I2C_Set_SCL_In();	                                                        //Set SCL 输入
    I2C_Set_SDA_In();	                                                        //Set SDA to high
}

void I2C_ACK(void)
{
    I2C_Clear_SDA();	                                                        //set SDA 输出	
    I2C_Delay();
    I2C_Set_SCL();	                                                            //Set SCL to high
    
    I2C_Delay();
    I2C_Delay();
    I2C_Delay();
    
    I2C_Clear_SCL();	                                                        //Set SCL 输出	
    
    I2C_Delay();
}

void I2C_NACK(void)
{
    I2C_Set_SDA();		                                                        //Set SDA  to High	
    I2C_Delay();
    I2C_Delay();
    I2C_Set_SCL();		                                                        //Set SCL to High
    I2C_Delay();
    I2C_Delay();
    I2C_Clear_SCL();	                                                        //Set SCL 输出
    I2C_Delay();
    I2C_Delay();
}

/*********start************************
SCK  __|-|__|-|__|-|__|-|__|-|__|-|__|-|__|-|___
SDA  __x7   x6   x5   x4   x3   x2   x1   x0 ---
***************************************/
void I2C_WriteByte(UCHAR WriteData)
{
    UCHAR by_Loop;
    for(by_Loop = 0; by_Loop < 8; by_Loop++)
    {
        if(0 == (WriteData & BIT7))
        {
            I2C_Clear_SDA();	                                                //Set SDA 输出 low
        }
        else
        {
            I2C_Set_SDA();	                                                    //set SDA High 
        }
        I2C_Delay();
        I2C_Delay();
        I2C_Set_SCL();		                                                    //set SCL to high
        I2C_Delay();
        I2C_Delay();
        I2C_Clear_SCL();
        WriteData <<= 1;
    }
}

/*********start************************
SCK  ___\----\_____
SDA  ------Rxxxxxxx
***************************************/
UCHAR I2C_Check_ACK(void)
{
    UINT16 wCounter = 100;
    UCHAR by_Out = I2COK;
    I2C_Set_SDA_In();
    I2C_Delay();
    I2C_Set_SCL();
    I2C_Delay();
    I2C_Delay();
    
    while(--wCounter)
    {
        if(I2C_Get_SDA())
        {
            by_Out = I2CFAIL;                                                   //check ACK signal fail
        }
        else
        {
            by_Out = I2COK;                                                     //check ACK signal fail
            break;
        }
    }
    
    I2C_Clear_SCL();
    I2C_Delay();
    I2C_Delay();
    I2C_Delay();
    I2C_Delay();
    I2C_Set_SDA_Out();
    
    return by_Out;
}

UCHAR I2C_ReadByte(void)
{
    UCHAR by_Out = 0;
    
    I2C_Set_SDA_In();                                                           //SDA设为输入
    I2C_Delay();
    
    for(UCHAR by_Loop = 0; by_Loop < 8; by_Loop++)
    {
        I2C_Set_SCL();	                                                        //set SCL to High
        
        I2C_Delay();
        I2C_Delay();
        
        by_Out = (by_Out << 1) | I2C_Get_SDA();		                            //read a bit
        
        I2C_Clear_SCL();	                                                    //set SCL to low
        I2C_Delay();
        I2C_Delay();
    }
    
    I2C_Set_SDA_Out();//SDA设为输入
    
    return(by_Out);	
}

UCHAR I2C_Write_Mass(UCHAR by_Adress, UCHAR by_Len, UCHAR by_Offset, UCHAR *Aby_TX_Buffer)
{
    I2C_Set_WP_WR();
    
    UINT16 wCounter = 100;
    UCHAR by_Out = 0;
    
    while(wCounter--)
    {
        I2C_Start();
        I2C_WriteByte(0x22);
        if(I2C_Check_ACK() == I2COK) 
        {
            by_Out = 1;
            break;
        }
    }
    if(by_Out != 1) 
    {
        return 0;  
    }
    
    I2C_WriteByte(by_Adress);
    
    if(I2C_Check_ACK() != I2COK) 
    {
        return 0;
    }
    
    for(UCHAR by_Loop =0;by_Loop < by_Len; by_Loop++)
    {
        
        I2C_WriteByte(*(Aby_TX_Buffer+(by_Loop+by_Offset)));
        if(I2C_Check_ACK() != I2COK) 
        {
            return 0;
        }
    }
    
    I2C_Stop();
    
    I2C_Set_WP_Read();
    
    return 1;
}

UCHAR I2C_Read_Mass(UCHAR by_Address, UCHAR by_Length, UCHAR by_Offset, UCHAR *Aby_TX_Buffer)
{
    UINT16 wCounter = 100;
    UCHAR by_Out = 0;
    
    while(wCounter--)
    {
        I2C_Start();
        I2C_WriteByte(0x22);
        if(I2C_Check_ACK() == I2COK) 
        {
            by_Out = 1;
            break;
        }
    }
    
    if(by_Out != 1) 
    {
        return 0;
    }
    
    I2C_WriteByte(by_Address);
    
    if(I2C_Check_ACK() != I2COK) 
    {
        return 0;
    }
    
    I2C_Stop();
    
    by_Out = 0;
    wCounter = 100;
    
    while(wCounter--)
    {
        I2C_Start();
        I2C_WriteByte(0x23);                                                    //0x22地址
        if(I2C_Check_ACK() == I2COK)
        {
            by_Out = 1;
            break;
        }
    }
    
    if(by_Out != 1) 
    {
        return 0;
    }
    
    UINT16 wLoop = 0;
    
    for(wLoop=0; wLoop<(by_Length-1); wLoop++)
    {
        *(Aby_TX_Buffer+(wLoop+by_Offset)) = I2C_ReadByte();
        I2C_ACK();
    }
    
    *(Aby_TX_Buffer+(wLoop+by_Offset)) = I2C_ReadByte();
    I2C_NACK();
    I2C_Stop();
    
    return 1;
}

UCHAR I2C_Read(UCHAR by_Address, UCHAR by_Length, UCHAR by_Offset, UCHAR *Aby_TX_Buffer)
{
    UCHAR by_Loop = 5;
    
    while(by_Loop--)                                                            //尝试5次连接，如果都连接超时将不再尝试连接
    {
        if(I2C_Read_Mass(by_Address, by_Length, by_Offset, Aby_TX_Buffer) != 0)
        {
            return 1;
        }
        else
        {
            I2C_Initail();
        }
    }
    
    b.I2C_Error = 1;
    
    return 0;
}

UCHAR I2C_Write(UCHAR by_Adress, UCHAR by_Len, UCHAR by_Offset, UCHAR *Aby_TX_Buffer)
{
    UCHAR by_Loop = 5;
    
    while(by_Loop--)                                                            //尝试5次连接，如果都连接超时将不再尝试连接
    {
        if(I2C_Write_Mass(by_Adress, by_Len, by_Offset, Aby_TX_Buffer) != 0)
        {
            return  1;
        }
        else
        {
            I2C_Initail();
        }
    }
    
    b.I2C_Error = 1;
    
    return 0;
}

void I2C_Erp(void)
{
    HW_Set_IO_InputFloat(I2C_WP_PORT, I2C_WP_PIN);
    HW_Set_IO_InputFloat(I2C_SDA_PORT, I2C_SDA_PIN);
    HW_Set_IO_InputFloat(I2C_SCL_PORT, I2C_SCL_PIN);
}


