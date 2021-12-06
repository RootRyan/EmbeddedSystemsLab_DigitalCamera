// UltimateMain.c

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "ST7735.h"
#include "Bool.h"
#include "ButtonFunctions.h"
//#include "Graphics.h"
//#include "Timers.h"
#include "PWM.h"
//#include "Speaker.h"
#include "ImageProcessor.h"
#include "I2C0.h"
#include "esp8266.h"
#include "Timer2.h"
#include "Timer3.h"
#include "StepperMotor.h"
#include "Camera_Port_Init.h"
#include "CortexM.h"
#include "TExaS.h"
#include <stdio.h>
#include "BlynkControl.h"

#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))
#define PA0       (*((volatile unsigned long *)0x40004004))


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

bool motorOn = false;
int NextSt = 0;
int capturePic = false;
uint32_t screenIdentifier = 0;

int main(void){
	DisableInterrupts();
	PLL_Init(Bus80MHz);
	Camera_Port_Init();
	ST7735_InitR(INITR_REDTAB);
  ST7735_SetRotation(1);
	StepperMotor_Init();
	PWM_Init(3,2);
	ST7735_FillScreen(ST7735_RED);
	//I2C_Init();

	ST7735_FillScreen(ST7735_YELLOW);
	//CameraSetup();

	ST7735_FillScreen(ST7735_GREEN);
	//Buttons_Init();
	
	EnableInterrupts();
	while(1){
		StreamImage();

		if(motorOn) {
			GPIO_PORTD_DATA_R = fsm[currentState].Out; //output to the motor
			
			SysTick_Wait10ms(fsm[currentState].Delay); //delay for how ever long each state runs for
			if(PA0 == 1)
				PA0 = 0;
			input_t = NextSt;		//take the inputs from the Blynk App
		}
	}
}
// End of Main
