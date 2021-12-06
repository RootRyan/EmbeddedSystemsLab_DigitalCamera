// OV7670.c
// Ryan Root, Pranesh Satish, Jacob Dach, Jeppe Lynge
// Initial Creation Date: 10/28/21
// Lab 08
// TA: Hassan Iqbal
// Low level drivers for the OV7670 Camera Module
// Last Modified: 11/28/21

/* Pin Connections
VSYNC/VS - PB1
RRST/RESET - PA1
PCLCK - PIN12
SIOD/SDA - PB3 (I2C data) - 10K resistor to 3.3V
SIOC/SCL - PB2 (I2C clock) - 10K resistor to 3.3V
D0..D3 - PD4..7 (pixel data bits 0..3)
D4..D7 - PB4..7 (pixel data bits 4..7)
*/

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include <stdio.h>
#include "Bool.h"
#include "I2C0.h"
#include "OV7670.h"
#include "OV7670RegisterAddresses.h"

//Slave addresses are 42 for write and 43 for read
//So 0x2A and 0x2B for r/w respectively

//vsync
#define PB1       (*((volatile uint32_t *)0x40005008))

//pixel data bits 0-3
#define PD4       (*((volatile uint32_t *)0x40007040))
#define PD5       (*((volatile uint32_t *)0x40007080))
#define PD6       (*((volatile uint32_t *)0x40007100))
#define PD7       (*((volatile uint32_t *)0x40007200))

//pixel data bits 4-7
#define PB4       (*((volatile uint32_t *)0x40005040))     
#define PB5       (*((volatile uint32_t *)0x40005080))     
#define PB6       (*((volatile uint32_t *)0x40005100))     
#define PB7       (*((volatile uint32_t *)0x40005200))

bool ResetOV7670Registers(void);
bool WriteDefaults(void);
bool SetRGB565(void);
bool SetQQVGA(void);
void SetDisablePixelClockDuringBlankLines(void);
void SetDisableHREFDuringBlankLines(void);
void SetHREFReverse(void);
void SetInternalClockPreScaler(uint8_t);
void SetPLLMultiplier(uint8_t);
void SetManualContrastCenter(uint8_t);
void SetContrast(uint8_t);
void SetBrightness(uint8_t);
void ReversePixelBits(void);
void SetShowColorBar(bool);

uint8_t internalClockPreScaler = 4;
uint8_t pllMultiplier = 5;
bool CameraInit(){
	bool failure = false;
	//Make sure IO is initialized before anything below is called
	//Potential added delay
	for (int delay = 0; delay <= 100000; delay++){
			__nop();
	}
	//failure |= WriteDefaults();
	for (int delay = 0; delay <= 100000; delay++){
			__nop();
	}
	//failure |= SetRGB565();
	for (int delay = 0; delay <= 100000; delay++){
			__nop();
	}
	//failure |= SetQQVGA();
	for (int delay = 0; delay <= 100000; delay++){
			__nop();
	}

	//SetDisablePixelClockDuringBlankLines();
  //SetDisableHREFDuringBlankLines();
  //SetInternalClockPreScaler(internalClockPreScaler);
  //SetPLLMultiplier(pllMultiplier);
	
	return failure;
}

bool ResetOV7670Registers(){
	bool result = I2C_Send1(REG_COM7, REG_COM7 | 0x80);
	for (int delay = 0; delay <= 10000000; delay++){
			__nop();
	}
	return result;
}

