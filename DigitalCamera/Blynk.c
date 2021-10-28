// -------------------------------------------------------------------
// File name: Blynk.c
// Description: This code is used to bridge the TM4C123 board and the Blynk Application
//              via the ESP8266 WiFi board
// Author: Mark McDermott and Andrew Lynch (Arduino source)
// Converted to EE445L style Jonathan Valvano
// Orig gen date: May 21, 2018
// Last update: Sept 20, 2018
//
// Download latest Blynk library here:
//   https://github.com/blynkkk/blynk-library/releases/latest
//
//  Blynk is a platform with iOS and Android apps to control
//  Arduino, Raspberry Pi and the likes over the Internet.
//  You can easily build graphic interfaces for all your
//  projects by simply dragging and dropping widgets.
//
//   Downloads, docs, tutorials: http://www.blynk.cc
//   Sketch generator:           http://examples.blynk.cc
//   Blynk community:            http://community.blynk.cc
//
//------------------------------------------------------------------------------

// TM4C123       ESP8266-ESP01 (2 by 4 header)
// PE5 (U5TX) to Pin 1 (Rx)
// PE4 (U5RX) to Pin 5 (TX)
// PE3 output debugging
// PE2 nc
// PE1 output    Pin 7 Reset
// PE0 input     Pin 3 Rdy IO2
//               Pin 2 IO0, 10k pullup to 3.3V  
//               Pin 8 Vcc, 3.3V (separate supply from LaunchPad 
// Gnd           Pin 4 Gnd  
// Place a 4.7uF tantalum and 0.1 ceramic next to ESP8266 3.3V power pin
// Use LM2937-3.3 and two 4.7 uF capacitors to convert USB +5V to 3.3V for the ESP8266
// http://www.ti.com/lit/ds/symlink/lm2937-3.3.pdf
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "PLL.h"
#include "Timer2.h"
#include "Timer3.h"
#include "UART.h"
#include "PortF.h"
#include "esp8266.h"
#include "Bool.h"
//#include "Buttons.h"
#include "Graphics.h"
#include "Timers.h"
#include "PWM.h"
#include "Speaker.h"
#include "Unified_Port_Init.h"
#include <stdio.h>

#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))
void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

void clockGraphicsInit(void);
void updateClockGraphics(void);
void clockGraphicsDay(void);
void clockGraphicsNight(void);

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
uint32_t Hours_alarm_old = 0;
uint32_t Mins_alarm_old = 0;
uint32_t StopHOld = 0;
uint32_t StopMOld = 0;

// 0-23 hours
uint32_t hours = 0;
// 0-59 minutes
uint32_t minutes = 0;
bool start_stopwatch = false;
bool Alarm_active = false;
bool PrevActive[9] = {false,false,false,false,false,false,false,false,false};

uint32_t LED;      // VP1
uint32_t LastF;    // VP74
// These 6 variables contain the most recent Blynk to TM4C123 message
// Blynk to TM4C123 uses VP0 to VP15
char serial_buf[64];
char Pin_Number[2]   = "99";       // Initialize to invalid pin number
char Pin_Integer[8]  = "0000";     //
char Pin_Float[8]    = "0.0000";   //
uint32_t pin_num; 
uint32_t pin_int;
 
