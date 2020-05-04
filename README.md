# MATRIX

I found an LED matrix in the bin outside a business. So I took it home and got it going!

I used an Atmega Xplained Mini (Atmega328PB) to control the module, and a 12V 2A DC PSU unit to provide power (note power requirements below)

Note: This is also published on [my blog](https://01001000.xyz/2020-05-03-Rescuing-a-trashed-LED-Matrix/)!

## Investigation

First port of call was to examine the PCB for the control scheme. I found an INPUT port, an OUTPUT port (presumably for daisy chaining) and 12V/GND port.
The input port had a few labels, including OE, CLK, LAT, R1/R2/G1/G2/B1/B2, and 2*GND.
The CLK hinted that this was a serial connection of some kind, and I could guess that OE stood for Output Enable. 
Obviously also there was an R, G, and B component to the signal, but I wasn't sure how these fit in yet, or why there was two wires for each.
I grabbed a bunch of the modules, and then headed home. On the way, I sent a message to a former colleague who has experience with digital signage what the typical control scheme was. 
He said 99% of the time it was just simple shift register based electronics, no complex IIC or SPI or CAN protocols.

When I got home, I looked up the main IC that was located across the board - the MBI5024. Looks like it's a shift register - my former colleague was right!
The labels on the bottom were very helpful, except for LAT, which is equidistant between two pins. To make matters worse, due to coronavirus, I'm stuck at home with no proper gear.
Nonetheless, I wrote a basic program to shift in a constant stream of 1's on G1, and tried the first possible LAT pin, and then the other - with the other, the top half of the module lit up in green!
Tried on G2. The bottom half lit up!
That explains the "1"/"2" on the colors, and the location of LAT.

I then shifted in the signal slowly. Interestingly, it didn't fill a whole row as it came in - instead, it filled segments of the display. It seems that the LEDs are wired in a peculiar way - presumably to make
the traces easier. I discussed this more under Pixel Addressing below.

At any rate, I was ready to program a full controller! 

## Wiring the full controller

The LED matrix has got an INPUT port, an OUTPUT port, and a 12V/GND ports.
Here's the pinout for the INPUT port.

### Top down view of INPUT port:
```
+------------------+      +------------------+
|                  +------+                  |
|                                            |
| OE   CLK   NC   NC    B2    R2    B1    R1 |
|                                            |
| NC   LAT   NC   NC    GND   G2    GND   G1 |
|                                            |
+--------------------------------------------+
```

* OE = Output Enable, set to 0 to enable output, 1 to disable (active low)
* CLK = Clock Data, use to capture data on R1/R2/B1/B2/G1/G2 pins
* LAT = Latch, latch in all current data ready to output
* R1/R2/B1/B2/G1/G2 = Data pins, loaded in parallel.

To the Xplained Mini:

* Control pins to PORTC, Colors to PORTD, as depicted in code segment below;
* Two exposed GNDs connected to xplained mini,
* Xplained Mini USB to my computer

```
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

```

Power supply:
* 12V to 12V 2A DC PSU,
* GND to GND PSU,

### Pixel addressing

This is awkward. All pixels are individually addressable but when you shift them in via the INPUT port it's a bit of a chore.
Imagine you have a canvas of 16 pixels by 16 pixels.
You address it as such:
```
+-----+-----+-----+-----+-----+ ... +-----+ 
|   0 |   1 |   2 |   3 |   4 |     |  15 |
+-----+-----+-----+-----+-----+ ... +-----+ 
|  16 |  17 |  18 |  19 |  20 |     |  31 |
+-----+-----+-----+-----+-----+ ... +-----+ 
:     :     :     :     :     :     :     :
+-----+-----+-----+-----+-----+ ... +-----+ 
| 239 | 240 | 241 | 242 | 243 |     | 255 |
+-----+-----+-----+-----+-----+ ... +-----+ 
```
To shift in this canvas to display it, you need to load each segment in the following way:
1. The shift registers are wired such that we output 4 pixels in a row, then the 4 pixels beneath them, then the 4 pixels beneath them... until 7 rows down, then we shift over one segment and repeat
```
0  1  2  3   /
16 17 18 19  /
 ....
4  5  6  7   /
etc.
```
2. Then, to make it slightly more complicated, we need to do the bottom 8 rows at the same time as the top 8 rows.
3. (That is, the top and bottom 8 rows are loaded in parallel.)

You may check Display_TransmitBuffer() to see how I did this.

## Power requirements
After some investigation with a couple of colleagues we determined that one module can draw a whopping 4A of current!
Obviously this wasn't going to do given I only had a 2A power supply at home (it runs, but the PSU is unhappy).
To resolve this, I apply a PWM signal to the OE input pin on the module. 
The module is actually painfully bright (even at 2A with an overwhelmed PSU), so I configure the PWM signal to have the display active just 6.25% of the time.
You can check this in Timer2_Init() to see my configuration.
