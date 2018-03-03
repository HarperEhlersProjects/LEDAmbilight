#include "system_set_up.h"
#include "interrupt.h"

#include "ui_statemachine.h"

uint16_t uiSystemCounter;

void vSystemSetUp(void)
{
    clock=SysCtlClockFreqSet(SYSCTL_OSC_INT | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480, SYS_CLK);

    tsSystemTime.uiSecond=0;
    tsSystemTime.uiMinute=0;
    tsSystemTime.uiHour=0;
    tsSystemTime.uiDay=0;
    tsSystemTime.uiMonth=0;
    tsSystemTime.uiYear=0;

    uiSystemStandbyTimeSeconds=SYSTEM_STANDBY_TIME_INIT;

    SysTickPeriodSet(SYS_CLK/10); //interrupt every 100ms

    SysTickIntRegister(vSystemSysTickHandler);

    IntPrioritySet(15,0x20); //15 is the place of the handler in Interrupt vector table

    SysTickIntEnable();

    SysTickEnable();
}

void vSystemSysTickHandler(void)
{
    if(tsSystemTime.ui100MilliSecond<10)
    {
        tsSystemTime.ui100MilliSecond++;
    }
    else if(tsSystemTime.uiSecond<59)
    {
        tsSystemTime.ui100MilliSecond=0;
        tsSystemTime.uiSecond++;

        //----------------GENERATING STANDBY TIMER---------------------------------------
        if(uiSystemStandbyCounterSeconds>0)
        {
            uiSystemStandbyCounterSeconds--;
        }
        else
        {
            uiUIState=UI_STATE_STANDBY;

            uiUISLA=0;
            uiUIMode=0;
            uiUIModeActive=UI_MODE_NO_SELECTION;
            uiUIParameter=0;

            for(uiSystemCounter=0;uiSystemCounter<SETTINGS_SLA_AMOUNT;uiSystemCounter++)
            {
                uiUISLAActive[uiSystemCounter]=0x0;
            }

            for(uiSystemCounter=0;uiSystemCounter<UI_MODIFIER_MAX_VALUE-UI_MODE_MAX_VALUE;uiSystemCounter++)
            {
                uiUIModifierSelection[uiSystemCounter]=0x2;
            }

            vUIUpdate();
        }
        //----------------GENERATING STANDBY TIMER---------------------------------------

    }
    else if(tsSystemTime.uiMinute<59)
    {
        tsSystemTime.ui100MilliSecond=0;
        tsSystemTime.uiSecond=0;
        tsSystemTime.uiMinute++;
    }
    else if(tsSystemTime.uiHour<23)
    {
        tsSystemTime.ui100MilliSecond=0;
        tsSystemTime.uiSecond=0;
        tsSystemTime.uiMinute=0;
        tsSystemTime.uiHour++;
    }
    else
    {
        tsSystemTime.ui100MilliSecond=0;
        tsSystemTime.uiSecond=0;
        tsSystemTime.uiMinute=0;
        tsSystemTime.uiHour=0;
        tsSystemTime.uiDay++;
    }
}
