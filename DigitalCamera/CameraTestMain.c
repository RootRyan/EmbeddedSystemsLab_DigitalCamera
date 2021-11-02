//CameraTestMain.c

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "ST7735.h"
#include "Bool.h"
#include <stdio.h>
#include <string.h>
#include "Unified_Port_Init.h"
#include "OV7670.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode


//Sets up the camera initialization and gives a visual indicator of success and failure
void initializeScreenAndCamera() {
  bool cameraInitialized = init();
  ST7735_InitR(INITR_BLACKTAB);
  if (cameraInitialized) {
    // flash green screen if camera setup was successful
    ST7735_FillScreen(ST7735_GREEN);
    for (int delay = 0; delay <= 10000; delay++){
			__nop();
		}
    ST7735_FillScreen(ST7735_BLACK);
  } else {
    // red screen if failed to connect to camera
    ST7735_FillScreen(ST7735_RED);
    for (int delay = 0; delay <= 10000; delay++){
			__nop();
		}
  }
}

inline void sendLineToDisplay() __attribute__((always_inline));
inline void screenLineStart(void) __attribute__((always_inline));
inline void screenLineEnd(void) __attribute__((always_inline));
inline void sendPixelByte(uint8_t byte) __attribute__((always_inline));

// Normally it is a portrait screen. Use it as landscape
uint8_t screen_w = ST7735_TFTHEIGHT;
uint8_t screen_h = ST7735_TFTWIDTH;
uint8_t screenLineIndex;
uint16_t lineLength = 160;
uint16_t lineCount = 120;

//Continuously processes data frames received from the camera
void processFrame() {
  screenLineIndex = screen_h;

  DisableInterrupts();
  waitForVsync();
  ignoreVerticalPadding();

  for (uint8_t i = 0; i < lineCount; i++) {
        sendLineToDisplay();
  }
  EnableInterrupts();
}

//static const uint16_t byteCountForDisplay = getPixelBufferLength() < screen_w*2 ?
                                           // getPixelBufferLength() : screen_w*2;

void sendLineToDisplay() {
  if (screenLineIndex > 0) {
    screenLineStart();
    for (uint16_t i=0; i<lineCount; i++) {
      sendPixelByte(0);
    }
    screenLineEnd();
  }
}

void screenLineStart()   {
  if (screenLineIndex > 0) screenLineIndex--;
  //tft.startAddrWindow(screenLineIndex, 0, screenLineIndex, screen_w-1);
}

void screenLineEnd() {
  //tft.endAddrWindow();
}


void sendPixelByte(uint8_t byte) {
  //SPDR = byte;
  // this must be adjusted if sending loop has more/less instructions
  asm volatile("nop");
  asm volatile("nop");
  asm volatile("nop");
  asm volatile("nop");
  asm volatile("nop");
}


int main(void){
	DisableInterrupts();										
	PLL_Init(Bus80MHz);   
	Unified_Port_Init(); //--> Need to change for camera ports
	
	initializeScreenAndCamera();
	
	EnableInterrupts();

	while(1){
		processFrame();
	}
}