// ImageProcessor.h
// Ryan Root, Pranesh Satish, Jacob Dach, Jeppe Lynge
// Initial Creation Date: 11/14/21
// Lab 08
// TA: Hassan Iqbal
// Interface between the OV7670 camera module and the rest of the embedded components (ie the ST7735, picture taking button, etc)
// Last Modified: 11/18/21

#ifndef ImageProcessor_h
#define ImageProcessor_h 1

#include <stdint.h>
#include "Bool.h"

void CameraSetup(void);

void displayCameraCapture(void);

bool storeImage(void);

bool takePicture(void);

bool timedPicture(uint32_t);

#endif