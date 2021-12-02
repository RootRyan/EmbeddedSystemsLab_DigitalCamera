//ButtonFunctions.c

#include <stdint.h>
#include "ST7735.h"
#include "Bool.h"
#include <stdio.h>
#include "OV7670.h"
#include "Bitmap.h"
#include "Timer4A.h"
#include "ImageProcessor.h"
#include "tm4c123gh6pm.h"

extern uint32_t screenIdentifier;
//0 = startup
//1 = constant streaming of data
//2 = settings/configuration menu
//3 = storage interaction menu (view old photos)

void Buttons_Init(void){
	
}
bool debounce[4];

#define CYCLE_WAIT 5

void Buttons_ReadInput(void) {
	/*uint32_t data = GPIO_PORTD_DATA_R;
	if((data & 0x) >> 4)
		debounce[0] = ((data & 0x) >> )*CYCLE_WAIT;
	if((data & 0x) != 0)
		debounce[1] = ((data & 0x) >> )*CYCLE_WAIT;
	if((data & 0x) != 0)
		debounce[2] = ((data & 0x) >> )*CYCLE_WAIT;
	if((data & 0x) != 0)
		debounce[3] = ((data & 0x) >> )*CYCLE_WAIT;*/
}

void Buttons_Pressed(uint32_t button) {
	if (button==0) {
		
	} else if (button==1) {
		
	} else if (button==2) {
		
	} else if (button==3) {
		
	}
}

void ChangeScreenFunction(uint32_t newScreen){
	//pretty much the same thing as setting the screen identifier except this enables it to be called on specific buttons
}


void Buttons_10ms_Handler(void) {
	Buttons_ReadInput();
	for(uint32_t i = 0; i < 4; i++) {
		if(debounce[i] > 0) {
			debounce[i] -= 1;
			if (debounce[i] == 0) {
				Buttons_Pressed(i);
			}
		}
	}
}
