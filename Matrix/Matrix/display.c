/*
 * display.c
 *
 * Created: 2/05/2020 5:59:35 PM
 *  Author: Hammond
 */ 

#include "display.h"

#include "defines.h"

#include <avr/io.h>
#include <util/delay.h>

#define DISPLAY_WIDTH 16
#define DISPLAY_HEIGHT 16

#define ABS(X)      ((X) > 0 ? (X) : -(X))
#define MIN(X,Y)    ((X) < (Y) ? (X) : (Y))

static volatile pixel_color_t scrbuf1[DISPLAY_WIDTH*DISPLAY_HEIGHT] = {0}; //we'll use 8 bits for each pixel at the moment, format R  R  R  G  G  G  B  B
static volatile pixel_color_t scrbuf2[DISPLAY_WIDTH*DISPLAY_HEIGHT] = {0}; //we'll use 8 bits for each pixel at the moment, format R  R  R  G  G  G  B  B

volatile pixel_color_t* frontbuf; //currently being rendered
volatile pixel_color_t* backbuf; //currently being manipulated

void Display_Init() {
	DDR_MATRIX_CTRL |= (1 << PIN_CLK) | (1 << PIN_LAT) | (1 << PIN_OE);
	DDR_MATRIX_COLORS |= (1 << PIN_R1) | (1 << PIN_R2) | (1 << PIN_G1) | (1 << PIN_G2) | (1 << PIN_B1) | (1 << PIN_B2);
	frontbuf = scrbuf1;
	backbuf = scrbuf2;
}

void Display_SwapBuffers() {
	if(frontbuf == scrbuf1) {
		frontbuf = scrbuf2;
		backbuf = scrbuf1;
	} else {
		frontbuf = scrbuf1;
		backbuf = scrbuf2;
	}
}

void Display_TransmitBuffer() {
	//1. Turn off the display (so there's no flicker) by writing a 1 to OE
	DDR_MATRIX_CTRL |= (1 << PIN_OE);

	//2. Emit the screen buffer, pixel by pixel, into the shift registers.
	//Note: getting everything out in the right order is a chore
	//the shift registers are wired such that we output 4 pixels in a row, then the 4 pixels beneath them, then the 4 pixels beneath them... until 7 rows down,
	//then we shift over one segment and repeat
	//0  1  2  3   /
	//16 17 18 19  /
	// ....
	//4  5  6  7   /
	// etc.
	//Then, to make it slightly more complicated, we need to do the bottom 8 rows at the same time as the top 8 rows.
	//That is, the top and bottom 8 rows are loaded in parallel.
	//We call each pixel as we do this as px1 and px2 going to rgb1 and rgb2.

	//Whew! Overall, we get it all out with the following triple nested for loop:
	for(uint8_t seg = 0; seg < 4; seg++) {			//for each segment of 4 columns
		for(uint8_t row = 0; row < 8; row++) {		//for each 8 rows in the segment
			for(uint8_t col = 0; col < 4; col++) {	//for each column in the 4 columns

				pixel_color_t px1 = frontbuf[seg * 4 + col + DISPLAY_WIDTH*row];		//convert (seg, col, row) to pixel address for the top 8 rows
				pixel_color_t px2 = frontbuf[seg * 4 + col + DISPLAY_WIDTH*row + 128];	//now add 128 to that and you have the pixel in the bottom 8 rows as well

				uint8_t r1 = (px1 & 0b11100000) >> 5; //extract rgb for px1
				uint8_t g1 = (px1 & 0b00011100) >> 2;
				uint8_t b1 = (px1 & 0b00000011);

				uint8_t r2 = (px2 & 0b11100000) >> 5; //extract rgb for px2
				uint8_t g2 = (px2 & 0b00011100) >> 2;
				uint8_t b2 = (px2 & 0b00000011);

				//load rgb onto output pins for px1 and px2
				if(r1) {
					PORT_MATRIX_COLORS |= (1 << PIN_R1);
				} else {
					PORT_MATRIX_COLORS &= ~(1 << PIN_R1);
				}
				if(r2) {
					PORT_MATRIX_COLORS |= (1 << PIN_R2);
				} else {
					PORT_MATRIX_COLORS &= ~(1 << PIN_R2);
				}
				if(g1) {
					PORT_MATRIX_COLORS |= (1 << PIN_G1);
				} else {
					PORT_MATRIX_COLORS &= ~(1 << PIN_G1);
				}
				if(g2) {
					PORT_MATRIX_COLORS |= (1 << PIN_G2);
				} else {
					PORT_MATRIX_COLORS &= ~(1 << PIN_G2);
				}
				if(b1) {
					PORT_MATRIX_COLORS |= (1 << PIN_B1);
				} else {
					PORT_MATRIX_COLORS &= ~(1 << PIN_B1);
				}
				if(b2) {
					PORT_MATRIX_COLORS |= (1 << PIN_B2);
				} else {
					PORT_MATRIX_COLORS &= ~(1 << PIN_B2);
				}

				//now toggle the CLK pin to store the rgb1 and rgb2
				_delay_us(1); //first delay is for settling time
				PORT_MATRIX_CTRL |= (1 << PIN_CLK);
				_delay_us(1);
				PORT_MATRIX_CTRL &= ~(1 << PIN_CLK);

				//rinse and repeat!
			}
		}
	}
	
	//3. Now all data is loaded, latch it to the outputs by toggling the LAT pin
	_delay_us(2); //first delay is for settling time
	PORT_MATRIX_CTRL |= (1 << PIN_LAT);
	_delay_us(2);
	PORT_MATRIX_CTRL &= ~(1 << PIN_LAT);

	//4. Now we re-enable the display 
	PORT_MATRIX_CTRL &= ~(1 << PIN_OE); //this line is correct
}