// ----------------------------------- TM4C_to_Blynk ------------------------------
// Send data to the Blynk App
// It uses Virtual Pin numbers between 70 and 99
// so that the ESP8266 knows to forward the data to the Blynk App
void TM4C_to_Blynk(uint32_t pin,uint32_t value){
  if((pin < 70)||(pin > 99)){
    return; // ignore illegal requests
  }
// your account will be temporarily halted 
  // if you send too much data
  ESP8266_OutUDec(pin);       // Send the Virtual Pin #
  ESP8266_OutChar(',');
  ESP8266_OutUDec(value);      // Send the current value
  ESP8266_OutChar(',');
  ESP8266_OutString("0.0\n");  // Null value not used in this example
}
 
 
// -------------------------   Blynk_to_TM4C  -----------------------------------
// This routine receives the Blynk Virtual Pin data via the ESP8266 and parses the
// data and feeds the commands to the TM4C.
void Blynk_to_TM4C(void){int j; char data;
// Check to see if a there is data in the RXD buffer
  if(ESP8266_GetMessage(serial_buf)){  // returns false if no message
    // Read the data from the UART5
#ifdef DEBUG1
    j = 0;
    do{
      data = serial_buf[j];
      UART_OutChar(data);        // Debug only
      j++;
    }while(data != '\n');
    UART_OutChar('\r');        
#endif
           
// Rip the 3 fields out of the CSV data. The sequence of data from the 8266 is:
// Pin #, Integer Value, Float Value.
    strcpy(Pin_Number, strtok(serial_buf, ","));
    strcpy(Pin_Integer, strtok(NULL, ","));       // Integer value that is determined by the Blynk App
    strcpy(Pin_Float, strtok(NULL, ","));         // Not used
    pin_num = atoi(Pin_Number);     // Need to convert ASCII to integer
    pin_int = atoi(Pin_Integer);  
  // ---------------------------- VP #1 ----------------------------------------
  // This VP is the LED select button
    if(pin_num == 0x01)  {  
      LED = pin_int;
      PortF_Output(LED<<2); // Blue LED
#ifdef DEBUG3
      Output_Color(ST7735_CYAN);
      ST7735_OutString("Rcv VP1 data=");
      ST7735_OutUDec(LED);
      ST7735_OutChar('\n');
#endif
    }                               // Parse incoming data        
#ifdef DEBUG1
    UART_OutString(" Pin_Number = ");
    UART_OutString(Pin_Number);
    UART_OutString("   Pin_Integer = ");
    UART_OutString(Pin_Integer);
    UART_OutString("   Pin_Float = ");
    UART_OutString(Pin_Float);
    UART_OutString("\n\r");
#endif
		if(pin_num == 0x00 && PrevActive[0] == false && pin_int == 1){//clockhours
			Hours_old = Hours_current;
			Hours_current = (Hours_current + 1) % 12;
			PrevActive[0] = true;
		} else {
			PrevActive[0] = false;
		}
		if(pin_num == 0x01 && PrevActive[1] == false && pin_int == 1){//clockmins
			if(Mins_current+1 >= 60) {
				Hours_old = Hours_current;
				Hours_current = (Hours_current + 1) % 12;
			}
			Mins_old = Mins_current;
			Mins_current = (Mins_current + 1) % 60;
			PrevActive[1] = true;
		} else {
			PrevActive[1] = false;
		}
		if(pin_num == 0x02 && PrevActive[2] == false && pin_int == 1){//alarmhours
			Hours_alarm = (Hours_alarm + 1) % 12;
			if (Hours_alarm == 0) {
					Hours_alarm = 12;
			}
			PrevActive[2] = true;
		} else {
			PrevActive[2] = false;
		}
		if(pin_num == 0x03 && PrevActive[3] == false && pin_int == 1){//alarmmins
			if(Mins_alarm+1 >= 60) {
				Hours_alarm = (Hours_alarm + 1) % 12;
				if (Hours_alarm == 0) {
					Hours_alarm = 12;
				}
			}
			Mins_alarm = (Mins_alarm + 1) % 60;
			PrevActive[3] = true;
		} else {
			PrevActive[3] = false;
		}
		if(pin_num == 0x04 && PrevActive[4] == false && pin_int == 1){//armalarm
			Alarm_active = !Alarm_active;
			SpeakerEnable(false);
			Speaker_active = false;
			PrevActive[4] = true;
		} else {
			PrevActive[4] = false;
		}
		if(pin_num == 0x05 && PrevActive[5] == false && pin_int == 1){//snoozealarm
			if(Speaker_active == true){
				SpeakerEnable(false);
				Speaker_active = false;
				if(Mins_alarm+10 >= 60) {
					Hours_alarm = (Hours_alarm + 1) % 12;
				}
				Mins_alarm = (Mins_alarm + 10) % 60;
			}
			PrevActive[5] = true;
		} else {
			PrevActive[5] = false;
		}
		if(pin_num == 0x06 && PrevActive[6] == false && pin_int == 1){//startwatch
			start_stopwatch = true;
			PrevActive[6] = true;
		} else {
			PrevActive[6] = false;
		}
		if(pin_num == 0x07 && PrevActive[7] == false && pin_int == 1){//stopwatch
			start_stopwatch = false;
			PrevActive[7] = true;
		} else {
			PrevActive[7] = false;
		}
		if(pin_num == 0x08 && PrevActive[8] == false && pin_int == 1){//resetwatch
			Hours_stop = 0;
			Mins_stop = 0;
			Secs_stop = 0;
			PrevActive[8] = true;
		} else {
			PrevActive[8] = false;
		}
  }  
}

