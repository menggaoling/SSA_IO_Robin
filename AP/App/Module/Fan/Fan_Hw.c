#include "Fan_Hw.h"
#include "hw.h"


#define FANL_PORT       GPIOA
#define FANL_PIN        BIT5

#define FANM_PORT       GPIOA
#define FANM_PIN        BIT6

#define FANH_PORT       GPIOA
#define FANH_PIN        BIT7


void Fan_Hw_Init(void)
{
    HW_Set_IO_Output(FANL_PORT, FANL_PIN);
    HW_Set_IO_Output(FANM_PORT, FANM_PIN);
    HW_Set_IO_Output(FANH_PORT, FANH_PIN);
    HW_Set_IO_Low(FANL_PORT, FANL_PIN);
    HW_Set_IO_Low(FANM_PORT, FANM_PIN);
    HW_Set_IO_Low(FANH_PORT, FANH_PIN);
}

void Fan_Hw_SetControl(FAN_CTRL FanCtrl)
{
    if(FanCtrl > FAN_HSP)
    {
        FanCtrl = FAN_HSP;
    }

    switch(FanCtrl)
    {
    case FAN_CLOSE:
        HW_Set_IO_Low(FANL_PORT, FANL_PIN);
        HW_Set_IO_Low(FANM_PORT, FANM_PIN);
        HW_Set_IO_Low(FANH_PORT, FANH_PIN);
        break;
    case FAN_LSP:
        HW_Set_IO_High(FANL_PORT, FANL_PIN);
        HW_Set_IO_Low(FANM_PORT, FANM_PIN);
        HW_Set_IO_Low(FANH_PORT, FANH_PIN);
        break;
    case FAN_MSP:
        HW_Set_IO_High(FANL_PORT, FANL_PIN);
        HW_Set_IO_High(FANM_PORT, FANM_PIN);
        HW_Set_IO_Low(FANH_PORT, FANH_PIN);
        break;
    case FAN_HSP:
        HW_Set_IO_High(FANL_PORT, FANL_PIN);
        HW_Set_IO_High(FANM_PORT, FANM_PIN);
        HW_Set_IO_High(FANH_PORT, FANH_PIN);
        break;
    }
}

void Fan_Hw_Erp(void)
{
    HW_Set_IO_InputFloat(FANL_PORT, FANL_PIN);
    HW_Set_IO_InputFloat(FANM_PORT, FANM_PIN);
    HW_Set_IO_InputFloat(FANH_PORT, FANH_PIN);
}


