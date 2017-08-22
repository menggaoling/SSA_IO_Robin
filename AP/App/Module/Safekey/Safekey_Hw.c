#include "Safekey_Hw.h"


#define SAFETY_KEY_PORT                 GPIOB
#define SAFETY_KEY_PIN                  BIT5


void Safekey_Hw_Initial(void)
{
    HW_Set_IO_InputFloat(SAFETY_KEY_PORT, SAFETY_KEY_PIN);
}

//safekey Remove: false, safkey plug: true
UCHAR Safekey_Hw_Get_Safekey(void)
{
    return HW_Get_IO_Filter(SAFETY_KEY_PORT, SAFETY_KEY_PIN);
}

void Safekey_Hw_ERP(void)
{
    HW_Set_IO_InputFloat(SAFETY_KEY_PORT, SAFETY_KEY_PIN);
}










