#include "ui_statemachine.h"

#include "i2c_interface.h"

uint8_t uiSevenSegmentLUT[UI_SYMBOLS_NUMBER] = {0x01,0x37,0x42,0x12,0x34,0x18,0x08,0x33,0x0,0x10,0x7F};

uint8_t uiUIDisplay00;
uint8_t uiUIDisplay01;

uint8_t uiUIDisplay10;
uint8_t uiUIDisplay11;
uint8_t uiUIDisplay12;

uint8_t uiUILED;

uint16_t uiUICounter;

void vUIInit()
{
    uiUIState=UI_STATE_STANDBY;

    for(uiUICounter=0;uiUICounter<8;uiUICounter++)
    {
        uiUISLAActive[uiUICounter]=0x0;
    }

    uiUIMode=0;

    for(uiUICounter=0;uiUICounter<UI_MODIFIER_MAX_VALUE-UI_MODE_MAX_VALUE;uiUICounter++)
    {
        uiUIModifierSelection[uiUICounter]=0x2;
    }

    uiUIModeActive=UI_MODE_NO_SELECTION;

    vUIUpdate();
}

void vUIUpdate(void)
{
    switch(uiUIState)
    {
        case UI_STATE_STANDBY:
                                    uiUIDisplay00=UI_SYMBOLS_BLANK;
                                    uiUIDisplay01=UI_SYMBOLS_BLANK;

                                    uiUIDisplay10=UI_SYMBOLS_BLANK;
                                    uiUIDisplay11=UI_SYMBOLS_BLANK;
                                    uiUIDisplay12=UI_SYMBOLS_BLANK;

                                    uiUILED=0x0;
        break;
        case UI_STATE_SLA_SELECTION:
                                    uiUIDisplay00=uiUISLA;
                                    uiUIDisplay01=UI_SYMBOLS_BLANK;

                                    uiUIDisplay10=UI_SYMBOLS_BLANK;
                                    uiUIDisplay11=UI_SYMBOLS_BLANK;
                                    uiUIDisplay12=UI_SYMBOLS_BLANK;

                                    if(uiUISLAActive[uiUISLA])
                                    {
                                        uiUILED=0x0A;
                                    }
                                    else
                                    {
                                        uiUILED=0x09;
                                    }
        break;
        case UI_STATE_MODE_SELECTION:

                                    if(uiUIMode<10)
                                    {
                                        uiUIDisplay00=uiUIMode;
                                        uiUIDisplay01=UI_SYMBOLS_BLANK;
                                    }
                                    else
                                    {
                                        uiUIDisplay00=uiUIMode%10;
                                        uiUIDisplay01=(int) (uiUIMode/10);
                                    }

                                    uiUIDisplay10=UI_SYMBOLS_BLANK;
                                    uiUIDisplay11=UI_SYMBOLS_BLANK;
                                    uiUIDisplay12=UI_SYMBOLS_BLANK;

                                    if(uiUIMode<UI_MODE_MAX_VALUE)
                                    {
                                        if(uiUIMode==uiUIModeActive)
                                        {
                                            uiUILED=0x15;

                                        }
                                        else
                                        {
                                            uiUILED=0x10;

                                        }
                                    }
                                    else if(uiUIMode<UI_MODIFIER_MAX_VALUE)
                                    {
                                        if(uiUIModifierSelection[uiUIMode-UI_MODE_MAX_VALUE]==0x2)
                                        {
                                            uiUILED=0x14;
                                        }
                                        else if(uiUIModifierSelection[uiUIMode-UI_MODE_MAX_VALUE]==0x0)
                                        {
                                            uiUILED=0x11;
                                        }
                                        else
                                        {
                                            uiUILED=0x12;
                                        }
                                    }
                                    else
                                    {
                                        uiUILED=0x17;
                                    }
        break;
        case UI_STATE_PARAMETER_SELECTION:
                                    uiUIDisplay00=uiUIParameter;
                                    uiUIDisplay01=UI_SYMBOLS_BLANK;

                                    if(uiUIParameterValue<10)
                                    {
                                        uiUIDisplay10=uiUIParameterValue;
                                        uiUIDisplay11=UI_SYMBOLS_BLANK;
                                        uiUIDisplay12=UI_SYMBOLS_BLANK;
                                    }
                                    else if(uiUIParameterValue<100)
                                    {
                                        uiUIDisplay10=uiUIParameterValue%10;
                                        uiUIDisplay11=uiUIParameterValue/10;
                                        uiUIDisplay12=UI_SYMBOLS_BLANK;
                                    }
                                    else
                                    {
                                        uiUIDisplay10=uiUIParameterValue%10;
                                        uiUIDisplay11=(uiUIParameterValue/10)%10;
                                        uiUIDisplay12=uiUIParameterValue/100;
                                    }

                                    uiUILED=0x20;
        break;
        case UI_STATE_PARAMETER_SELECTED:
                                    uiUIDisplay00=uiUIParameter;
                                    uiUIDisplay01=UI_SYMBOLS_BLANK;

                                    if(uiUIParameterValue<10)
                                    {
                                        uiUIDisplay10=uiUIParameterValue;
                                        uiUIDisplay11=UI_SYMBOLS_BLANK;
                                        uiUIDisplay12=UI_SYMBOLS_BLANK;
                                    }
                                    else if(uiUIParameterValue<100)
                                    {
                                        uiUIDisplay10=uiUIParameterValue%10;
                                        uiUIDisplay11=uiUIParameterValue/10;
                                        uiUIDisplay12=UI_SYMBOLS_BLANK;
                                    }
                                    else
                                    {
                                        uiUIDisplay10=uiUIParameterValue%10;
                                        uiUIDisplay11=(uiUIParameterValue/10)%10;
                                        uiUIDisplay12=uiUIParameterValue/100;
                                    }

                                    uiUILED=0x24;
        break;
    }

    vI2CInterfaceOutputWrite(0,uiSevenSegmentLUT[uiUIDisplay01]);
    vI2CInterfaceOutputWrite(1,uiSevenSegmentLUT[uiUIDisplay00]);

    vI2CInterfaceOutputWrite(2,uiSevenSegmentLUT[uiUIDisplay12]);
    vI2CInterfaceOutputWrite(3,uiSevenSegmentLUT[uiUIDisplay11]);
    vI2CInterfaceOutputWrite(4,uiSevenSegmentLUT[uiUIDisplay10]);

    vI2CInterfaceOutputWrite(5,uiUILED);
}

void vUIClear()
{
    vI2CInterfaceOutputWrite(0,uiSevenSegmentLUT[UI_SYMBOLS_BLANK]);
    vI2CInterfaceOutputWrite(1,uiSevenSegmentLUT[UI_SYMBOLS_BLANK]);

    vI2CInterfaceOutputWrite(2,uiSevenSegmentLUT[UI_SYMBOLS_BLANK]);
    vI2CInterfaceOutputWrite(3,uiSevenSegmentLUT[UI_SYMBOLS_BLANK]);
    vI2CInterfaceOutputWrite(4,uiSevenSegmentLUT[UI_SYMBOLS_BLANK]);

    vI2CInterfaceOutputWrite(5,0x0);
}
