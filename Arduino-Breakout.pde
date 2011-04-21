/*
 Implement a several games and programs
 on a 8x8 LED matrix

 Currently implemented subprograms:
   * Breakout
   * Conway's game of life
   * (Space) Invaders

*/

// functions for handling the display
#include "display.h"
#include "input.h"
#include "programs.h"

#include "TimerOne.h"

static int current_program = 1;

void load_program(int i) {
	programs[i].init();
	current_program = i;
}

void setup() { 
	// used for seeding the RNG
	pinMode(A1, INPUT);
	randomSeed(analogRead(A1));

	setup_display();
	// use Timer interrupt to update the display
	Timer1.initialize();
	Timer1.attachInterrupt(update_display, 500);
	setup_input();

	load_program(3);
}

void loop() {
	programs[current_program].loop();

	if (btn_pressed(1)) {
		load_program( (current_program+1)%PROGRAM_CNT );
	}
}
