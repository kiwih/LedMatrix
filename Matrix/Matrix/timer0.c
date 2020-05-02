/*
 * timer0.c
 *
 * Created: 2/05/2020 7:09:25 PM
 *  Author: Hammond
 */ 
 #include "timer0.h"
 
 #include <avr/io.h>

 void Timer0_Init() {
 	//Task 3
	//clock is 16,000,000 / 1024 = each tick is 64 uS
	OCR0A = 250; //count 250 cycles of clk/1024 = 64uS * 250 = 16ms
 	TCCR0A = 0b00000010; //output disabled, CTC mode
 	TCCR0B = 0b00000101; //no forcing, CTC mode, clk/1024
 	TIMSK0 |= (1 << OCIE0A); //enable interrupts on compare A
 }