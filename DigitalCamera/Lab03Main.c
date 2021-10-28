// Lab03Main.c
// Ryan Root, Pranesh Satish, Jacob Dach
// Initial Creation Date: 9/15/21
// Lab 03
// TA: Hassan Iqbal
// Description: Main program for alarm clock
// Last Modified: 9/24/21

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

void clockGraphicsInit(void);
void updateClockGraphics(void);
void clockGraphicsDay(void);
void clockGraphicsNight(void);

// measures analog voltage on PD3
void ScopeTask(void){  // called 10k/sec
  UART0_DR_R = (ADC1_SSFIFO3_R>>4); // send ADC to TExaSdisplay
}

/***** GLOBAL VARIABLES *****/
uint32_t Hours_displayed = 0;	
uint32_t Hours_current = 0;		
uint32_t Mins_current = 0;						
uint32_t Secs_current = 0;					
uint32_t Hours_old;									
uint32_t Mins_old;				
uint32_t Secs_old;					
uint32_t Hours_alarm = 0;		
uint32_t Mins_alarm = 0;
uint32_t Hours_stop = 0;		
uint32_t Mins_stop = 0;
uint32_t Secs_stop = 0;	
bool Speaker_active = false;
uint32_t next_state = 0;
bool dayNight = false;

int main(void){
	DisableInterrupts();										
	PLL_Init(Bus80MHz);   
	Unified_Port_Init();            
	ST7735_InitR(INITR_BLACKTAB);
	Timer0A_Init(80000000);	 // initialize timer0A for one second interupts
	PWM0A_Init(40000, 10000);   // initialize PWM0, 500 Hz, 25% duty
	SpeakerEnable(false);
	Timer1_Init();
	PF1 = 0;            
  PF2 = 0;
	TExaS_SetTask(&ScopeTask);
	EnableInterrupts();
	
	ST7735_FillScreen(ST7735_BLACK);
	ST7735_OutString("Lab3");
	dayNight = false;
	updateClockGraphics();
	while(1){
		PF2 ^= 0x04;
		updateClockGraphics();
		if (Buttons_SetAlarmMode() && (Hours_displayed == Hours_alarm) && (Mins_current%60 == Mins_alarm%60)){		
			Speaker_active = true;
			SpeakerEnable(true);																																				
		}
		
		PF2 ^= 0x04;
	}		
}
// End of Main

void updateClockGraphics(void) {
	if(((Hours_current % 24) < 6) || ((Hours_current % 24) >= 18)) {
		if (dayNight){
			ST7735_FillScreen(ST7735_BLACK);
			dayNight = false;
		}
		clockGraphicsNight();
	} else {
		if (!dayNight){
			ST7735_FillScreen(ST7735_WHITE);
			dayNight = true;
		}
		clockGraphicsDay();
	}		
	ST7735_Line(CLOCK_CENTER,Min_hand[Secs_current].x,Min_hand[Secs_current].y,ST7735_RED);	
	ST7735_Line(CLOCK_CENTER,Min_hand[Mins_current].x,Min_hand[Mins_current].y,ST7735_BLUE);	
	ST7735_Line(CLOCK_CENTER,Hour_hand[Hours_current].x,Hour_hand[Hours_current].y,ST7735_BLUE);	
	
	ST7735_SetCursor(0,12);
	ST7735_OutString("Time:   :  :    M");
	
	if (Hours_current % 12 == 0) {
		Hours_displayed = 12;
	} else {
		Hours_displayed = Hours_current % 12;
	}
	
	ST7735_SetCursor(6,12);
	if (Hours_displayed < 10) {
		ST7735_OutChar('0');
		ST7735_SetCursor(7,12);
		ST7735_OutUDec(Hours_displayed);
	} else {
		ST7735_OutUDec(Hours_displayed);
	}
	ST7735_SetCursor(9,12);
	if (Mins_current < 10) {
		ST7735_OutChar('0');
		ST7735_SetCursor(10,12);
		ST7735_OutUDec(Mins_current);
	} else {
		ST7735_OutUDec(Mins_current);
	}
	ST7735_SetCursor(12,12);
	if (Secs_current < 10) {
		ST7735_OutChar('0');
		ST7735_SetCursor(13,12);
		ST7735_OutUDec(Secs_current);
	} else {
		ST7735_OutUDec(Secs_current);
	}
	ST7735_SetCursor(15,12);
	if (Hours_current % 24 < 12) {
		ST7735_OutChar('A');
	} else {
		ST7735_OutChar('P');
	}
	
	ST7735_SetCursor(0,13);
	if (Buttons_SetAlarmMode()){
		ST7735_OutString("Alarm is Active  ");
	} else {
		ST7735_OutString("Alarm is Inactive");
	}
	
	ST7735_SetCursor(0,14);
	ST7735_OutString("Alarm:   :    ");
	
	ST7735_SetCursor(7,14);
	if (Hours_alarm < 10) {
		ST7735_OutChar('0');
		ST7735_SetCursor(8,14);
		ST7735_OutUDec(Hours_alarm);
	} else {
		ST7735_OutUDec(Hours_alarm);
	}
	ST7735_SetCursor(10,14);
	if (Mins_alarm < 10) {
		ST7735_OutChar('0');
		ST7735_SetCursor(11,14);
		ST7735_OutUDec(Mins_alarm);
	} else {
		ST7735_OutUDec(Mins_alarm);
	}
	
	
	ST7735_SetCursor(0,15);
	ST7735_OutString("Stopwatch:   :  :  ");
	ST7735_SetCursor(11,15);
	if (Hours_stop < 10) {
		ST7735_OutChar('0');
		ST7735_SetCursor(12,15);
		ST7735_OutUDec(Hours_stop);
	} else {
		ST7735_OutUDec(Hours_stop);
	}
	ST7735_SetCursor(14,15);
	if (Mins_stop < 10) {
		ST7735_OutChar('0');
		ST7735_SetCursor(15,15);
		ST7735_OutUDec(Mins_stop);
	} else {
		ST7735_OutUDec(Mins_stop);
	}
	ST7735_SetCursor(17,15);
	if (Secs_stop < 10) {
		ST7735_OutChar('0');
		ST7735_SetCursor(18,15);
		ST7735_OutUDec(Secs_stop);
	} else {
		ST7735_OutUDec(Secs_stop);
	}
	
}

