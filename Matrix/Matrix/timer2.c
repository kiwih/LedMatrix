/*
 * timer2.c
 *
 * Created: 3/05/2020 1:37:41 PM
 *  Author: Hammond
 */ 

 #include "timer2.h"
 #include "defines.h"

 #include <avr/io.h>

 void Timer2_Init() {
	//we will be PWMing on OC2A which is on PB3
	DDRB |= (1 << PORTB3);

	//my power supply is a bit underwhelming so to protect it we're going to PWM the OE at a pretty relaxed rate
	//let's have it running at just 6.25% intensity (16 cyles on, 240 cycles off)

	TCCR2A = 0b10000011; //fast PWM, Clear OC2A on Compare Match, set OC2A at BOTTOM (non-inverting mode)
	TCCR2B = 0b00000010; //clk/8
	OCR2A = 239; //239; //count 240 cycles OE=high (display disabled) and 16 cycles low (display enabled) = 16 on cycles
 }