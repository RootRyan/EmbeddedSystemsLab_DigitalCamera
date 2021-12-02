//BlynkControl.h

#ifndef BLYNKCONTROL_H
#define BLYNKCONTROL_H

#include <stdint.h>

void blynkInit(void);

void TM4C_to_Blynk(uint32_t pin,uint32_t value);

void Blynk_to_TM4C(void);

void sendInformation(void);

#endif