void SendInformation(void){
  uint32_t thisF;
  thisF = PortF_Input();
// your account will be temporarily halted if you send too much data
  /*if(thisF != LastF){
    TM4C_to_Blynk(74, thisF);  // VP74
#ifdef DEBUG3
    Output_Color(ST7735_WHITE);
    ST7735_OutString("Send 74 data=");
    ST7735_OutUDec(thisF);
    ST7735_OutChar('\n');
#endif
  }
  LastF = thisF;*/
	if(Hours_current != Hours_old){
		TM4C_to_Blynk(74,Hours_displayed);
	}
	if(Mins_current != Mins_old){
		TM4C_to_Blynk(75,Mins_current);
	}
	if(Secs_current != Secs_old){
		TM4C_to_Blynk(76,Secs_current);
		if (start_stopwatch == true) {
			TM4C_to_Blynk(81,Secs_stop);
		}
	}
	if(Hours_alarm != Hours_alarm_old){
		TM4C_to_Blynk(74,Hours_alarm);
	}
	if(Mins_current != Mins_alarm_old){
		TM4C_to_Blynk(75,Mins_alarm);
	}
	Hours_alarm_old = Hours_alarm;
	Mins_alarm_old = Mins_alarm;
	if(Hours_stop != StopHOld){
		TM4C_to_Blynk(79,Hours_stop);
	}
	if(Mins_stop != StopMOld){
		TM4C_to_Blynk(80,Mins_stop);
	}
	StopHOld = Hours_stop;
	StopMOld = Mins_stop;
	updateClockGraphics();
	if ((Alarm_active == true) && (Hours_displayed == Hours_alarm) && (Mins_current%60 == Mins_alarm%60)){		
		Speaker_active = true;
		SpeakerEnable(true);																																				
	}
}

  
int main(void){       
  PLL_Init(Bus80MHz);   // Bus clock at 80 MHz
  DisableInterrupts();  // Disable interrupts until finished with inits
	Unified_Port_Init();
  //PortF_Init();
  LastF = PortF_Input();
#ifdef DEBUG3
  Output_Init();        // initialize ST7735
  ST7735_OutString("EE445L Lab 4D\nBlynk example\n");
#endif
#ifdef DEBUG1
  UART_Init(5);         // Enable Debug Serial Port
  UART_OutString("\n\rEE445L Lab 4D\n\rBlynk example");
#endif
  ESP8266_Init();       // Enable ESP8266 Serial Port
  ESP8266_Reset();      // Reset the WiFi module
  ESP8266_SetupWiFi();  // Setup communications to Blynk Server  
  
  Timer2_Init(&Blynk_to_TM4C,800000); 
  // check for receive data from Blynk App every 10ms

  Timer3_Init(&SendInformation,40000000); 
  // Send data back to Blynk App every 1/2 second
	
	ST7735_InitR(INITR_BLACKTAB);
	Timer0A_Init(80000000);	 // initialize timer0A for one second interupts
	PWM0A_Init(40000, 10000);   // initialize PWM0, 500 Hz, 25% duty
	SpeakerEnable(false);
	PF1 = 0;            
  PF2 = 0;
  EnableInterrupts();
	ST7735_FillScreen(ST7735_BLACK);
	ST7735_OutString("Lab3");
	dayNight = false;
	updateClockGraphics();
  while(1) {   
    WaitForInterrupt(); // low power mode
  }
}

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
	if (Alarm_active == true){
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
	ST7735_OutString("Lab4");
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
	ST7735_OutString("Lab4");
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

