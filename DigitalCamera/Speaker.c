// Speaker.c
// Ryan Root, Pranesh Satish, Jacob Dach
// Initial Creation Date: 9/15/21
// Lab 03
// TA: Hassan Iqbal
// Description: Enable and Disable Speaker
// Last Modified: 9/24/21

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Bool.h"

// This function enables and disables speaker output
// Outputs: None
// Inputs: enable signal: 1 = on, 0 = off
// Assumes Timer0A Interrupt has been initialized.

void SpeakerEnable(bool enable) {
	// this effectively turns the speaker on and off
	if(enable)
		PWM0_ENABLE_R |= 0x01;
	else
		PWM0_ENABLE_R &= ~0x01;
}
