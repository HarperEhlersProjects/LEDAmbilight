#include "quadrature_interface.h"

#include "sysctl.h"
#include "interrupt.h"
#include "qei.h"
#include "gpio.h"
#include "hw_ints.h"
#include "pin_map.h"

#include "i2c_interface.h"
#include "ui_statemachine.h"
#include "settings.h"

uint16_t uiQEIPosition;
uint16_t uiQEIPositionPrevious;

uint16_t uiQEICounter;

uint8_t uiQEIDataFound;
void vQEIInit()
{
    //Enable clock to the QEI module
    SysCtlPeripheralEnable(SYSCTL_PERIPH_QEI0);
    //wait 3 clocksycles for SysCtlPeripheralEnable
    asm("\tnop;\r\n\tnop;\r\n\tnop;\r\n");

    //Enable clock to the QEI input GPIO Port
    SysCtlPeripheralEnable(QEI_SYSCTL_INPUT_PORT);
    //wait 3 clocksycles for SysCtlPeripheralEnable
    asm("\tnop;\r\n\tnop;\r\n\tnop;\r\n");

    //Set pins for as alternative function
    GPIOPinConfigure(GPIO_PL1_PHA0);
    GPIOPinConfigure(GPIO_PL2_PHB0);

    //Configure QEI GPIO Pins with alternative function for QEI
    GPIOPinTypeQEI(QEI_INPUT_PORT,QEI_PINS);

    //Use led controller interface output GPIO port as Digital IO
    GPIOPadConfigSet(QEI_INPUT_PORT,QEI_PINS,GPIO_STRENGTH_8MA,GPIO_PIN_TYPE_STD);

    //Configure QEI module as
    QEIConfigure(QEI_MODULE,QEI_CONFIGURATION,QEI_COUNTER_MAX);

    //Configure QEI velocity module
    QEIVelocityConfigure(QEI_MODULE,QEI_VELDIV_1,QEI_MEASUREMENT_TIME);

    //Configure QEI filter
    //QEIFilterConfigure(QEI_MODULE,QEI_FILTCNT_17);

    //Enable QEI filter
    //QEIFilterEnable(QEI_MODULE);

    //Set Position to initial value
    QEIPositionSet(QEI_MODULE,QEI_COUNTER_INIT);

    //Enable velocity module
    QEIVelocityEnable(QEI_MODULE);

    //register interrupt handler for increment and decrement
    QEIIntRegister(QEI_MODULE,QEIIncrementHandler);

    //Set priority of interrupt
    IntPrioritySet(INT_QEI0,0x40);

    //Enable velocity timer interrupt
    QEIIntEnable(QEI_MODULE,QEI_INTTIMER);

    QEIEnable(QEI_MODULE);
}

