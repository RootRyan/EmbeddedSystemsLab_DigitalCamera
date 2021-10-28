// StepperMotorController.h
// Runs on TM4C123
// Finite state machine to operate a stepper motor.
// Pranesh Satish, Jacob Dach, Ryan Root, Jeppe Lynge
// 10/28/2021

// Hardware connections (External: two input buttons and four outputs to stepper motor)
//  left input
// right input
//  PE4-0 are stepper motor outputs
//  PF1 PF2 or PF3 control the LED on Launchpad used as a heartbeat
//  PB6 is LED output (1 activates external LED on protoboard)

#include "SysTick.h"
#include "TExaS.h"
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

	uint8_t currentState;
	uint8_t input_t;
#define T100msec 10
struct state{
  uint8_t Out;
  uint32_t Delay;
  const struct State *Next[4];
};

typedef const struct State State_t;

#define S5  &fsm[0]
#define S6  &fsm[1]
#define S10 &fsm[2]
#define S9  &fsm[3]

State_t fsm[4]={
  {5, T100msec, S5, S6, S9, S5},
  {6, T100msec, S9,S10, S5, S6},
  {10,T100msec,S10, S9, S6,S10},
  {9, T100msec, S9, S5,S10, S9}
};
