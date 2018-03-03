#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "stdint.h"
#include "led_controller_interface.h"

typedef struct{
uint8_t uiRed;
uint8_t uiGreen;
uint8_t uiBlue;
}tsColorStruct;

void vGraphicsModeCalc();
void vGraphicsTransmissionInitiate();
void vGraphicsLedSet(uint8_t,uint8_t,tsColorStruct);

void vGraphicsMode0(uint8_t);
void vGraphicsMode1(uint8_t);
#endif /* GRAPHICS_H_ */
