//// Buttons.c
//// Ryan Root, Pranesh Satish, Jacob Dach
//// Initial Creation Date: 9/15/21
//// Lab 03
//// TA: Hassan Iqbal
//// Description: Adds functionality for all tactile switches used in clock
//// Last Modified: 9/24/21

//#include <stdint.h>
//#include "../inc/tm4c123gh6pm.h"
//#include "Bool.h"
//#include "Buttons.h"
//#include "Timers.h"
//#include "Speaker.h"

//#define GPIO_PORTC_DATA_R       (*((volatile uint32_t *)0x400063FC))

//extern uint32_t Hours_current;						// holds the current hour
//extern uint32_t Mins_current;						// holds the current minute
//extern uint32_t Hours_old;
//extern uint32_t Mins_old;
//extern uint32_t Hours_alarm;
//extern uint32_t Mins_alarm;
//extern uint32_t Hours_stop;
//extern uint32_t Mins_stop;
//extern uint32_t Secs_stop;
//extern bool Speaker_active;

//// 0-23 hours
//uint32_t hours = 0;
//// 0-59 minutes
//uint32_t minutes = 0;
//bool set_alarm = false;
//bool set_time = false;
//bool start_stopwatch = false;
//bool Alarm_active = false;

//bool debounce[4];

//uint32_t Buttons_Hours(void) {return hours;}
//uint32_t Buttons_Minutes(void) {return minutes;}
//bool Buttons_SetAlarmMode(void) {return Alarm_active;}
//bool Buttons_Stopwatch(void) {return start_stopwatch;}

//#define CYCLE_WAIT 5

//void Buttons_ReadInput(void) {
//	uint32_t data = GPIO_PORTC_DATA_R;
//	if((data & 0x10) >> 4)
//		debounce[0] = ((data & 0x10) >> 4)*CYCLE_WAIT;
//	if((data & 0x20) != 0)
//		debounce[1] = ((data & 0x20) >> 5)*CYCLE_WAIT;
//	if((data & 0x40) != 0)
//		debounce[2] = ((data & 0x40) >> 6)*CYCLE_WAIT;
//	if((data & 0x80) != 0)
//		debounce[3] = ((data & 0x80) >> 7)*CYCLE_WAIT;
//}

//void Buttons_Pressed(uint32_t button) {
//	if (button==0) {
//		if (set_alarm) {
//			Hours_alarm = (Hours_alarm + 1) % 12;
//			if (Hours_alarm == 0) {
//					Hours_alarm = 12;
//			}
//		} else	if (!set_time) {
//			set_time = true;
//		} else if (set_time) {
//			Hours_old = Hours_current;
//			Hours_current = (Hours_current + 1) % 12;
//		}
//	} else if (button==1) {
//		if (set_time) {
//			if(Mins_current+1 >= 60) {
//				Hours_old = Hours_current;
//				Hours_current = (Hours_current + 1) % 12;
//			}
//			Mins_old = Mins_current;
//			Mins_current = (Mins_current + 1) % 60;
//		} else if (!set_alarm) {
//			set_alarm = true;
//		} else if (set_alarm) {
//			if(Mins_alarm+1 >= 60) {
//				Hours_alarm = (Hours_alarm + 1) % 12;
//				if (Hours_alarm == 0) {
//					Hours_alarm = 12;
//				}
//			}
//			Mins_alarm = (Mins_alarm + 1) % 60;
//		}
//	} else if (button==2) {
//		if (Speaker_active){
//			SpeakerEnable(false);
//			Alarm_active = false;
//		}
//		start_stopwatch = !start_stopwatch;
//		
//	} else if (button==3) {
//		if (set_time) {
//			set_time = false;
//		} else if (set_alarm) {
//			set_alarm = false;
//		} else if (Speaker_active) {
//			SpeakerEnable(false);
//			if(Mins_alarm+10 >= 60) {
//				Hours_alarm = (Hours_alarm + 1) % 12;
//			}
//			Mins_alarm = (Mins_alarm + 10) % 60;
//		} else {
//			Alarm_active = !Alarm_active;
//		}
//	}
//}

//void Buttons_10ms_Handler(void) {
//	Buttons_ReadInput();
//	for(uint32_t i = 0; i < 4; i++) {
//		if(debounce[i] > 0) {
//			debounce[i] -= 1;
//			if (debounce[i] == 0) {
//				Buttons_Pressed(i);
//			}
//		}
//	}
//}
