#include "graphics.h"

#include "led_controller_interface.h"
#include "settings.h"

uint16_t uiGraphicsCounter;
uint16_t uiGraphicsCounter1;

uint8_t uiGraphicsBitField[60][BIT_SEQUENCE_LENGTH];

void vGraphicsModeCalc()
{
    for(uiGraphicsCounter=0;uiGraphicsCounter<SETTINGS_SLA_AMOUNT;uiGraphicsCounter++)
    {
        switch(tsSettings[uiGraphicsCounter].uiMode)
        {
            case 0:vGraphicsMode0(uiGraphicsCounter);//everything clear

            case 1:vGraphicsMode1(uiGraphicsCounter);//constant RGB
        }
    }

}

void vGraphicsTransmissionInitiate(void)
{
    while(uiLEDCITransmissionRun);

    for(uiGraphicsCounter=0;uiGraphicsCounter<60;uiGraphicsCounter++)
    {
        for(uiGraphicsCounter1=0;uiGraphicsCounter1<24;uiGraphicsCounter1++)
        {
            uiLEDCIBitField[uiGraphicsCounter][uiGraphicsCounter1]=uiGraphicsBitField[uiGraphicsCounter][uiGraphicsCounter1];
        }
    }

    uiLEDCITransmissionRun=1;

    for(uiGraphicsCounter=0;uiGraphicsCounter<60;uiGraphicsCounter++)
    {
        for(uiGraphicsCounter1=0;uiGraphicsCounter<24;uiGraphicsCounter++)
        {
            uiGraphicsBitField[uiGraphicsCounter][uiGraphicsCounter1]=0;
        }
    }
}

void vGraphicsLedSet(uint8_t uiSLAMask,uint8_t uiLED,tsColorStruct tsColor)
{
uint8_t i;
for(i=0;i<8;i++)
{
    if(tsColor.uiRed & (0x80 >> i))
    {
        uiGraphicsBitField[uiLED][i+8]|=uiSLAMask;
    }
    else
    {
        uiGraphicsBitField[uiLED][i+8]&=~uiSLAMask;
    }

    if(tsColor.uiGreen & (0x80 >> i))
    {
        uiGraphicsBitField[uiLED][i]|=uiSLAMask;
    }
    else
    {
        uiGraphicsBitField[uiLED][i]&=~uiSLAMask;
    }

    if(tsColor.uiBlue & (0x80 >> i))
    {
        uiGraphicsBitField[uiLED][i+16]|=uiSLAMask;
    }
    else
    {
        uiGraphicsBitField[uiLED][i+16]&=~uiSLAMask;
    }
}

}

void vGraphicsMode0(uint8_t uiSLA)
{


}

void vGraphicsMode1(uint8_t uiSLA)
{
    tsColorStruct tsColor={tsSettings[uiSLA].uiParameterValue[0],tsSettings[uiSLA].uiParameterValue[1],tsSettings[uiSLA].uiParameterValue[2]};
    uint8_t uiCounter;

    for(uiCounter=0;uiCounter<60;uiCounter++)
    {
        vGraphicsLedSet(1<<uiSLA,uiCounter,tsColor);
    }
}


