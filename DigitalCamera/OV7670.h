// OV7670.h
// Ryan Root, Pranesh Satish, Jacob Dach, Jeppe Lynge
// Initial Creation Date: 10/28/21
// Lab 08
// TA: Hassan Iqbal
// Low level drivers for the OV7670 Camera Module
// Last Modified: 10/28/21
#ifndef _OV7670_h_
#define _OV7670_h_

#include <stdint.h>
#include "Bool.h"

typedef enum {
        PIXEL_RGB565,
        PIXEL_BAYERRGB,
        PIXEL_YUV422
} PixelFormat;

typedef enum {
        RESOLUTION_VGA_640x480 = 640,
        RESOLUTION_QVGA_320x240 = 320,
        RESOLUTION_QQVGA_160x120 = 160
} Resolution;

typedef enum {
        PLL_MULTIPLIER_BYPASS = 0,
        PLL_MULTIPLIER_X4 = 1,
        PLL_MULTIPLIER_X6 = 2,
        PLL_MULTIPLIER_X8 = 3
} PLLMultiplier;

bool init();
bool setRegister(uint8_t addr, uint8_t val);
uint8_t readRegister(uint8_t addr);
void setRegisterBitsOR(uint8_t addr, uint8_t bits);
void setRegisterBitsAND(uint8_t addr, uint8_t bits);
void setManualContrastCenter(uint8_t center);
void setContrast(uint8_t contrast);
void setBrightness(uint8_t birghtness);
void reversePixelBits();
void showColorBars(bool transparent);

inline void waitForVsync(void) __attribute__((always_inline));
inline void waitForPixelClockRisingEdge(void) __attribute__((always_inline));
inline void waitForPixelClockLow(void) __attribute__((always_inline));
inline void waitForPixelClockHigh(void) __attribute__((always_inline));
inline void ignoreHorizontalPaddingLeft(void) __attribute__((always_inline));
inline void ignoreHorizontalPaddingRight(void) __attribute__((always_inline));
inline void readPixelByte(uint8_t) __attribute__((always_inline));

void ignoreVerticalPadding();
bool setUpCamera();

void waitForVsync();
void waitForPixelClockRisingEdge();
void waitForPixelClockLow();
void waitForPixelClockHigh();
void ignoreHorizontalPaddingLeft();
void ignoreHorizontalPaddingRight();

void readPixelByte(uint8_t);

//registers
uint32_t resetSettings();
void setRegisters(const uint32_t *registerData);
bool setRegister(uint8_t addr, uint8_t val);
uint8_t readRegister(uint8_t addr);
void setRegisterBitsOR(uint8_t addr, uint8_t bits);
void setRegisterBitsAND(uint8_t addr, uint8_t bits);

void setDisablePixelClockDuringBlankLines();
void setDisableHREFDuringBlankLines();
void setHREFReverse();
void setInternalClockPreScaler(int preScaler);
void setPLLMultiplier(uint8_t multiplier);
void setManualContrastCenter(uint8_t center);
void setContrast(uint8_t contrast);
void setBrightness(uint8_t birghtness);
void reversePixelBits();
void setShowColorBar(uint32_t transparent);

#endif // _OV7670_h_
