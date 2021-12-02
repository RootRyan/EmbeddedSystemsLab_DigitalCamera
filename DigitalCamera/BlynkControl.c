// BlynkControl.c
// Ryan Root, Pranesh Satish, Jacob Dach, Jeppe Lynge
// Lab 11
// TA: Hassan Iqbal

/* -------------------------------------------------------------------
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
//------------------------------------------------------------------------------*/

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
#include "tm4c123gh6pm.h"
#include "Bool.h"
#include "esp8266.h"
#include "Timer2.h"
#include "Timer3.h"
#include "UART.h"
#include "ImageProcessor.h"


bool pinPrevActive[16] = {false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false};
bool rotationLockEnabled = false;
extern bool motorOn;
extern int NextSt;

bool countdownEnabled = false;
uint32_t countdownOptions[4] = {1,3,5,10};
uint32_t countdownSelected = 0;
bool countdownIsActive = false; //Should be changeable by picture taking function
extern uint32_t countdownTime;
uint32_t lastCountdownTime = 0;

extern int capturePic;
bool flashEnabled = false;

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
    pin_int = atoi(Pin_Integer);    // Parse incoming data        
#ifdef DEBUG1
    UART_OutString(" Pin_Number = ");
    UART_OutString(Pin_Number);
    UART_OutString("   Pin_Integer = ");
    UART_OutString(Pin_Integer);
    UART_OutString("   Pin_Float = ");
    UART_OutString(Pin_Float);
    UART_OutString("\n\r");
#endif

// ----------------------------- Virtual Pin Definitions ------------------------------------
// Stepper Motor
// - Rotate from virtual pin button(s) press --> either a switch controlling direction and a button to rotate or two buttons for rotating cw or ccw
// - Output stepper motor stage if that's a thing
//
// Camera
// - Turn flash on/off based on virtual pin switch or button press
// - Take picture (or timed picture) on virtual pin button press	
// - Activate/Deactivate picture countdown timer on virtual pin switch	
// - Set picture countdown timer on virtual pin button press (rotate through preset times -- 1, 3, 5, 10 seconds)
//  \_,-> I think this is simpler than a slider since it avoids interacting with floats and limits timer to reasonable and controlled values
// - Output active picture countdown timer time (for viewing potential countdowns and it decrementing too)
//

		if(pin_num == 0x00)  {
      capturePic = pin_int;
			//use capturePic to determine if a picture must be taken
      //PortF_Output(LED<<2); // Blue LED
		} else if(pin_num == 0x01) { //next motor state output
			NextSt = pin_int;
			if(NextSt < 0) {
				NextSt = 2;
			}
		} else if(pin_num == 0x02) { //motorOn button
			if(pin_int == 1) {
				motorOn = true;
			} else {
				motorOn = false;
			}
		}	
		
		switch(pin_num){
			case 0x00: //SWITCH - stepper motor lock rotation
				rotationLockEnabled = pin_int % 2;
			case 0x01: //BUTTON - stepper motor rotate CW
				if (rotationLockEnabled) {
					pinPrevActive[1] = true;
				} else if (pin_int == 1 && pinPrevActive[1] == false) {
					//apply rotation
					pinPrevActive[1] = true;
				} else if (pin_int == 0) {
					pinPrevActive[1] = false;
				}
			case 0x02: //BUTTON - stepper motor rotate CCW
				if (rotationLockEnabled) {
					pinPrevActive[1] = true;
				} else if (pin_int == 1 && pinPrevActive[1] == false) {
					//apply rotation
					pinPrevActive[1] = true;
				} else if (pin_int == 0) {
					pinPrevActive[1] = false;
				}
			case 0x03: //SWITCH - enable/disable countdown timer
				countdownEnabled = pin_int % 2; 
				if(countdownEnabled == false){
					countdownIsActive = false;
				}
			case 0x04: //BUTTON - rotate through 4 possible countdown times
				if(countdownEnabled){
					if(countdownIsActive){
						pinPrevActive[4] = true;
					} else if (pin_int == 1 && pinPrevActive[4] == false) {
						pinPrevActive[4] = true;
						countdownSelected = (countdownSelected + 1) % 4;
					} else if (pin_int == 0) {
						pinPrevActive[4] = false;
					}
				} else {
					pinPrevActive[4] = true;
				}				
			case 0x05: //BUTTON - take picture
				if(countdownIsActive) {
					pinPrevActive[0] = true;
				} else if (pin_int == 1 && pinPrevActive[5] == false) {
					pinPrevActive[0] = true;
					if (countdownEnabled) { //countdown
						countdownIsActive = true;
						timedPicture(countdownSelected,flashEnabled);
						//take picture w count and flash setting
					} else { //no countdown
						//take picture w flash setting
						CaptureImage(flashEnabled);
					}
				} else if (pin_int == 0) {
					pinPrevActive[5] = false;
				}
			case 0x06: //SWITCH - enable/disable flash photography
				flashEnabled = (pin_int % 2);
			case 0x07:
			case 0x08:
			case 0x09:
			case 0x0A:
			case 0x0B:
			case 0x0C:
			case 0x0D:
			case 0x0E:
			case 0x0F:
			default: ;//some error happened if it reaches this since blynk to tm4c only uses virtual pins 0-15
		}
	}
}

void SendInformation(void){
	// VP74: Countdown Time
	if(countdownIsActive && (lastCountdownTime != countdownTime)){
		TM4C_to_Blynk(74,countdownTime);
		lastCountdownTime = countdownTime;
	} else if(countdownEnabled && (lastCountdownTime != countdownOptions[countdownSelected])){
		TM4C_to_Blynk(74,countdownOptions[countdownSelected]);
		lastCountdownTime = countdownOptions[countdownSelected];
	} else if (lastCountdownTime != 0){
		TM4C_to_Blynk(74,0);
		lastCountdownTime = 0;
	}
	
	//Any thoughts on what other info to send? maybe available sdc storage/number of pictures in storage
}

void blynkInit(void) {
  ESP8266_Init();       // Enable ESP8266 Serial Port
  ESP8266_Reset();      // Reset the WiFi module
  ESP8266_SetupWiFi();  // Setup communications to Blynk Server  
  
  Timer2_Init(&Blynk_to_TM4C,800000); 
  // check for receive data from Blynk App every 10ms

  Timer3_Init(&SendInformation,40000000); 
  // Send data back to Blynk App every 1/2 second
}
