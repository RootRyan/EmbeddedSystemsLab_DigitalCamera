// Graphics.c
// Ryan Root, Pranesh Satish, Jacob Dach
// Initial Creation Date: 9/15/21
// Lab 03
// TA: Hassan Iqbal
// Description: Adds a draw circle function with user-inputted radius
// Last Modified: 9/24/21

#include <stdint.h>
#include <stdio.h>
#include "ST7735.h"

//extern uint32_t Hours_current,Mins_current,Secs_current;

void Graphics_DrawCircle(int x0, int y0, int radius, int color)
{
  int x = radius;
  int y = 0;
  int decisionOver2 = 1 - x;   // Decision criterion divided by 2 evaluated at x=r, y=0

  while( y <= x )
  {
    ST7735_DrawPixel( x + x0,  y + y0, color); // Octant 1
    ST7735_DrawPixel( y + x0,  x + y0, color); // Octant 2
    ST7735_DrawPixel(-x + x0,  y + y0, color); // Octant 4
    ST7735_DrawPixel(-y + x0,  x + y0, color); // Octant 3
    ST7735_DrawPixel(-x + x0, -y + y0, color); // Octant 5
    ST7735_DrawPixel(-y + x0, -x + y0, color); // Octant 6
    ST7735_DrawPixel( x + x0, -y + y0, color); // Octant 7
    ST7735_DrawPixel( y + x0, -x + y0, color); // Octant 8
    y++;
    if (decisionOver2<=0)
    {
      decisionOver2 += 2 * y + 1;   // Change in decision criterion for y -> y+1
    }
    else
    {
      x--;
      decisionOver2 += 2 * (y - x) + 1;   // Change for y -> y+1, x -> x-1
    }
  }
}
