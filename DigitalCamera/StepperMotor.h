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

#define S1  &fsm[0]
#define S2  &fsm[1]
#define S4  &fsm[2]
#define S8  &fsm[3]
#define S16 &fsm[4]

State_t fsm[5]={
  {1, T100msec, S1, S2, S16, S1},
  {2, T100msec, S2, S4, S1, S2},
  {4, T100msec, S4, S8, S2, S4},
  {8, T100msec, S8, S16, S4, S8},
  {16,T100msec, S16, S1, S8, S16}
};
