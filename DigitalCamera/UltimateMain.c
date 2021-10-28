// UltimateMain.c

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "ST7735.h"
#include "Bool.h"
#include "Buttons.h"
#include "Graphics.h"
#include "Timers.h"
#include "PWM.h"
#include "Speaker.h"
#include "Unified_Port_Init.h"
#include "../inc/LaunchPad.h"
#include "../inc/CortexM.h"
#include "../inc/TExaS.h"
#include <stdio.h>
#include <string.h>

#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

int main(void){
	DisableInterrupts();										
	PLL_Init(Bus80MHz);   
	Unified_Port_Init();            

	while(1){
		
	}
}
// End of Main
