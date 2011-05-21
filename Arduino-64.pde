/*
 Arduino 64 - the 64 bit gaming platform 

 Implement a several games and programs
 on a 8x8 LED matrix using an NES gamepad

 Currently implemented subprograms:
   * WOPR (display random patterns)
   * Breakout
   * Conway's game of life
   * (Space) Invaders
   * Pong

*/
#include <stdlib.h>

void * operator new(size_t size) {
	return malloc(size);
}

void operator delete(void * ptr) {
	free(ptr);
} 

#include "display.h"
#include "programs.h"

#include "controller.h"

#include "TimerOne.h"

static int current_program = 0;

Controller input(11, 13, 12);

void load_program(int i) {
	programs[i]->init();
	current_program = i;
}

void setup() { 
	// used for seeding the RNG
	pinMode(A1, INPUT);
	randomSeed(analogRead(A1));

	setup_display();
	// use Timer interrupt to update the display
	Timer1.initialize();
	Timer1.attachInterrupt(update_display, 1000);

	load_program(current_program);
}

void loop() {
	input.refresh();

	programs[current_program]->loop();

	if (input.pressed(Controller::SELECT)) {
		load_program( (current_program+1)%PROGRAM_CNT );
	}
}
