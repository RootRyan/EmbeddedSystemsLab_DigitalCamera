// Buttons.h
// Ryan Root, Pranesh Satish, Jacob Dach
// Initial Creation Date: 9/15/21
// Lab 03
// TA: Hassan Iqbal
// Description: Adds functionality for all tactile switches used in clock
// Last Modified: 9/24/21
#ifndef Buttons_h
#define Buttons_h 1

#include <stdint.h>

void Buttons_Init(void);
uint32_t Buttons_getProperty(void);

#endif
