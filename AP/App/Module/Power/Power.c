#include "Power.h"
#include "System.h"


#define POWER_UI_PORT                   GPIOB
#define POWER_UI_PIN                    BIT2

#define POWER_ERP_5V_PORT               GPIOE   //ERP下要保持打开 RS485,安全开关电源
#define POWER_ERP_5V_PIN                BIT4

#define POWER_9V_PORT                   GPIOD   //
#define POWER_9V_PIN                    BIT6

#define POWER_USB_PORT                  GPIOB
#define POWER_USB_PIN                   BIT0

#define POWER_USB1_PORT                 GPIOD
#define POWER_USB1_PIN                  BIT7

#define POWER_USB2_PORT                 GPIOD
#define POWER_USB2_PIN                  BIT1

#define POWER_USB3_PORT                 GPIOD
#define POWER_USB3_PIN                  BIT0

#define POWER_HR_PORT                   GPIOB
#define POWER_HR_PIN                    BIT9

#define POWER_TV_PORT                   GPIOB
#define POWER_TV_PIN                    BIT6

#define POWER_BT_PORT                   GPIOD
#define POWER_BT_PIN                    BIT15


#define POWER_STB_PORT                  GPIOA   //
#define POWER_STB_PIN                   BIT12

#define POWER_FAIL_PORT                 GPIOC
#define POWER_FAIL_PIN                  BIT5


__no_init volatile static UINT32 w_Power_On @ 0x20002000;


void Power_Initial(void)
{
    HW_Set_IO_Output(POWER_UI_PORT, POWER_UI_PIN);
    HW_Set_IO_High(POWER_UI_PORT, POWER_UI_PIN);
    
    HW_Set_IO_Output(POWER_ERP_5V_PORT, POWER_ERP_5V_PIN);
    HW_Set_IO_High(POWER_ERP_5V_PORT, POWER_ERP_5V_PIN);
    
    HW_Set_IO_Output(POWER_9V_PORT, POWER_9V_PIN);
    HW_Set_IO_High(POWER_9V_PORT, POWER_9V_PIN);
    
    HW_Set_IO_Output(POWER_USB_PORT, POWER_USB_PIN);
    HW_Set_IO_High(POWER_USB_PORT, POWER_USB_PIN);
    
    HW_Set_IO_Output(POWER_USB1_PORT, POWER_USB1_PIN);
    HW_Set_IO_High(POWER_USB1_PORT, POWER_USB1_PIN);
    
    HW_Set_IO_Output(POWER_USB2_PORT, POWER_USB2_PIN);
    HW_Set_IO_High(POWER_USB2_PORT, POWER_USB2_PIN);
    
    HW_Set_IO_Output(POWER_USB3_PORT, POWER_USB3_PIN);
    HW_Set_IO_High(POWER_USB3_PORT, POWER_USB3_PIN);
    
    HW_Set_IO_Output(POWER_HR_PORT, POWER_HR_PIN);
    HW_Set_IO_High(POWER_HR_PORT, POWER_HR_PIN);
    
    HW_Set_IO_Output(POWER_TV_PORT, POWER_TV_PIN);                              //默认TV电源不打开
    HW_Set_IO_Low(POWER_TV_PORT, POWER_TV_PIN);
    
    HW_Set_IO_Output(POWER_BT_PORT, POWER_BT_PIN);
    HW_Set_IO_High(POWER_BT_PORT, POWER_BT_PIN);
    
    HW_Set_IO_Input(POWER_FAIL_PORT, POWER_FAIL_PIN);
    HW_Set_IO_Input(POWER_STB_PORT, POWER_STB_PIN);
}

void Power_Set_Power_On(UINT32 w_Data)
{
    w_Power_On = w_Data;
}

UINT32 Power_Get_Power_On(void)
{
    return w_Power_On;
}

