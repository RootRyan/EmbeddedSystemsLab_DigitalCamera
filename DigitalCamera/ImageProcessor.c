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
#include "OV7670.h"

//vsync
#define PB1       (*((volatile uint32_t *)0x40005008))

//pixel data bits 0-3
#define PD4       (*((volatile uint32_t *)0x40007040))
#define PD5       (*((volatile uint32_t *)0x40007080))
#define PD6       (*((volatile uint32_t *)0x40007100))
#define PD7       (*((volatile uint32_t *)0x40007200))

//pixel data bits 4-7
#define PB4       (*((volatile uint32_t *)0x40005040))     
#define PB5       (*((volatile uint32_t *)0x40005080))     
#define PB6       (*((volatile uint32_t *)0x40005100))     
#define PB7       (*((volatile uint32_t *)0x40005200))

void (*bufferFullFunctionPtr)(uint8_t *);
void (*readImageStartFunctionPtr)(void);
void (*readImageStopFunctionPtr)(void);
uint16_t bufferPos = 0;
static const uint16_t BUFFER_SIZE = 480;
uint8_t buffer[BUFFER_SIZE];
void CameraSetup(){
	bufferPos = 0;
	bufferFullFunctionPtr = 0;
	readImageStartFunctionPtr = 0;
	readImageStopFunctionPtr = 0;
	CameraInit();
}

void captureImage() {
	
  bufferPos = 0;
  if (readImageStartFunctionPtr) {
  	(*readImageStartFunctionPtr)();
  }

  uint8_t r=0, g=0, b=0, d1 = 0, d2 = 0, d3 = 0, d4 = 0;
  uint16_t index = 0;
	//Wait for vsync it is on pin 3 (counting from 0) portD
	while(!(PB1&1)){};//wait for high
  // read image
  for (int y = 0; y < 120; y++) {
  	for (int x = 0; x < 160; x++) {
			while((PB1&1)){};//wait for low
			b=((PB7<<7)|(PB6<<6)|(PB5<<5)|(PB4<<4)|(PB7<<3)|(PB6<<2)|(PB5<<1)|(PB4));
			g=0;
			r=0;
			while(!(PB1&1)){};//wait for high
  	  b = (d1 & 0x1F) << 3;
			g = (((d1 & 0xE0) >> 3) | ((d2 & 0x07) << 5));
  	  r = (d2 & 0xF8);

      index++;


  		buffer[bufferPos] = r;
  		buffer[bufferPos + 1] = g;
  		buffer[bufferPos + 2] = b;
  		bufferPos += 3;
  		if (bufferPos >= BUFFER_SIZE) {
  			if (bufferFullFunctionPtr) {
  				(*bufferFullFunctionPtr)(buffer);
  			}

  			bufferPos = 0;
  		}
  	}
  }

  if (readImageStopFunctionPtr) {
  	(*readImageStopFunctionPtr)();
  }

}

bool timedPicture(uint32_t seconds){
	//called from the button/blynk/its relevant file in order to specify that a frame/image capture should be taken with a countdown delay
	//requires initializing a system timer that is responsible for said countdowns
	//might be better implemented in another file since the timed picture may require interfacing the ST7735, speaker, and flash led
	//the above would also mean that there doesn't need to be 2 separate implementations of taking a picture -- only takePicture would be needed
}