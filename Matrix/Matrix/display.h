/*
 * display.h
 *
 * Created: 2/05/2020 6:00:05 PM
 *  Author: Hammond
 */ 


#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "display_colors.h"

#include <stdint.h>

void Display_Init();

void Display_SwapBuffers();

void Display_TransmitBuffer();

void Display_Fill(pixel_color_t px);

void Display_DrawPixel(uint8_t x, uint8_t y, pixel_color_t px);

void Display_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, pixel_color_t px);

void Display_DrawCircle(uint8_t xc, uint8_t yc, uint8_t r, pixel_color_t px);

#endif /* DISPLAY_H_ */