// UltimateMain.c

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "ST7735.h"
#include "Bool.h"
//#include "Buttons.h"
//#include "Graphics.h"
//#include "Timers.h"
#include "PWM.h"
//#include "Speaker.h"
#include "ImageProcessor.h"
#include "I2C0.h"
#include "esp8266.h"
#include "Timer2.h"
#include "Timer3.h"
//#include "StepperMotor.h"
#include "Unified_Port_Init.h"
#include "CortexM.h"
#include "TExaS.h"
#include <stdio.h>

#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

bool motorOn = false;
int capturePic = false;
int NextSt = 0;

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
    if(pin_num == 0x00)  {
      capturePic = pin_int;
			//use capturePic to determine if a picture must be taken
      //PortF_Output(LED<<2); // Blue LED
#ifdef DEBUG3
      Output_Color(ST7735_CYAN);
      ST7735_OutString("Rcv VP1 data=");
      ST7735_OutUDec(LED);
      ST7735_OutChar('\n');
#endif
    }                               // Parse incoming data
		else if(pin_num == 0x01) { //next motor state output
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
#ifdef DEBUG1
    UART_OutString(" Pin_Number = ");
    UART_OutString(Pin_Number);
    UART_OutString("   Pin_Integer = ");
    UART_OutString(Pin_Integer);
    UART_OutString("   Pin_Float = ");
    UART_OutString(Pin_Float);
    UART_OutString("\n\r");
#endif

  }
}

void SendInformation(void){
  //uint32_t thisF;
  //thisF = PortF_Input();
// your account will be temporarily halted if you send too much data
  if(thisF != LastF){
    TM4C_to_Blynk(74, thisF);  // VP74
#ifdef DEBUG3
    Output_Color(ST7735_WHITE);
    ST7735_OutString("Send 74 data=");
    ST7735_OutUDec(thisF);
    ST7735_OutChar('\n');
#endif
  }
  LastF = thisF;
}


int main(void){
	DisableInterrupts();
	PLL_Init(Bus80MHz);
	Unified_Port_Init();
	ST7735_InitR(INITR_REDTAB);
	//StepperMotor_Init(); ->should be ready to go

	I2C_Init();
	for (int delay = 0; delay <= 100000; delay++){
			__nop();
	}
	CameraSetup();

	ESP8266_Init();
	ESP8266_Reset();
	ESP8266_SetupWiFi();

	//Buttons_Init();
//Everything below here should work,
	/*Timer2_Init(&Blynk_to_TM4C,800000);
  // check for receive data from Blynk App every 10ms

  Timer3_Init(&SendInformation,4000000);

	EnableInterrupts();
	while(1){
		StreamImage();

		if(motorOn) {
			GPIO_PORTD_DATA_R = fsm[currentState].Out; //output to the motor

			SysTick_Wait10ms(FSM[current].delay); //delay for how ever long each state runs for

			input_t = NextSt;		//take the inputs from the Blynk App
			currentState = fsm[currentState].Next[input_t]; //use the new value as the index of the next state graph
		}
	}*/
	PF2 = 1;
	while(1){
		PF2 ^= 1;
	}
}
// End of Main
