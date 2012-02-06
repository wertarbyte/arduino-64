#include "Arduino.h"
#include "display.h"

static byte display[2][RES_Y];

static int current = 0;

/*
 *   8x8 LED Matrix connected through two shift registers
 */

// shift register pins
#define DATA 6
#define CLOCK 4
#define LATCH 3

#define X_OFFSET 0
#define Y_OFFSET 0

static int current_row = 0;

void setup_display(void) {
	pinMode(DATA, OUTPUT);
	pinMode(CLOCK, OUTPUT);
	pinMode(LATCH, OUTPUT);
	clear_display();
	swap_displays();
}

void set_pixel(int x, int y, boolean value) {
	if (value) {
		display[!current][y] |= 1<<x;
	} else {
		display[!current][y] &= ~(1<<x);
	}
}

boolean get_pixel(int x, int y) {
	return (display[!current][y] & 1<<x) != 0;
}

void clear_display(void) {
	for (int x=0; x<RES_X; x++) {
		for (int y=0; y<RES_Y; y++) {
			set_pixel(x, y, false);
		}
	}
}

void swap_displays(void) {
	current = !current;
}

void update_display(void) {
	digitalWrite(LATCH, LOW);
	shiftOut(DATA, CLOCK, LSBFIRST, 1<<(current_row-Y_OFFSET));
	shiftOut(DATA, CLOCK, LSBFIRST, (display[current][current_row]<<X_OFFSET));
	digitalWrite(LATCH, HIGH);
	current_row = (current_row+1)%RES_Y;
}
