#include "buttons.h"

#include "gpio.h"
#include "interrupt.h"

#include "hw_ints.h"

#include "ui_statemachine.h"
#include "quadrature_interface.h"
#include "delay.h"

uint8_t uiButtonsPushed;
uint8_t uiButtonsCounter;
uint8_t uiButtonsCounter1;

uint8_t uiButtonsDataFound;

uint8_t uiButtonsSLAAllActive;

void vButtonsInit()
{
    //Enable clock to the QEI input GPIO Port
    SysCtlPeripheralEnable(BUTTONS_SYSCTL_GPIO_PORT);
    //wait 3 clocksycles for SysCtlPeripheralEnable
    asm("\tnop;\r\n\tnop;\r\n\tnop;\r\n");

    //configure as standard pins
    GPIOPadConfigSet(BUTTONS_GPIO_PORT,BUTTONS_PINS,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD);

    //set as input
    GPIOPinTypeGPIOInput(BUTTONS_GPIO_PORT,BUTTONS_PINS);

    //Set interrupt on falling edge
    GPIOIntTypeSet(BUTTONS_GPIO_PORT,BUTTONS_PINS,GPIO_RISING_EDGE);

    //Set interrupt Priority to 2
    IntPrioritySet(INT_GPIOE,0x40);

    //register interrupthandler
    GPIOIntRegister(BUTTONS_GPIO_PORT,vButtonsHandler);

    //enable interrupt
    GPIOIntEnable(BUTTONS_GPIO_PORT,BUTTONS_PINS);

}

