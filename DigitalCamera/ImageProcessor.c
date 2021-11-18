// ImageProcessor.c
// Ryan Root, Pranesh Satish, Jacob Dach, Jeppe Lynge
// Initial Creation Date: 11/14/21
// Lab 08
// TA: Hassan Iqbal
// Interface between the OV7670 camera module and the rest of the embedded components (ie the ST7735, picture taking button, etc)
// Last Modified: 11/18/21

#include <stdint.h>
#include "ST7735.h"
#include "Bool.h"
#include <stdio.h>
#include <stdbool.h>
#include "ov7670Camera.h"

extern int OV7670_read_register(uint8_t reg);

void displayCameraCapture(){
	//interface the bmp/camera data with the ST7735 LCD to display a specific frame (this frame being the bmp image acquired from the camera)
	//occurs directly after taking a picture
	//different from displaying a previously taken picture
}

bool storeImage(){
	//interface the bmp/camera data with the ST7735 SDC Breakout Module --> Specifically for storing on the SDC
	//may be called from a button/main logic method (when the user specifies they want to store an image to the SDC)
}

bool takePicture(){
	//called from the button/blynk/its relevant file in order to specify that a frame/image capture should be taken
}

bool timedPicture(uint32_t seconds){
	//called from the button/blynk/its relevant file in order to specify that a frame/image capture should be taken with a countdown delay
	//requires initializing a system timer that is responsible for said countdowns
	//might be better implemented in another file since the timed picture may require interfacing the ST7735, speaker, and flash led
	//the above would also mean that there doesn't need to be 2 separate implementations of taking a picture -- only takePicture would be needed
}