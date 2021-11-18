#include "SysTick.h"
#include "TExaS.h"
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

volatile int delay;

void EnableInterrupts(void);
// edit the following only if you need to move pins from PA4, PE3-0
// logic analyzer on the real board
#define PA4       (*((volatile unsigned long *)0x40004040))
#define PE50      (*((volatile unsigned long *)0x400240FC))
void SendDataToLogicAnalyzer(void){
  UART0_DR_R = 0x80|(PA4<<2)|PE50;
}

int Stepper_Check(void){
  //TExaS_Init(&SendDataToLogicAnalyzer);    // activate logic analyzer and set system clock to 80 MHz
  SysTick_Init();
// you initialize your system here
	SYSCTL_RCGCGPIO_R |= 0x3A; //initialize each port used
	delay++;

	GPIO_PORTE_DIR_R |= 0x1F;
	GPIO_PORTE_DEN_R |= 0x1F;

	GPIO_PORTD_DIR_R &= 0xFC;
	GPIO_PORTD_DEN_R |= 0x03;

	GPIO_PORTB_DIR_R |= 0x40;
	GPIO_PORTB_DEN_R |= 0x40;

	GPIO_PORTF_DIR_R |= 0x02;
	GPIO_PORTF_DEN_R |= 0x02;


	EnableInterrupts();
  while(1){
// output
// wait
// input
// next
		GPIO_PORTF_DATA_R ^= 0x02; //toggle the heartbeat LED

		GPIO_PORTE_DATA_R = fsm[currentState].Out; //output to the motor
		//GPIO_PORTB_DATA_R = fsm[current].LED; //toggle the light when it is on

		//SysTick_Wait10ms(FSM[current].delay); //delay for how ever long each state runs for

		input_t = (GPIO_PORTD_DATA_R);		//take the inputs and shift them right
		currentState = fsm[currentState].Next[input_t]; //use the new value as the index of the next state graph
  }
}


void StepperMotor_Init() {
  SYSCTL_RCGCGPIO_R |= 0x3A; //initialize each port used
	delay++;

	GPIO_PORTE_DIR_R |= 0x1F;
	GPIO_PORTE_DEN_R |= 0x1F;

	GPIO_PORTD_DIR_R &= 0xFC;
	GPIO_PORTD_DEN_R |= 0x03;

	GPIO_PORTB_DIR_R |= 0x40;
	GPIO_PORTB_DEN_R |= 0x40;

	GPIO_PORTF_DIR_R |= 0x02;
	GPIO_PORTF_DEN_R |= 0x02;
}
