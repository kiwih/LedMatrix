/*
 * display_colors.h
 *
 * Created: 2/05/2020 6:38:21 PM
 *  Author: Hammond
 */ 


#ifndef DISPLAY_COLORS_H_
#define DISPLAY_COLORS_H_

#include <stdint.h>

typedef uint8_t pixel_color_t; //rrrgggbb

#define DISPLAY_RED		0b11100000
#define DISPLAY_GREEN	0b00011100
#define DISPLAY_BLUE	0b00000011
#define DISPLAY_YELLOW	0b11111100
#define DISPLAY_CYAN	0b00011111
#define DISPLAY_MAGENTA 0b11100011
#define DISPLAY_BLACK	0b00000000
#define DISPLAY_WHITE	0b11111111

#endif /* DISPLAY_COLORS_H_ */