//Fill the screen buffer with a solid colour
void Display_Fill(pixel_color_t px) {
	for(uint32_t i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
		backbuf[i] = px;
	}
}

//Fill a pixel at a given address.
void Display_DrawPixel(uint8_t x, uint8_t y, pixel_color_t px) {
	backbuf[x + DISPLAY_WIDTH*y] = px;
}

//Bresenham's line algorithm adapted from Rosetta Code website
//https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C
void Display_DrawLine(
	uint8_t x0,        //location of start of line x
	uint8_t y0,        //location of start of line y
	uint8_t x1,        //location of end of line x
	uint8_t y1,        //location of end of line y
	pixel_color_t px   //color of line
) {
	int8_t dx = ABS(x1-x0), sx = x0<x1 ? 1 : -1;
	int8_t dy = ABS(y1-y0), sy = y0<y1 ? 1 : -1;
	int8_t err = (dx>dy ? dx : -dy)/2, e2;

	for(;;){
		Display_DrawPixel(x0, y0, px);

		if (x0==x1 && y0==y1)
			break;

		e2 = err;
		if (e2 >-dx) {
			err -= dy; x0 += sx;
		}
		if (e2 < dy) {
			err += dx; y0 += sy;
		}
	}
}

//Midpoint circul drawing algorithm adapted from
//https://www.geeksforgeeks.org/mid-point-circle-drawing-algorithm/
//(I found that the provided code didn't deal with the initial 4 points correctly)
void Display_DrawCircle(uint8_t xc, uint8_t yc, uint8_t r, pixel_color_t px)
{
	int16_t x = r, y = 0;
	
	// Printing the initial point on the axes
	// after translation
	Display_DrawPixel(xc + r, yc, px);
	
	// When radius is zero only a single
	// point will be printed
	if (r > 0)
	{
		Display_DrawPixel(xc, yc - r, px);
		Display_DrawPixel(xc - r, yc, px);
		Display_DrawPixel(xc, yc + r, px);
	}
	
	int16_t P = 1 - r;
	while (x > y)
	{
		y++;
		
		// Mid-point is inside or on the perimeter
		if (P <= 0)
		P = P + 2*y + 1;
		
		// Mid-point is outside the perimeter
		else
		{
			x--;
			P = P + 2*y - 2*x + 1;
		}
		
		// All the perimeter points have already been printed
		if (x < y)
		break;
		
		// Printing the generated point and its reflection
		// in the other octants after translation
		Display_DrawPixel(x + xc, y + yc, px);
		Display_DrawPixel(-x + xc, y + yc, px);
		Display_DrawPixel(x + xc, -y + yc, px);
		Display_DrawPixel(-x + xc, -y + yc, px);
		
		// If the generated point is on the line x = y then
		// the perimeter points have already been printed
		if (x != y)
		{
			Display_DrawPixel(y + xc, x + yc, px);
			Display_DrawPixel(-y + xc, x + yc, px);
			Display_DrawPixel(y + xc, -x + yc, px);
			Display_DrawPixel(-y + xc, -x + yc, px);
		}
	}
}