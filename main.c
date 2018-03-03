#include <stdint.h>
#include <stdbool.h>

#include "interrupt.h"
#include "timer.h"

#include "system_set_up.h"
#include "delay.h"
#include "led_controller_interface.h"
#include "quadrature_interface.h"
#include "i2c_interface.h"
#include "buttons.h"
#include "graphics.h"

#include "ui_statemachine.h"
#include "settings.h"

uint8_t uiCounter=0;

void main(void)
{

    IntMasterDisable();
    vSystemSetUp();
    vSettingsInit();
    vDelayInit();
    vLEDControllerInterfaceInit();
    vQEIInit();
    vI2CInterfaceInit();
    vButtonsInit();
    vUIInit();
    IntMasterEnable();

    uiLEDCIOutputMask=0x1;

    while(1)
    {
        //vDelaySec(1);
        vGraphicsModeCalc();
        vGraphicsTransmissionInitiate();

    }
}

