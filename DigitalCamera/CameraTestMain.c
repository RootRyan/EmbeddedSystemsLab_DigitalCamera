//CameraTestMain.c

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "ST7735.h"
#include "Bool.h"
#include <stdio.h>
#include <string.h>
#include "Unified_Port_Init.h"
#include "I2C0.h"

#define PF2             (*((volatile uint32_t *)0x40025010))

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

int main(void){
	DisableInterrupts();										
	PLL_Init(Bus80MHz);   
	Unified_Port_Init(); //--> Need to change for camera ports
	I2C_Init();
	for (int delay = 0; delay <= 100000; delay++){
			__nop();
	}
	//OV7670_begin(OV7670_COLOR_RGB, OV7670_SIZE_DIV4, 15);
	PF2 = 0;
	EnableInterrupts();
	
	while(1){
		PF2 ^= 1;
	}
}