void vButtonsHandler(void)
{

uiButtonsPushed=GPIOPinRead(BUTTONS_GPIO_PORT,BUTTONS_PINS);

GPIOIntClear(BUTTONS_GPIO_PORT,BUTTONS_PINS);

switch(uiButtonsPushed)
{
case BUTTONS_PUSHED_INCREMENTER://INCREMENTER IS PUSHED
    switch(uiUIState)
    {
        case UI_STATE_SLA_SELECTION: //constant state

                                     uiUISLAActive[uiUISLA]=!uiUISLAActive[uiUISLA];

                                     for(uiButtonsCounter=0;uiButtonsCounter<UI_MODIFIER_MAX_VALUE-UI_MODE_MAX_VALUE;uiButtonsCounter++)
                                     {
                                         uiUIModifierSelection[uiButtonsCounter]=UI_MODIFIER_UNCHANGED;
                                     }

                                     uiUIModeActive=UI_MODE_NO_SELECTION;
        break;
        case UI_STATE_MODE_SELECTION: //constant state
                                      if(uiUIMode<UI_MODE_MAX_VALUE)//Mode selected
                                      {
                                          uiUIModeActive=uiUIMode;

                                          for(uiButtonsCounter=0;uiButtonsCounter<SETTINGS_SLA_AMOUNT;uiButtonsCounter++)
                                          {
                                              if(uiUISLAActive[uiButtonsCounter])
                                              {
                                                  tsSettings[uiButtonsCounter].uiMode=uiUIModeActive;

                                                  for(uiButtonsCounter1=0;uiButtonsCounter1<SETTINGS_PARAMETER_AMOUNT;uiButtonsCounter1++)
                                                  {
                                                      tsSettings[uiButtonsCounter].uiParameterValue[uiButtonsCounter1]=uiSettingsModeParameterStandard[uiButtonsCounter][uiButtonsCounter1];
                                                  }
                                              }
                                          }
                                      }
                                      else if(uiUIMode<UI_MODIFIER_MAX_VALUE)//Modifier activated or deactivated
                                      {
                                          if(uiUIModifierSelection[uiUIMode-UI_MODE_MAX_VALUE]==UI_MODIFIER_ACTIVATED)
                                          {
                                              uiUIModifierSelection[uiUIMode-UI_MODE_MAX_VALUE]=UI_MODIFIER_DEACTIVATED;
                                              for(uiButtonsCounter=0;uiButtonsCounter<SETTINGS_SLA_AMOUNT;uiButtonsCounter++)
                                              {
                                                  if(uiUISLAActive[uiButtonsCounter])
                                                  {
                                                      tsSettings[uiButtonsCounter].uiModifier[uiUIMode-UI_MODE_MAX_VALUE][0]=0x0;
                                                  }
                                              }
                                          }
                                          else
                                          {
                                              uiUIModifierSelection[uiUIMode-UI_MODE_MAX_VALUE]=UI_MODIFIER_ACTIVATED;
                                              for(uiButtonsCounter=0;uiButtonsCounter<SETTINGS_SLA_AMOUNT;uiButtonsCounter++)
                                              {
                                                  if(uiUISLAActive[uiButtonsCounter])
                                                  {
                                                      for(uiButtonsCounter1=0;uiButtonsCounter1<SETTINGS_PARAMETER_AMOUNT;uiButtonsCounter1++)
                                                      {
                                                          tsSettings[uiButtonsCounter].uiModifier[uiUIMode-UI_MODE_MAX_VALUE][uiButtonsCounter1]=uiSettingsModifierParameterStandard[uiUIMode-UI_MODE_MAX_VALUE][uiButtonsCounter1];
                                                      }
                                                  }
                                              }
                                          }

                                      }
                                      else if(uiUIMode<UI_PRESET_MAX_VALUE)//Preset selected
                                      {
                                          for(uiButtonsCounter=0;uiButtonsCounter<SETTINGS_SLA_AMOUNT;uiButtonsCounter++)
                                          {
                                                  tsSettings[uiButtonsCounter]=tsPresets[uiUIMode][uiButtonsCounter];
                                          }
                                      }

        break;
        case UI_STATE_PARAMETER_SELECTION: uiUIState=UI_STATE_PARAMETER_SELECTED;

                                           uiButtonsDataFound=0;
                                           uiButtonsCounter=0;
                                           while(!uiButtonsDataFound && uiButtonsCounter<SETTINGS_SLA_AMOUNT)
                                           {
                                               if(uiUISLAActive[uiButtonsCounter])
                                               {
                                                   uiUIParameterValue=tsSettings[uiButtonsCounter].uiParameterValue[uiUIParameter];
                                                   uiButtonsDataFound=1;
                                               }
                                               uiButtonsCounter++;
                                           }
                                           vQEIPositionSet(uiUIParameterValue);

        break;
        case UI_STATE_PARAMETER_SELECTED: uiUIState=UI_STATE_PARAMETER_SELECTION;
                                          if(uiUIMode<=UI_MODE_MAX_VALUE || uiUIMode>UI_MODIFIER_MAX_VALUE)
                                          {
                                              for(uiButtonsCounter=0;uiButtonsCounter<SETTINGS_SLA_AMOUNT;uiButtonsCounter++)
                                              {
                                                  if(uiUISLAActive[uiButtonsCounter])
                                                  {
                                                      tsSettings[uiButtonsCounter].uiParameterValue[uiUIParameter]=uiUIParameterValue;
                                                  }
                                              }
                                          }
                                          else
                                          {
                                              for(uiButtonsCounter=0;uiButtonsCounter<SETTINGS_SLA_AMOUNT;uiButtonsCounter++)
                                              {
                                                  if(uiUISLAActive[uiButtonsCounter])
                                                  {
                                                      tsSettings[uiButtonsCounter].uiModifier[uiUIMode-UI_MODE_MAX_VALUE][uiUIParameter]=uiUIParameterValue;
                                                  }
                                              }
                                          }
                                          vQEIPositionSet(uiUIParameter);
        break;
    }
break;
case BUTTONS_PUSHED_RIGHT://RIGHT BUTTON PUSHED
    switch(uiUIState)
    {
        case UI_STATE_SLA_SELECTION: uiUIState=UI_STATE_MODE_SELECTION;
                                     vQEIPositionSet(uiUIMode);
        break;
        case UI_STATE_MODE_SELECTION:
                                      if(uiUIModeActive != UI_MODE_NO_SELECTION || (uiUIMode>=UI_MODE_MAX_VALUE && uiUIMode<=UI_MODIFIER_MAX_VALUE && uiUIModifierSelection[uiUIMode-UI_MODE_MAX_VALUE]==1))
                                      {
                                          uiUIState=UI_STATE_PARAMETER_SELECTION;
                                          vQEIPositionSet(uiUIParameter);
                                      }
        break;
        case UI_STATE_PARAMETER_SELECTION: uiUIState=UI_STATE_SLA_SELECTION;
                                           vQEIPositionSet(uiUISLA);
        break;
    }
break;
case BUTTONS_PUSHED_LEFT://LEFT BUTTON PUSHED
    switch(uiUIState)
    {
        case UI_STATE_SLA_SELECTION:
                                      if(uiUIModeActive != UI_MODE_NO_SELECTION || (uiUIMode>=UI_MODE_MAX_VALUE && uiUIMode<=UI_MODIFIER_MAX_VALUE && uiUIModifierSelection[uiUIMode-UI_MODE_MAX_VALUE]==1))
                                      {
                                          uiUIState=UI_STATE_PARAMETER_SELECTION;
                                          vQEIPositionSet(uiUIParameter);
                                      }
        break;
        case UI_STATE_MODE_SELECTION:
                                          uiUIState=UI_STATE_SLA_SELECTION;
                                          vQEIPositionSet(uiUIParameter);
        break;
        case UI_STATE_PARAMETER_SELECTION: uiUIState=UI_STATE_MODE_SELECTION;
                                           vQEIPositionSet(uiUIMode);
        break;
    }
break;
}

if(uiUIState==UI_STATE_STANDBY) //WAKE UP
{
    uiUIState=UI_STATE_SLA_SELECTION;
}

uiSystemStandbyCounterSeconds=uiSystemStandbyTimeSeconds;

vUIUpdate();    //update UI with new state

//vDelayMiliSec(100);

GPIOIntClear(BUTTONS_GPIO_PORT,BUTTONS_PINS);

}
