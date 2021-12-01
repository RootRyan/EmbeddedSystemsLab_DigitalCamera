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
	//readImageStopFunctionPtr = 0; --> Set to display image on screen
	CameraInit();
}

void StreamImage() {
  bufferPos = 0;

  uint8_t r=0, g=0, b=0;
  uint16_t index = 0;
	//Wait for vsync
	while(!(PB1&1)){};//wait for high
  // read image
  for (int y = 0; y < 120; y++) {
  	for (int x = 0; x < 160; x++) {
			while((PB1&1)){};//wait for low
			r = ((PB7<<4)|(PB6<<3)|(PB5<<2)|(PB4<<1)|(PD7)) & 0x1F;
			g = ((PD6<<4)|(PD5<<3)|(PD4<<2)) & 0x1F;
			while(!(PB1&1)){};//wait for high
			while((PB1&1)){};//wait for low
  	  g = (g|(PB7<<2)|(PB6<<1)|(PB5)) & 0x1F;
			b = ((PB4<<4)|(PD7<<3)|(PD6<<2)|(PD5<<1)|(PD4)) & 0x1F;
			while(!(PB1&1)){};//wait for high

      index++;


  		buffer[bufferPos] = r;
  		buffer[bufferPos + 1] = g;
  		buffer[bufferPos + 2] = b;
  		bufferPos += 3;
  		if (bufferPos >= BUFFER_SIZE) {
  			bufferPos = 0;
  		}
  	}
  }

  //call to display on screen

}

bool timedPicture(uint32_t seconds){
	return false;
	//called from the button/blynk/its relevant file in order to specify that a frame/image capture should be taken with a countdown delay
	//requires initializing a system timer that is responsible for said countdowns
	//might be better implemented in another file since the timed picture may require interfacing the ST7735, speaker, and flash led
	//the above would also mean that there doesn't need to be 2 separate implementations of taking a picture -- only takePicture would be needed
}
