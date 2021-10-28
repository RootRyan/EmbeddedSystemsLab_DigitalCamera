// Timers.h
// Ryan Root, Pranesh Satish, Jacob Dach
// Initial Creation Date: 9/15/21
// Lab 03
// TA: Hassan Iqbal
// Description: Initialize timers used for clock
// Last Modified: 9/15/21

#ifndef __TIMERSINTS_H__ // do not include more than once
#define __TIMERSINTS_H__

void Timer0A_Init(uint32_t period);

// ***************** Timer1_Init ****************
// Activate Timer1 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
// Outputs: none
void Timer1_Init(void);

#endif