bool WriteDefaults(){
	I2C_Send1(REG_COM7, COM7_RESET);
	I2C_Send1(REG_TSLB,  0x04);	/* OV */
	I2C_Send1(REG_COM7, 0);	/* VGA */
	/*
	 * Set the hardware window.  These values from OV don't entirely
	 * make sense - hstop is less than hstart.  But they work...
	 */
	I2C_Send1(REG_HSTART, 0x13);	I2C_Send1(REG_HSTOP, 0x01);
	I2C_Send1(REG_HREF, 0xb6);	I2C_Send1(REG_VSTART, 0x02);
	I2C_Send1(REG_VSTOP, 0x7a);	I2C_Send1(REG_VREF, 0x0a);

	I2C_Send1(REG_COM3, 0);	I2C_Send1(REG_COM14, 0);
	/* Mystery scaling numbers */
	I2C_Send1(0x70, 0x3a);		I2C_Send1(0x71, 0x35);
	I2C_Send1(0x72, 0x11);		I2C_Send1(0x73, 0xf0);
	I2C_Send1(0xa2,/* 0x02 changed to 1*/1);I2C_Send1(REG_COM10, COM10_VS_NEG);
	/* Gamma curve values */
	I2C_Send1(0x7a, 0x20);		I2C_Send1(0x7b, 0x10);
	I2C_Send1(0x7c, 0x1e);		I2C_Send1(0x7d, 0x35);
	I2C_Send1(0x7e, 0x5a);		I2C_Send1(0x7f, 0x69);
	I2C_Send1(0x80, 0x76);		I2C_Send1(0x81, 0x80);
	I2C_Send1(0x82, 0x88);		I2C_Send1(0x83, 0x8f);
	I2C_Send1(0x84, 0x96);		I2C_Send1(0x85, 0xa3);
	I2C_Send1(0x86, 0xaf);		I2C_Send1(0x87, 0xc4);
	I2C_Send1(0x88, 0xd7);		I2C_Send1(0x89, 0xe8);
	/* AGC and AEC parameters.  Note we start by disabling those features,
	   then turn them only after tweaking the values. */
	I2C_Send1(REG_COM8, COM8_FASTAEC | COM8_AECSTEP);
	I2C_Send1(REG_GAIN, 0);	I2C_Send1(REG_AECH, 0);
	I2C_Send1(REG_COM4, 0x40); /* magic reserved bit */
	I2C_Send1(REG_COM9, 0x18); /* 4x gain + magic rsvd bit */
	I2C_Send1(REG_BD50MAX, 0x05);	I2C_Send1(REG_BD60MAX, 0x07);
	I2C_Send1(REG_AEW, 0x95);	I2C_Send1(REG_AEB, 0x33);
	I2C_Send1(REG_VPT, 0xe3);	I2C_Send1(REG_HAECC1, 0x78);
	I2C_Send1(REG_HAECC2, 0x68);	I2C_Send1(0xa1, 0x03); /* magic */
	I2C_Send1(REG_HAECC3, 0xd8);	I2C_Send1(REG_HAECC4, 0xd8);
	I2C_Send1(REG_HAECC5, 0xf0);	I2C_Send1(REG_HAECC6, 0x90);
	I2C_Send1(REG_HAECC7, 0x94);
	I2C_Send1(REG_COM8, COM8_FASTAEC|COM8_AECSTEP|COM8_AGC|COM8_AEC);
	I2C_Send1(0x30,0);I2C_Send1(0x31,0);//disable some delays
	/* Almost all of these are magic "reserved" values.  */
	I2C_Send1(REG_COM5, 0x61);	I2C_Send1(REG_COM6, 0x4b);
	I2C_Send1(0x16, 0x02);		I2C_Send1(REG_MVFP, 0x07);
	I2C_Send1(0x21, 0x02);		I2C_Send1(0x22, 0x91);
	I2C_Send1(0x29, 0x07);		I2C_Send1(0x33, 0x0b);
	I2C_Send1(0x35, 0x0b);		I2C_Send1(0x37, 0x1d);
	I2C_Send1(0x38, 0x71);		I2C_Send1(0x39, 0x2a);
	I2C_Send1(REG_COM12, 0x78);	I2C_Send1(0x4d, 0x40);
	I2C_Send1(0x4e, 0x20);		I2C_Send1(REG_GFIX, 0);
	/*I2C_Send1(0x6b, 0x4a);*/		I2C_Send1(0x74,0x10);
	I2C_Send1(0x8d, 0x4f);		I2C_Send1(0x8e, 0);
	I2C_Send1(0x8f, 0);		I2C_Send1(0x90, 0);
	I2C_Send1(0x91, 0);		I2C_Send1(0x96, 0);
	I2C_Send1(0x9a, 0);		I2C_Send1(0xb0, 0x84);
	I2C_Send1(0xb1, 0x0c);		I2C_Send1(0xb2, 0x0e);
	I2C_Send1(0xb3, 0x82);		I2C_Send1(0xb8, 0x0a);

	/* More reserved magic, some of which tweaks white balance */
	I2C_Send1(0x43, 0x0a);		I2C_Send1(0x44, 0xf0);
	I2C_Send1(0x45, 0x34);		I2C_Send1(0x46, 0x58);
	I2C_Send1(0x47, 0x28);		I2C_Send1(0x48, 0x3a);
	I2C_Send1(0x59, 0x88);		I2C_Send1(0x5a, 0x88);
	I2C_Send1(0x5b, 0x44);		I2C_Send1(0x5c, 0x67);
	I2C_Send1(0x5d, 0x49);		I2C_Send1(0x5e, 0x0e);
	I2C_Send1(0x6c, 0x0a);		I2C_Send1(0x6d, 0x55);
	I2C_Send1(0x6e, 0x11);		I2C_Send1(0x6f, 0x9e); /* it was 0x9F "9e for advance AWB" */
	I2C_Send1(0x6a, 0x40);		I2C_Send1(REG_BLUE, 0x40);
	I2C_Send1(REG_RED, 0x60);
	I2C_Send1(REG_COM8, COM8_FASTAEC|COM8_AECSTEP|COM8_AGC|COM8_AEC|COM8_AWB);

	/* Matrix coefficients */
	I2C_Send1(0x4f, 0x80);		I2C_Send1(0x50, 0x80);
	I2C_Send1(0x51, 0);		I2C_Send1(0x52, 0x22);
	I2C_Send1(0x53, 0x5e);		I2C_Send1(0x54, 0x80);
	I2C_Send1(0x58, 0x9e);

	I2C_Send1(REG_COM16, COM16_AWBGAIN);	I2C_Send1(REG_EDGE, 0);
	I2C_Send1(0x75, 0x05);		I2C_Send1(REG_REG76, 0xe1);
	I2C_Send1(0x4c, 0);		I2C_Send1(0x77, 0x01);
	I2C_Send1(REG_COM13, /*0xc3*/0x48);	I2C_Send1(0x4b, 0x09);
	I2C_Send1(0xc9, 0x60);		/*I2C_Send1(REG_COM16, 0x38);*/
	I2C_Send1(0x56, 0x40);

	I2C_Send1(0x34, 0x11);		I2C_Send1(REG_COM11, COM11_EXP|COM11_HZAUTO);
	I2C_Send1(0xa4, 0x82/*Was 0x88*/);		I2C_Send1(0x96, 0);
	I2C_Send1(0x97, 0x30);		I2C_Send1(0x98, 0x20);
	I2C_Send1(0x99, 0x30);		I2C_Send1(0x9a, 0x84);
	I2C_Send1(0x9b, 0x29);		I2C_Send1(0x9c, 0x03);
	I2C_Send1(0x9d, 0x4c);		I2C_Send1(0x9e, 0x3f);
	I2C_Send1(0x78, 0x04);

	/* Extra-weird stuff.  Some sort of multiplexor register */
	I2C_Send1(0x79, 0x01);		I2C_Send1(0xc8, 0xf0);
	I2C_Send1(0x79, 0x0f);		I2C_Send1(0xc8, 0x00);
	I2C_Send1(0x79, 0x10);		I2C_Send1(0xc8, 0x7e);
	I2C_Send1(0x79, 0x0a);		I2C_Send1(0xc8, 0x80);
	I2C_Send1(0x79, 0x0b);		I2C_Send1(0xc8, 0x01);
	I2C_Send1(0x79, 0x0c);		I2C_Send1(0xc8, 0x0f);
	I2C_Send1(0x79, 0x0d);		I2C_Send1(0xc8, 0x20);
	I2C_Send1(0x79, 0x09);		I2C_Send1(0xc8, 0x80);
	I2C_Send1(0x79, 0x02);		I2C_Send1(0xc8, 0xc0);
	I2C_Send1(0x79, 0x03);		I2C_Send1(0xc8, 0x40);
	I2C_Send1(0x79, 0x05);		I2C_Send1(0xc8, 0x30);
	I2C_Send1(0x79, 0x26);

	I2C_Send1(0xff, 0xff);	/* END MARKER */
	return false;
}