void clockGraphicsNight(void) {
	ST7735_SetCursor(0,0);
	ST7735_OutString("Lab3");
	Graphics_DrawCircle(CLOCK_CENTER,CLOCK_RADIUS+2,ST7735_WHITE);
	
	ST7735_DrawChar(60, 1, 0x31, ST7735_WHITE, ST7735_BLACK, 1); 	
	ST7735_DrawChar(65, 1, 0x32, ST7735_WHITE, ST7735_BLACK, 1); 																	
	ST7735_DrawChar(87, 7, 0x31, ST7735_WHITE, ST7735_BLACK, 1); 																	
	ST7735_DrawChar(103, 24, 0x32, ST7735_WHITE, ST7735_BLACK, 1);																
	ST7735_DrawChar(109, 48, 0x33, ST7735_WHITE, ST7735_BLACK, 1);																
	ST7735_DrawChar(103, 70, 0x34, ST7735_WHITE, ST7735_BLACK, 1);																
	ST7735_DrawChar(86, 89, 0x35, ST7735_WHITE, ST7735_BLACK, 1);																	
	ST7735_DrawChar(62, 95, 0x36, ST7735_WHITE, ST7735_BLACK, 1);																	
	ST7735_DrawChar(39, 89, 0x37, ST7735_WHITE, ST7735_BLACK, 1);																	
	ST7735_DrawChar(21, 71, 0x38, ST7735_WHITE, ST7735_BLACK, 1);																	
	ST7735_DrawChar(14, 47, 0x39, ST7735_WHITE, ST7735_BLACK, 1);																
	ST7735_DrawChar(16, 23, 0x31, ST7735_WHITE, ST7735_BLACK, 1);																	
	ST7735_DrawChar(21, 23, 0x30, ST7735_WHITE, ST7735_BLACK, 1);																
	ST7735_DrawChar(34, 5, 0x31, ST7735_WHITE, ST7735_BLACK, 1);																
	ST7735_DrawChar(39, 5, 0x31, ST7735_WHITE, ST7735_BLACK, 1);
	
	ST7735_Line(CLOCK_CENTER,Hour_hand[Hours_old].x,Hour_hand[Hours_old].y,ST7735_BLACK);
	ST7735_Line(CLOCK_CENTER,Min_hand[Mins_old].x,Min_hand[Mins_old].y,ST7735_BLACK);
	ST7735_Line(CLOCK_CENTER,Min_hand[Secs_old].x,Min_hand[Secs_old].y,ST7735_BLACK);	
}

void clockGraphicsDay(void) {
	ST7735_SetCursor(0,0);
	ST7735_OutString("Lab3");
	Graphics_DrawCircle(CLOCK_CENTER,CLOCK_RADIUS+2,ST7735_BLACK);
	
	ST7735_DrawChar(60, 1, 0x31, ST7735_BLACK, ST7735_WHITE, 1); 	
	ST7735_DrawChar(65, 1, 0x32, ST7735_BLACK, ST7735_WHITE, 1); 																	
	ST7735_DrawChar(87, 7, 0x31, ST7735_BLACK, ST7735_WHITE, 1); 																	
	ST7735_DrawChar(103, 24, 0x32, ST7735_BLACK, ST7735_WHITE, 1);																
	ST7735_DrawChar(109, 48, 0x33, ST7735_BLACK, ST7735_WHITE, 1);																
	ST7735_DrawChar(103, 70, 0x34, ST7735_BLACK, ST7735_WHITE, 1);																
	ST7735_DrawChar(86, 89, 0x35, ST7735_BLACK, ST7735_WHITE, 1);																	
	ST7735_DrawChar(62, 95, 0x36, ST7735_BLACK, ST7735_WHITE, 1);																	
	ST7735_DrawChar(39, 89, 0x37, ST7735_BLACK, ST7735_WHITE, 1);																	
	ST7735_DrawChar(21, 71, 0x38, ST7735_BLACK, ST7735_WHITE, 1);																	
	ST7735_DrawChar(14, 47, 0x39, ST7735_BLACK, ST7735_WHITE, 1);																
	ST7735_DrawChar(16, 23, 0x31, ST7735_BLACK, ST7735_WHITE, 1);																	
	ST7735_DrawChar(21, 23, 0x30, ST7735_BLACK, ST7735_WHITE, 1);																
	ST7735_DrawChar(34, 5, 0x31, ST7735_BLACK, ST7735_WHITE, 1);																
	ST7735_DrawChar(39, 5, 0x31, ST7735_BLACK, ST7735_WHITE, 1);
	
	ST7735_Line(CLOCK_CENTER,Hour_hand[Hours_old].x,Hour_hand[Hours_old].y,ST7735_WHITE);
	ST7735_Line(CLOCK_CENTER,Min_hand[Mins_old].x,Min_hand[Mins_old].y,ST7735_WHITE);
	ST7735_Line(CLOCK_CENTER,Min_hand[Secs_old].x,Min_hand[Secs_old].y,ST7735_WHITE);
}
