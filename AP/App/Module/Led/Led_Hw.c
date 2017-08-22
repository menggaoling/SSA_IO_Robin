#include "Led_hw.h"


#define LED_RED_PORT            GPIOC
#define LED_RED_PIN             BIT8

#define LED_GREEN_PORT          GPIOC
#define LED_GREEN_PIN           BIT9


void Led_Hw_Initial(void)
{
    HW_Set_IO_Output(LED_RED_PORT, LED_RED_PIN);
    HW_Set_IO_Low(LED_RED_PORT, LED_RED_PIN);

    HW_Set_IO_Output(LED_GREEN_PORT, LED_GREEN_PIN);
    HW_Set_IO_Low(LED_GREEN_PORT, LED_GREEN_PIN);
}

void Led_Hw_Toggle(void)
{
    HW_Toggle_IO(LED_RED_PORT, LED_RED_PIN);
}

void Led_Hw_Set(void)
{
    HW_Set_IO_High(LED_RED_PORT, LED_RED_PIN);
}

void Led_Hw_Clr(void)
{
    HW_Set_IO_Low(LED_RED_PORT, LED_RED_PIN);
}

void Led_Hw_Erp(void)
{
    HW_Set_IO_InputFloat(LED_RED_PORT, LED_RED_PIN);
    HW_Set_IO_InputFloat(LED_GREEN_PORT, LED_GREEN_PIN);
}