void Power_10ms_Int(void)
{
    static unsigned char by_Counter = 4;
    
    if(!HW_Get_IO(POWER_FAIL_PORT, POWER_FAIL_PIN))
    {
        if(by_Counter) by_Counter--;
        
        if(by_Counter == 1)
        {
            w_Power_On = C_PASSWORD_RESET;
            System_Reset();
        }
    }
    else
    {
        by_Counter = 4;
    }
}

void Power_UI_On(void)
{
    HW_Set_IO_Output(POWER_UI_PORT, POWER_UI_PIN);
    HW_Set_IO_High(POWER_UI_PORT, POWER_UI_PIN);
}

void Power_UI_Off(void)
{
    HW_Set_IO_Output(POWER_UI_PORT, POWER_UI_PIN);
    HW_Set_IO_Low(POWER_UI_PORT, POWER_UI_PIN);
}

void Power_USB1_On(void)
{
    HW_Set_IO_Output(POWER_USB1_PORT, POWER_USB1_PIN);
    HW_Set_IO_High(POWER_USB1_PORT, POWER_USB1_PIN);
}

void Power_USB1_Off(void)
{
    HW_Set_IO_Output(POWER_USB1_PORT, POWER_USB1_PIN);
    HW_Set_IO_Low(POWER_USB1_PORT, POWER_USB1_PIN);
}

void Power_USB2_On(void)
{
    HW_Set_IO_Output(POWER_USB2_PORT, POWER_USB2_PIN);
    HW_Set_IO_High(POWER_USB2_PORT, POWER_USB2_PIN);
}

void Power_USB2_Off(void)
{
    HW_Set_IO_Output(POWER_USB2_PORT, POWER_USB2_PIN);
    HW_Set_IO_Low(POWER_USB2_PORT, POWER_USB2_PIN);
}

void Power_USB3_On(void)
{
    HW_Set_IO_Output(POWER_USB3_PORT, POWER_USB3_PIN);
    HW_Set_IO_High(POWER_USB3_PORT, POWER_USB3_PIN);
}

void Power_USB3_Off(void)
{
    HW_Set_IO_Output(POWER_USB3_PORT, POWER_USB3_PIN);
    HW_Set_IO_Low(POWER_USB3_PORT, POWER_USB3_PIN);
}

void Power_TV_Off(void)
{
    HW_Set_IO_Output(POWER_TV_PORT, POWER_TV_PIN);
    HW_Set_IO_Low(POWER_TV_PORT, POWER_TV_PIN);
}

void Power_TV_On(void)
{
    HW_Set_IO_Output(POWER_TV_PORT, POWER_TV_PIN);
    HW_Set_IO_High(POWER_TV_PORT, POWER_TV_PIN);
}

void Power_Erp(void)
{
    HW_Set_IO_OutputLow(POWER_UI_PORT, POWER_UI_PIN);
    HW_Set_IO_Low(POWER_UI_PORT, POWER_UI_PIN);
    
    HW_Set_IO_InputFloat(POWER_ERP_5V_PORT, POWER_ERP_5V_PIN);
    HW_Set_IO_InputFloat(POWER_9V_PORT, POWER_9V_PIN);
    HW_Set_IO_InputFloat(POWER_USB_PORT, POWER_USB_PIN);
    HW_Set_IO_InputFloat(POWER_USB1_PORT, POWER_USB1_PIN);
    HW_Set_IO_InputFloat(POWER_USB2_PORT, POWER_USB2_PIN);
    HW_Set_IO_InputFloat(POWER_USB3_PORT, POWER_USB3_PIN);
    HW_Set_IO_InputFloat(POWER_HR_PORT, POWER_HR_PIN);
    HW_Set_IO_InputFloat(POWER_TV_PORT, POWER_TV_PIN);                          //默认TV电源不打开
    HW_Set_IO_InputFloat(POWER_BT_PORT, POWER_BT_PIN);
    HW_Set_IO_InputFloat(POWER_FAIL_PORT, POWER_FAIL_PIN);
    HW_Set_IO_InputFloat(POWER_STB_PORT, POWER_STB_PIN);
}



