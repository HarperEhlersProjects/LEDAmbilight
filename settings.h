#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "stdint.h"

#define SETTINGS_SLA_AMOUNT 8
#define SETTINGS_MODE_AMOUNT 2
#define SETTINGS_MODIFIER_AMOUNT 1
#define SETTINGS_PARAMETER_AMOUNT 6

#define SETTINGS_PRESET_AMOUNT 10

typedef struct{
uint8_t uiMode;
uint8_t uiModifier[SETTINGS_MODIFIER_AMOUNT][SETTINGS_PARAMETER_AMOUNT];


uint8_t uiParameterValue[SETTINGS_PARAMETER_AMOUNT];

}tsSettingsStruct;

tsSettingsStruct tsSettings[SETTINGS_SLA_AMOUNT];

//Preset 0 is reserved for initial Settings
tsSettingsStruct tsPresets[SETTINGS_PRESET_AMOUNT][SETTINGS_SLA_AMOUNT];

//variable for standard mode parameters
uint8_t uiSettingsModeParameterStandard[SETTINGS_MODE_AMOUNT][SETTINGS_PARAMETER_AMOUNT];
uint8_t uiSettingsModifierParameterStandard[SETTINGS_MODIFIER_AMOUNT][SETTINGS_PARAMETER_AMOUNT];
void vSettingsInit(void);

#endif /* SETTINGS_H_ */
