// OV7670.h
// Ryan Root, Pranesh Satish, Jacob Dach, Jeppe Lynge
// Initial Creation Date: 10/28/21
// Lab 08
// TA: Hassan Iqbal
// Low level drivers for the OV7670 Camera Module
// Last Modified: 11/28/21

#ifndef _OV7670_h_
#define _OV7670_h_

#include <stdint.h>
#include "Bool.h"

#define OV7670_XCLK_HZ 16000000 ///< XCLK to camera, 8-24 MHz

void CameraInit(void);
bool ResetOV7670Registers(void);
void SetManualContrastCenter(uint8_t);
void SetContrast(uint8_t);
void SetBrightness(uint8_t);
void ReversePixelBits(void);
void SetShowColorBar(bool);

#endif