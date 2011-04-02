/*
 Implement a several games and programs
 on a 5x7 LED matrix

 Currently implemented subprograms:
   * Breakout
   * Conway's game of life
   * (Space) Invaders

 The LED matrix is wired to two shift registers
 (74HC595), one controlling the columns, the other
 controlling the 5 rows. That way, only 3 pins of
 the arduino modules are needed to control the
 display.

 Wiring instructions:

    Arduino  6       -> 74HC595 14 (DATA)
    Arduino  5       -> 74HC595  9 (CLOCK)
    Arduino  4       -> 74HC595 12 (LATCH)

    Arduino 12       -> Matrix  1 (Row 1)
    Arduino 11       -> Matrix  3 (Row 2)
    Arduino 10       -> Matrix  9 (Row 3)
    Arduino  9       -> Matrix  8 (Row 4)
    Arduino  8       -> Matrix  7 (Row 5)

    SR1     15 (Q0)  -> Matrix 12 (Col 1)
    SR1      1 (Q1)  -> Matrix 11 (Col 2)
    SR1      2 (Q2)  -> Matrix  2 (Col 3)
    SR1      3 (Q3)  -> Matrix  9 (Col 4)
    SR1      4 (Q4)  -> Matrix  4 (Col 5)
    SR1      5 (Q5)  -> Matrix  5 (Col 6)
    SR1      6 (Q6)  -> Matrix  6 (Col 7)

    SR2     15 (Q0)  -> Matrix  8 (Col 5)
    SR2      1 (Q1)  -> Matrix  7 (Col 4)
    SR2      2 (Q2)  -> Matrix 10 (Col 3)
    SR2      3 (Q3)  -> Matrix  3 (Col 2)
    SR2      4 (Q4)  -> Matrix  1 (Col 1)

    (each one using a 120 Ohm resistor)

    SR1/SR2  8 (GND) -> +5V
    SR1/SR2 10 (MR)  -> +5V
    SR1/SR2 13 (OE)  -> GND
    SR1/SR2 16 (VCC) -> GND

    Arduino A0       -> Poti 10 kOhm
    Arduino  7       -> push button
    Arduino  1       -> push button

*/

// functions for handling the display
#include "display.h"
#include "input.h"
#include "programs.h"

static int current_program = 0;

void load_program(int i) {
	programs[i].init();
	current_program = i;
}

void setup() { 
	// used for seeding the RNG
	pinMode(A1, INPUT);
	randomSeed(analogRead(A1));

	setup_display();
	setup_input();

	load_program(0);
}

void loop() {
	update_display();
	programs[current_program].loop();

	if (btn_pressed(1)) {
		load_program( (current_program+1)%PROGRAM_CNT );
	}
}
