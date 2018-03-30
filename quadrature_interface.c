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
        case UI_STATE_SLA_SELECTION: uiUISLA = uiQEIPositionGet(0,SETTINGS_SLA_AMOUNT);
        break;
        case UI_STATE_MODE_SELECTION: uiUIMode = uiQEIPositionGet(0,UI_PRESET_MAX_VALUE);
        break;
        case UI_STATE_PARAMETER_SELECTION:  uiUIParameter = uiQEIPositionGet(0,SETTINGS_PARAMETER_AMOUNT);
                                            uiUIParameterValue = uiUIParameterValueGet();
        break;
        case UI_STATE_PARAMETER_SELECTED:   uiUIParameterValue = uiQEIPositionGet(0,510);
                                            uiUIParameterValueSet();
        break;
    }

if(uiQEIPositionPrevious != uiQEIPosition)
{
    vUIWakeUp();
}

uiQEIPositionPrevious = uiQEIPosition;
vUIUpdate(); //update UI after every QEI interaction
}

void vQEIPositionSet(uint16_t uiValue)
{
    QEIPositionSet(QEI_MODULE,(uiValue<<1)+QEI_COUNTER_OFFSET);
}

uint16_t uiQEIPositionGet(uint16_t uiBoundaryBottom, uint16_t uiBoundaryTop)
{
    if(uiQEIPosition < (QEI_COUNTER_OFFSET + uiBoundaryBottom))
    {
        QEIPositionSet(QEI_MODULE,QEI_COUNTER_OFFSET + (uiBoundaryBottom << 1));
        return uiBoundaryBottom;
    }
    else if(uiQEIPosition > (uiBoundaryTop << 1) + 1 + QEI_COUNTER_OFFSET)
    {
        QEIPositionSet(QEI_MODULE,(uiBoundaryTop << 1) + QEI_COUNTER_OFFSET);
        return uiBoundaryTop;
    }
    else
    {
        return ((uiQEIPosition-QEI_COUNTER_OFFSET)>>1);
    }

}
