/*
 Implement a breakout style game on a 5x7 LED matrix
 
 The 5 rows of the LED matrix are connected directly
 to arduino pins, while the 7 columns are controlled
 by a 74HC595 shift register.

 Wiring instructions:

    Arduino  6       -> 74HC595 14 (DATA)
    Arduino  5       -> 74HC595  9 (CLOCK)
    Arduino  4       -> 74HC595 12 (LATCH)

    Arduino 12       -> Matrix  1 (Row 1)
    Arduino 11       -> Matrix  3 (Row 2)
    Arduino 10       -> Matrix  9 (Row 3)
    Arduino  9       -> Matrix  8 (Row 4)
    Arduino  8       -> Matrix  7 (Row 5)

    74HC595 15 (Q0)  -> Matrix 12 (Col 1)
    74HC595  1 (Q1)  -> Matrix 11 (Col 2)
    74HC595  2 (Q2)  -> Matrix  2 (Col 3)
    74HC595  3 (Q3)  -> Matrix  9 (Col 4)
    74HC595  4 (Q4)  -> Matrix  4 (Col 5)
    74HC595  5 (Q5)  -> Matrix  5 (Col 6)
    74HC595  6 (Q6)  -> Matrix  6 (Col 7)

    74HC959  8 (GND) -> +5V
    74HC959 10 (MR)  -> +5V
    74HC959 13 (OE)  -> GND
    74HC959 16 (VCC) -> GND

    Arduino A0       -> Poti 10 kOhm
    Arduino  7       -> push button

*/

// functions for handling the display
#include "display.h"
#include "input.h"
#include "breakout.h"


void setup() { 
	// used for seeding the RNG
	pinMode(A1, INPUT);
	randomSeed(analogRead(A1));

	setup_display();
	setup_input();

	// the actual game code
	breakout_setup();
}

void loop() {
	breakout_loop();
	update_display();
}
