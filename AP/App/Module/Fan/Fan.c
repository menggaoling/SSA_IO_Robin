#include "Fan.h"
#include "Fan_Hw.h"

static FAN_CTRL FanCtrl;


void Fan_Initial(void)
{
    Fan_Hw_Init();
    FanCtrl = FAN_CLOSE;
    Fan_SetControl(FanCtrl);
}

void Fan_Control(void)
{
    FanCtrl++;
    if(FanCtrl > FAN_HSP)
    {
        FanCtrl = FAN_CLOSE;
    }

    Fan_Hw_SetControl(FanCtrl);
}

void Fan_SetControl(FAN_CTRL by_Dat)
{
    if(by_Dat > FAN_HSP)
    {
        FanCtrl = FAN_CLOSE;
    }
    else
    {
        FanCtrl = by_Dat;
    }

    Fan_Hw_SetControl(FanCtrl);
}

FAN_CTRL Fan_GetControl(void)
{
    return FanCtrl;
}

void Fan_Erp(void)
{
    Fan_Hw_Erp();
}



