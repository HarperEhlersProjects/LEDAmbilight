#include "ui_statemachine.h"

#include "i2c_interface.h"
#include "system_set_up.h"

uint8_t uiSevenSegmentLUT[UI_SYMBOLS_NUMBER] = {0x01,0x37,0x42,0x12,0x34,0x18,0x08,0x33,0x0,0x10,0x7F};

uint8_t uiUIDisplay00;
uint8_t uiUIDisplay01;

uint8_t uiUIDisplay10;
uint8_t uiUIDisplay11;
uint8_t uiUIDisplay12;

uint8_t uiUILED;

uint16_t uiUICounter;
uint8_t uiUICounterSLA;

void vUIInit()
{
    uiUIState=UI_STATE_STANDBY;
    uiUIStandbyPreviousState=UI_STATE_SLA_SELECTION;

    for(uiUICounter=0;uiUICounter<8;uiUICounter++)
    {
        uiUISLAActive[uiUICounter]=0x0;
    }

    uiUIMode=0;

    for(uiUICounter=1;uiUICounter<SETTINGS_MODIFIER_AMOUNT+1;uiUICounter++)
    {
        uiUIModeActive[uiUICounter]=0x2;
    }

    uiUIModeActive[0]=UI_MODE_NO_SELECTION;

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

                                    if(uiUIModeTypeGet() == UI_STATE_TYPE_MODE)
                                    {
                                        if(uiUIMode==uiUIModeActive[0])
                                        {
                                            uiUILED=0x15;

                                        }
                                        else
                                        {
                                            uiUILED=0x10;

                                        }
                                    }
                                    else if(uiUIModeTypeGet() == UI_STATE_TYPE_MODIFIER)
                                    {
                                        if(uiUIModeActive[UI_MODIFIER_INDEX]==0x2)
                                        {
                                            uiUILED=0x14;
                                        }
                                        else if(uiUIModeActive[UI_MODIFIER_INDEX]==0x0)
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

void vUIStandby(void)
{
    if(uiUIState!=UI_STATE_STANDBY)
    {
        uiUIStandbyPreviousState=uiUIState;
    }

    uiUIState=UI_STATE_STANDBY;

    vUIUpdate();
}


void vUIReset(void)
{
    uint8_t uiCounter;

    uiUIStandbyPreviousState=UI_STATE_SLA_SELECTION;

    if(uiUIState != UI_STATE_STANDBY)
    {
        uiUIState=UI_STATE_SLA_SELECTION;
    }

    uiUISLA=0;
    uiUIMode=0;
    uiUIModeActive[0]=UI_MODE_NO_SELECTION;
    uiUIParameter=0;

    for(uiCounter=0;uiCounter<SETTINGS_SLA_AMOUNT;uiCounter++)
    {
        uiUISLAActive[uiCounter]=0x0;
    }

    for(uiCounter=1;uiCounter<SETTINGS_MODIFIER_AMOUNT;uiCounter++)
    {
        uiUIModeActive[uiCounter]=0x2;
    }

    vUIUpdate();
}

void vUIWakeUp(void)
{
    if(uiUIState==UI_STATE_STANDBY) //WAKE UP
    {
        uiUIState=uiUIStandbyPreviousState;
    }

    uiSystemStandbyCounterSeconds=uiSystemStandbyTimeSeconds;
    uiSystemResetCounterSeconds=uiSystemResetTimeSeconds;
}

uint8_t uiUIModeTypeGet(void)
{

    if(uiUIMode <= UI_MODE_MAX_VALUE)
    {
        return UI_STATE_TYPE_MODE;
    }
    else if(uiUIMode <= UI_MODIFIER_MAX_VALUE)
    {
        return UI_STATE_TYPE_MODIFIER;
    }
    else
    {
        return UI_STATE_TYPE_PRESET;
    }
}


void vUIParameterSet(void)
{
    uint8_t uiCounterSLA;

    if(uiUIModeTypeGet() == UI_STATE_TYPE_MODE)
    {
        for(uiCounterSLA=0;uiCounterSLA<SETTINGS_SLA_AMOUNT;uiCounterSLA++)
        {
            if(uiUISLAActive[uiCounterSLA])
            {
                tsSettings[uiCounterSLA].uiModeParameter[0][uiUIParameter]=uiUIParameterValue;
            }
        }
    }
    else
    {
        for(uiCounterSLA=0;uiCounterSLA<SETTINGS_SLA_AMOUNT;uiCounterSLA++)
        {
            if(uiUISLAActive[uiCounterSLA])
            {
                tsSettings[uiCounterSLA].uiModeParameter[UI_MODIFIER_INDEX][uiUIParameter]=uiUIParameterValue;
            }
        }
    }

}

void vUISLASelectionSwitch(void)
{
    uint8_t uiCounterModifier;

    //switch selection
    uiUISLAActive[uiUISLA]=!uiUISLAActive[uiUISLA];


    //Reset rest of UI
    for(uiCounterModifier=1;uiCounterModifier<UI_MODIFIER_AMOUNT+1;uiCounterModifier++)
    {
        uiUIModeActive[uiCounterModifier]=UI_MODIFIER_UNCHANGED;
    }

    uiUIModeActive[0]=UI_MODE_NO_SELECTION;
}

/*
 * Sets the Parameters of Mode uiMode to standard values for all active SLAs
 */
void vUIModeParameterStandardSet(void)
{
    uint8_t uiCounterSLA,uiCounterParameter;

    for(uiCounterSLA=0;uiCounterSLA<SETTINGS_SLA_AMOUNT;uiCounterSLA++)
    {
        if(uiUISLAActive[uiCounterSLA])
        {
            tsSettings[uiCounterSLA].uiModeActive[0]=uiUIModeActive[0];

            for(uiCounterParameter=0;uiCounterParameter<SETTINGS_PARAMETER_AMOUNT;uiCounterParameter++)
            {
                tsSettings[uiCounterSLA].uiModeParameter[0][uiCounterParameter]=uiSettingsParameterStandard[uiUIModeActive[0]][uiCounterParameter];
            }

            //test for actors
            tsSettings[uiCounterSLA].uiModeActors[0]=0;
            tsSettings[uiCounterSLA].uiModeActors[1]=0;
        }
    }
}

void vUIModifierParameterStandardSet(void)
{


}

void vUIModifierSelectionSwitch(void)
{
    if(uiUIModeActive[UI_MODIFIER_INDEX]==UI_MODIFIER_ACTIVATED)
    {
        //Unselect Modifier in UI
        uiUIModeActive[UI_MODIFIER_INDEX]=UI_MODIFIER_DEACTIVATED;
        //deactivate selected modifier in settings
        vUIModifierDeactivate();
    }
    else
    {
        //Select Modifier in UI
        uiUIModeActive[UI_MODIFIER_INDEX]=UI_MODIFIER_ACTIVATED;
        //Activate Modifier in settings and load standard parameters
        vUIModifierActivate();
    }
}

void vUIModifierActivate(void)
{
    uint8_t uiCounterSLA;
    uint8_t uiCounterParameter;

    for(uiCounterSLA=0;uiCounterSLA<SETTINGS_SLA_AMOUNT;uiCounterSLA++)
    {
        if(uiUISLAActive[uiCounterSLA])
        {
            tsSettings[uiCounterSLA].uiModeActive[UI_MODIFIER_INDEX]=0x1;

            for(uiCounterParameter=0;uiCounterParameter<SETTINGS_PARAMETER_AMOUNT;uiCounterParameter++)
            {
                tsSettings[uiCounterSLA].uiModeParameter[UI_MODIFIER_INDEX][uiCounterParameter]=uiSettingsParameterStandard[UI_MODIFIER_INDEX][uiCounterParameter];
            }
        }
    }
}

void vUIModifierDeactivate(void)
{
    uint8_t uiCounterSLA;

    for(uiCounterSLA=0;uiCounterSLA<SETTINGS_SLA_AMOUNT;uiCounterSLA++)
    {
        if(uiUISLAActive[uiCounterSLA])
        {
            tsSettings[uiCounterSLA].uiModeActive[UI_MODIFIER_INDEX]=0x0;
        }
    }
}


void vUIPresetLoad(void)
{
    uint8_t uiCounterSLA;

    for(uiCounterSLA=0;uiCounterSLA<SETTINGS_SLA_AMOUNT;uiCounterSLA++)
    {
            tsSettings[uiCounterSLA]=tsPresets[UI_PRESET_INDEX][uiCounterSLA];
    }

}

uint8_t uiUIParameterValueGet(void)
{
    uint8_t uiCounterSLA=0;

    if(uiUIModeTypeGet() == UI_STATE_TYPE_MODE)
    {
        while(uiCounterSLA<SETTINGS_SLA_AMOUNT)
        {
            if(uiUISLAActive[uiCounterSLA])
            {
                return tsSettings[uiCounterSLA].uiModeParameter[0][uiUIParameter];
            }
            uiCounterSLA++;
        }
    }
    else if(uiUIModeTypeGet() == UI_STATE_TYPE_MODIFIER)
    {
        while(uiCounterSLA<SETTINGS_SLA_AMOUNT)
        {
            if(uiUISLAActive[uiCounterSLA])
            {
                return tsSettings[uiCounterSLA].uiModeParameter[UI_MODIFIER_INDEX][uiUIParameter];
            }
            uiCounterSLA++;
        }
    }

    return 0;
}

void uiUIParameterValueSet(void)
{
    uint8_t uiCounterSLA=0;

    if(uiUIModeTypeGet() == UI_STATE_TYPE_MODE)
    {
        for(uiCounterSLA=0;uiCounterSLA<SETTINGS_SLA_AMOUNT;uiCounterSLA++)
        {
            if(uiUISLAActive[uiCounterSLA])
            {
                tsSettings[uiCounterSLA].uiModeParameter[0][uiUIParameter]=uiUIParameterValue;
            }
        }
    }
    else if(uiUIModeTypeGet() == UI_STATE_TYPE_MODIFIER)
    {
        for(uiCounterSLA=0;uiCounterSLA<SETTINGS_SLA_AMOUNT;uiCounterSLA++)
        {
            if(uiUISLAActive[uiCounterSLA])
            {
                tsSettings[uiCounterSLA].uiModeParameter[UI_MODIFIER_INDEX][uiUIParameter]=uiUIParameterValue;
            }
        }
    }
}
