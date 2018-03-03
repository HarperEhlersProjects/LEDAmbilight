#include "settings.h"


uint16_t uiSettingsCounter;

void vSettingsInit(void)
{

    for(uiSettingsCounter=0;uiSettingsCounter<SETTINGS_SLA_AMOUNT;uiSettingsCounter++)
    {
        tsSettings[uiSettingsCounter].uiMode=0;
    }

    //Standard values for mode 0
    uiSettingsModeParameterStandard[0][0]=0;
    uiSettingsModeParameterStandard[0][1]=0;
    uiSettingsModeParameterStandard[0][2]=0;
    uiSettingsModeParameterStandard[0][3]=0;
    uiSettingsModeParameterStandard[0][4]=0;
    uiSettingsModeParameterStandard[0][5]=0;

    //Standard values for mode 1
    uiSettingsModeParameterStandard[1][0]=0;
    uiSettingsModeParameterStandard[1][1]=0;
    uiSettingsModeParameterStandard[1][2]=0;
    uiSettingsModeParameterStandard[1][3]=0;
    uiSettingsModeParameterStandard[1][4]=0;
    uiSettingsModeParameterStandard[1][5]=0;

    //Standard values for modifier 0
    uiSettingsModifierParameterStandard[2][0]=1;    //Modifier active
    uiSettingsModifierParameterStandard[2][1]=0;
    uiSettingsModifierParameterStandard[2][2]=0;
    uiSettingsModifierParameterStandard[2][3]=0;
    uiSettingsModifierParameterStandard[2][4]=0;
    uiSettingsModifierParameterStandard[2][5]=0;
}


