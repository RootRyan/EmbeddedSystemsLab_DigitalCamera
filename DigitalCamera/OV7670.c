// OV7670.c
// Ryan Root, Pranesh Satish, Jacob Dach, Jeppe Lynge
// Initial Creation Date: 10/28/21
// Lab 08
// TA: Hassan Iqbal
// Low level drivers for the OV7670 Camera Module
// Last Modified: 10/28/21

/* Pin Connections
VSYNC/VS - PB1
RRST/RESET - PA1
PCLCK - PIN12
SIOD/SDA - PB3 (I2C data) - 10K resistor to 3.3V
SIOC/SCL - PB2 (I2C clock) - 10K resistor to 3.3V
D0..D3 - PD4..7 (pixel data bits 0..3)
D4..D7 - PB4..7 (pixel data bits 4..7)
*/
#include <stdint.h>
#include "OV7670.h"
#include "ST7735.h"
#include "Bool.h"
#include <stdio.h>

uint32_t pixelFormat;
uint32_t resolution;
uint32_t verticalPadding;
uint32_t internalClockPreScaler;
uint32_t pllMultiplier;

bool init() {
  //Be sure to initialize global variables before this
  //Be sure to initialize ports before this
  for (int delay = 0; delay <= 1000; delay++){
			__nop();
	}
  return setUpCamera();
}


bool setUpCamera() {
	//ST7735 Uses RGB565
  pixelFormat = PIXEL_RGB565;
	resolution = RESOLUTION_QQVGA_160x120; //ST7735 is 128x160
  setDisablePixelClockDuringBlankLines();
  setDisableHREFDuringBlankLines();
  setInternalClockPreScaler(internalClockPreScaler);
	setPLLMultiplier(pllMultiplier);
}

bool setRegister(uint8_t addr, uint8_t val) {
  
}

uint8_t readRegister(uint8_t addr) {
  
}

void setRegisterBitsOR(uint8_t addr, uint8_t bits) {
  
}

void setRegisterBitsAND(uint8_t addr, uint8_t bits) {
  
}

void setManualContrastCenter(uint8_t contrastCenter) {
  
}


void setContrast(uint8_t contrast) {
  
}


void setBrightness(uint8_t birghtness) {
  
}


void reversePixelBits() {
  
}

void showColorBars(bool transparent) {

}

void ignoreVerticalPadding() {
  for (uint8_t i = 0; i < verticalPadding; i++) {
    ignoreHorizontalPaddingLeft();
    for (uint16_t x = 0; x < resolution * 2; x++) {
      waitForPixelClockRisingEdge();
    }
    ignoreHorizontalPaddingRight();
  }
}