bool SetRGB565(){
	bool failed = false;
	failed |= I2C_Send1(REG_COM7, COM7_RGB); /* Selects RGB mode */
  failed |= I2C_Send1(REG_RGB444, 0);	  /* No RGB444 please */
  failed |= I2C_Send1(REG_COM1, 0x0);
  failed |= I2C_Send1(REG_COM15, COM15_RGB565|COM15_R00FF);
  failed |= I2C_Send1(REG_COM9, 0x6A);	 /* 128x gain ceiling; 0x8 is reserved bit */
  failed |= I2C_Send1(0x4f, 0xb3);		 /* "matrix coefficient 1" */
  failed |= I2C_Send1(0x50, 0xb3);		 /* "matrix coefficient 2" */
  failed |= I2C_Send1(0x51, 0);		 /* vb */
  failed |= I2C_Send1(0x52, 0x3d);		 /* "matrix coefficient 4" */
  failed |= I2C_Send1(0x53, 0xa7);		 /* "matrix coefficient 5" */
  failed |= I2C_Send1(0x54, 0xe4);		 /* "matrix coefficient 6" */
  failed |= I2C_Send1(REG_COM13, /*COM13_GAMMA|*/COM13_UVSAT);
  failed |= I2C_Send1(0xff, 0xff);
	
	return failed;
}

