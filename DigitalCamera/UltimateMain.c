// UltimateMain.c

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "ST7735.h"
#include "Bool.h"
#include "Buttons.h"
#include "Graphics.h"
#include "Timers.h"
#include "PWM.h"
#include "Speaker.h"
#include "ImageProcessor.h"
#include "I2C0.h"
#include "esp8266.h"
#include "Timer2.h"
#include "Timer3.h"
//#include "StepperMotor.h"
#include "Unified_Port_Init.h"
#include <stdio.h>
#include <string.h>

#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

bool motorOn = false;

int main(void){
	DisableInterrupts();
	PLL_Init(Bus80MHz);
	Unified_Port_Init();
	ST7735_InitR(INITR_REDTAB);
	//StepperMotor_Init();
	
	I2C_Init();
	for (int delay = 0; delay <= 100000; delay++){
			__nop();
	}
	CameraSetup();

	ESP8266_Init();
	ESP8266_Reset();
	ESP8266_SetupWiFi();

	Buttons_Init();

	Timer2_Init(&Blynk_to_TM4C,800000);
  // check for receive data from Blynk App every 10ms

  Timer3_Init(&SendInformation,4000000);

	EnableInterrupts();
	while(1){
		StreamImage();

		if(motorOn) {
			GPIO_PORTE_DATA_R = fsm[currentState].Out; //output to the motor
			//GPIO_PORTB_DATA_R = fsm[current].LED; //toggle the light when it is on

			//SysTick_Wait10ms(FSM[current].delay); //delay for how ever long each state runs for

			input_t = (GPIO_PORTD_DATA_R);		//take the inputs and shift them right
			currentState = fsm[currentState].Next[input_t]; //use the new value as the index of the next state graph
		}
	}
}
// End of Main