void QEIIncrementHandler(void)
{
    uiQEIPosition=QEIPositionGet(QEI_MODULE);

    QEIIntClear(QEI_MODULE,QEI_INTTIMER);

    switch(uiUIState)
    {
        case UI_STATE_STANDBY:
                              if(uiQEIPositionPrevious != uiQEIPosition)
                              {
                                  uiUIState=UI_STATE_SLA_SELECTION;
                                  uiSystemStandbyTimeSeconds=SYSTEM_STANDBY_TIME_INIT;
                                  QEIPositionSet(QEI_MODULE,QEI_COUNTER_OFFSET);
                              }
        break;
        case UI_STATE_SLA_SELECTION:
                              if(uiQEIPosition < QEI_COUNTER_OFFSET)
                              {
                                  QEIPositionSet(QEI_MODULE,QEI_COUNTER_OFFSET);
                                  uiUISLA=0;
                              }
                              else if(uiQEIPosition > (SETTINGS_SLA_AMOUNT*2)+1+QEI_COUNTER_OFFSET)
                              {
                                  QEIPositionSet(QEI_MODULE,18+QEI_COUNTER_OFFSET);
                                  uiUISLA=SETTINGS_SLA_AMOUNT;
                              }
                              else
                              {
                                  uiUISLA=((uiQEIPosition-QEI_COUNTER_OFFSET)>>1);
                              }

        break;
        case UI_STATE_MODE_SELECTION:
                              if(uiQEIPosition < QEI_COUNTER_OFFSET)
                              {
                                  QEIPositionSet(QEI_MODULE,QEI_COUNTER_OFFSET);
                                  uiUIMode=0;
                              }
                              else if(uiQEIPosition > (UI_PRESET_MAX_VALUE*2)+1+QEI_COUNTER_OFFSET)
                              {
                                  QEIPositionSet(QEI_MODULE,(UI_PRESET_MAX_VALUE*2)+QEI_COUNTER_OFFSET);
                                  uiUIMode=UI_PRESET_MAX_VALUE;
                              }
                              else
                              {
                                  uiUIMode=((uiQEIPosition-QEI_COUNTER_OFFSET)>>1);
                              }
        break;
        case UI_STATE_PARAMETER_SELECTION:
                              if(uiQEIPosition < QEI_COUNTER_OFFSET)
                              {
                                  QEIPositionSet(QEI_MODULE,QEI_COUNTER_OFFSET);
                                  uiUIParameter=0;
                              }
                              else if(uiQEIPosition > (SETTINGS_PARAMETER_AMOUNT*2)+1+QEI_COUNTER_OFFSET)
                              {
                                  QEIPositionSet(QEI_MODULE,(SETTINGS_PARAMETER_AMOUNT*2)+QEI_COUNTER_OFFSET);
                                  uiUIParameter=SETTINGS_PARAMETER_AMOUNT;
                              }
                              else
                              {
                                  uiUIParameter=((uiQEIPosition-QEI_COUNTER_OFFSET)>>1);
                              }

                              uiQEIDataFound=0;
                              uiQEICounter=0;
                              while(!uiQEIDataFound && uiQEICounter<SETTINGS_SLA_AMOUNT)
                              {
                                  if(uiUISLAActive[uiQEICounter])
                                  {
                                      uiUIParameterValue=tsSettings[uiQEICounter].uiParameterValue[uiUIParameter];
                                      uiQEIDataFound=1;
                                  }
                                  uiQEICounter++;
                              }
        break;

        case UI_STATE_PARAMETER_SELECTED:
                              if(uiQEIPosition < QEI_COUNTER_OFFSET)
                              {
                                  QEIPositionSet(QEI_MODULE,QEI_COUNTER_OFFSET);
                                  uiUIParameterValue=0;
                              }
                              else if(uiQEIPosition > 511+QEI_COUNTER_OFFSET)
                              {
                                  QEIPositionSet(QEI_MODULE,510+QEI_COUNTER_OFFSET);
                                  uiUIParameterValue=255;
                              }
                              else
                              {
                                  uiUIParameterValue=((uiQEIPosition-QEI_COUNTER_OFFSET)>>1);
                              }

                              for(uiQEICounter=0;uiQEICounter<SETTINGS_SLA_AMOUNT;uiQEICounter++)
                              {
                                  if(uiUISLAActive[uiQEICounter])
                                  {
                                      tsSettings[uiQEICounter].uiParameterValue[uiUIParameter]=uiUIParameterValue;
                                  }
                              }

        break;
    }


    /*
if(uiUIMode<UI_MODE_MAX_VALUE && uiUIMode > UI_MODIFIER_MAX_VALUE)
{
    uiUIParameterValue=
}
else
{

}
*/

if(uiQEIPositionPrevious != uiQEIPosition)
{
    uiSystemStandbyCounterSeconds=uiSystemStandbyTimeSeconds;
}

uiQEIPositionPrevious = uiQEIPosition;
vUIUpdate(); //update UI after every QEI interaction
}

void vQEIPositionSet(uint16_t uiValue)
{
    QEIPositionSet(QEI_MODULE,(uiValue<<1)+QEI_COUNTER_OFFSET);
}
