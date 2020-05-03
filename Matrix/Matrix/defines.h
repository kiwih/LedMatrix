/*
 * defines.h
 *
 * Created: 2/05/2020 5:59:48 PM
 *  Author: Hammond
 */ 


#ifndef DEFINES_H_
#define DEFINES_H_

#define F_CPU 16000000

//CLK and LAT are on PORT C
#define DDR_MATRIX_CTRL DDRC
#define PORT_MATRIX_CTRL PORTC
#define PIN_CLK 0
#define PIN_LAT	1

//OE is associated with PORT B3 as this goes with PWM for TIMER2 (OC2A pin is same pin as PB3)
#define DDR_MATRIX_OE DDRB
#define PORT_MATRIX_OE PORTB
#define PIN_OE 3

//color pins on PORT D
#define DDR_MATRIX_COLORS DDRD
#define PORT_MATRIX_COLORS PORTD
#define PIN_R1 0
#define PIN_R2 1
#define PIN_B1 2
#define PIN_B2 3
#define PIN_G1 4
#define PIN_G2 5


#endif /* COMMON_H_ */