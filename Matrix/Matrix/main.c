/*
 * Matrix.c
 *
 * Created: 2/05/2020 3:41:56 PM
 * Author : Hammond
 */ 

#include "defines.h"

#include "display.h"
#include "timer0.h"
#include "timer2.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


int main(void)
{
	DDRB |= (1 << PORTB5); //for the LED on the xplained mini, we'll use it to ensure our interrupt is triggering

	Display_Init();
	Display_Fill(DISPLAY_BLACK);
	Display_SwapBuffers();
	Timer0_Init(); //refreshes the display
	Timer2_Init(); //pwm the OE

	sei();
	pixel_color_t current_color = DISPLAY_WHITE;
	while(1)
    {
		
		for(uint8_t c = ' '; c <= '~'; c++) {
			Display_Fill(DISPLAY_BLACK);
			Display_DrawChar(0, 8, c, 1, DISPLAY_WHITE);
			Display_DrawString(0, 0, "hi", 1, DISPLAY_WHITE);
			Display_SwapBuffers();
			_delay_ms(500);
		}

		//draw some rotating lines!
		
		/*for(uint8_t color_count = 0; color_count < 7; color_count++) {
			switch(color_count) {
			case 0:
				current_color = DISPLAY_RED;
				break;
			case 1:
				current_color = DISPLAY_GREEN;
				break;
			case 2:
				current_color = DISPLAY_BLUE;
				break;
			case 3:
				current_color = DISPLAY_CYAN;
				break;
			case 4:
				current_color = DISPLAY_MAGENTA;
				break;
			case 5:
				current_color = DISPLAY_YELLOW;
				break;
			default:
				current_color = DISPLAY_WHITE;
				break;

			}
			for(uint8_t count = 0; count < 15; count++) {
				Display_Fill(DISPLAY_BLACK);
				Display_DrawLine(count, 0, 15-count, 15, current_color);
				Display_SwapBuffers();
				_delay_ms(75);
			}
		
			for(uint8_t count = 0; count < 15; count++) {
				Display_Fill(DISPLAY_BLACK);
				Display_DrawLine(0, 15-count, 15, count, current_color);
				Display_SwapBuffers();
				_delay_ms(75);
			}
		} 
		
		//Draw a smiley face!
		Display_Fill(DISPLAY_BLACK);
		Display_DrawCircle(8,8,7,DISPLAY_YELLOW);
		Display_DrawCircle(6,6,2,DISPLAY_YELLOW);
		Display_DrawCircle(10,6,2,DISPLAY_YELLOW);
		Display_DrawPixel(6,7,DISPLAY_YELLOW);
		Display_DrawPixel(10,7,DISPLAY_YELLOW);

		Display_DrawLine(6,12,10,12,DISPLAY_YELLOW);
		Display_DrawPixel(5,11,DISPLAY_YELLOW);
		Display_DrawPixel(11,11,DISPLAY_YELLOW);

		Display_DrawPixel(8,13,DISPLAY_YELLOW);
		Display_DrawPixel(9,13,DISPLAY_YELLOW);
		Display_SwapBuffers();
		_delay_ms(1000);//*/
		
		
		//Disabled below because it's hecking bright
	
		/*
		
		Display_Fill(DISPLAY_RED);
		Display_SwapBuffers();
		_delay_ms(2000);
		Display_Fill(DISPLAY_GREEN);
		Display_SwapBuffers();
		_delay_ms(2000);
		Display_Fill(DISPLAY_BLUE);
		Display_SwapBuffers();
		_delay_ms(2000);
		Display_Fill(DISPLAY_CYAN);
		Display_SwapBuffers();
		_delay_ms(2000);
		Display_Fill(DISPLAY_YELLOW);
		Display_SwapBuffers();
		_delay_ms(2000);
		Display_Fill(DISPLAY_MAGENTA);
		Display_SwapBuffers();
		_delay_ms(2000);
		Display_Fill(DISPLAY_WHITE);
		Display_SwapBuffers();
		_delay_ms(2000);
		Display_Fill(DISPLAY_BLACK);
		Display_SwapBuffers();
		_delay_ms(2000);*/
		
    }
}

uint16_t count = 0;
ISR(TIMER0_COMPA_vect) {
	if(count++ >= 100) {
		count = 0;
		PORTB = ~PORTB; //flash the on-board LED
	}
	Display_TransmitBuffer();
}
