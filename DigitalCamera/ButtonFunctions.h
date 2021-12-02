// Buttons.h
// Ryan Root, Pranesh Satish, Jacob Dach
// Initial Creation Date: 9/15/21
// Lab 03
// TA: Hassan Iqbal
// Description: Adds functionality for all tactile switches used
// Last Modified: 9/24/21
#ifndef ButtonFunctions_h
#define ButtonFunctions_h 1

#include <stdint.h>

extern uint32_t screenIdentifier;
//0 = startup
//1 = constant streaming of data
//2 = settings/configuration menu
//3 = storage interaction menu (view old photos)

void Buttons_Init(void);

void ChangeScreenFunction(uint32_t newScreen); //pretty much the same thing as setting the screen identifier except this enables it to be called on specific buttons

void Buttons_10ms_Handler(void);

#endif