//QQVGA - 160x120 <--> ST7735 - 160x128
uint8_t QQVGA_VERTICAL_PADDING = 2;
bool SetQQVGA(){
	bool failed = false;
	
	uint16_t vstart = 0;
	uint16_t vstop = 120 + QQVGA_VERTICAL_PADDING;

	// 120 + 2 pixel (4 bytes) for padding.
	// One from the beginning and three at the end.
	uint16_t hstart = 182;
	uint16_t hstop = 46;
	
	failed |= I2C_Send1(REG_VSTART,vstart);
  failed |= I2C_Send1(REG_VSTOP,vstop);
  failed |= I2C_Send1(REG_VREF,0);
  failed |= I2C_Send1(REG_HSTART,hstart >> 3);
  failed |= I2C_Send1(REG_HSTOP,hstop >> 3);
  failed |= I2C_Send1(REG_HREF,0x00 | (hstart & 0x7) | ((hstop & 0x7) << 3));

  failed |= I2C_Send1(REG_COM3, COM3_DCWEN); // enable downsamp/crop/window
  failed |= I2C_Send1(REG_COM14, 0x1a);        // divide by 4
  failed |= I2C_Send1(SCALING_DCWCTR, 0x22);   // downsample by 4
  failed |= I2C_Send1(SCALING_PCLK_DIV, 0xf2); // divide by 4

  failed |= I2C_Send1(0xff, 0xff);	/* END MARKER */
	return failed;
}
void SetDisablePixelClockDuringBlankLines() {
	uint8_t regContents = I2C_Recv(REG_COM10);
  I2C_Send1(REG_COM10, regContents | COM10_PCLK_HB);
}

void SetDisableHREFDuringBlankLines() {
	uint8_t regContents = I2C_Recv(REG_COM6);
  I2C_Send1(REG_COM6, regContents | COM6_HREF_HB);
}

void SetHREFReverse() {
	uint8_t regContents = I2C_Recv(REG_COM10);
  I2C_Send1(REG_COM10, regContents | COM10_HREF_REV);
}

void SetInternalClockPreScaler(uint8_t preScaler) {
  I2C_Send1(REG_CLKRC, 0x80 | preScaler); // f = input / (val + 1)
}

void SetPLLMultiplier(uint8_t multiplier) {
  uint8_t mask = 0xC0;
  uint8_t currentValue = I2C_Recv(DBLV);
  I2C_Send1(DBLV, (currentValue & ~mask) | (multiplier << 6));
}

void SetManualContrastCenter(uint8_t contrastCenter) {
	uint8_t regContents = I2C_Recv(MTXS);
  I2C_Send1(MTXS, regContents & 0x7F); // disable auto contrast
  I2C_Send1(REG_CONTRAST_CENTER, contrastCenter);
}

void SetContrast(uint8_t contrast) {
  // default 0x40
  I2C_Send1(REG_CONTRAS, contrast);
}

void SetBrightness(uint8_t birghtness) {
  I2C_Send1(REG_BRIGHT, birghtness);
}


void ReversePixelBits() {
	uint8_t regContents = I2C_Recv(REG_COM3);
  I2C_Send1(REG_COM3, regContents | COM3_SWAP);
}


void SetShowColorBar(bool transparent) {
  if (transparent) {
		uint8_t regContents = I2C_Recv(REG_COM7);
    I2C_Send1(REG_COM7, regContents | COM7_COLOR_BAR);
  } else {
		uint8_t regContents = I2C_Recv(REG_COM17);
    I2C_Send1(REG_COM17, regContents | COM17_CBAR);
  }
}

//end
