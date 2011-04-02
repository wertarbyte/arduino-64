#include "WProgram.h"
#include "display.h"

static boolean display[RES_X][RES_Y];

/*
 *   7x5 LED Matrix connected through a shift register
 */

// shift register pins
#define DATA 6
#define CLOCK 5
#define LATCH 4

#define X_OFFSET 1
#define Y_OFFSET -3

/* Device size:
 * since the display is used in landscape orientation,
 * the X axis runs along the physical display rows
 */
#define D_ROWS RES_Y
#define D_COLS RES_X

static int current_row = 0;

void setup_display(void) {
	pinMode(DATA, OUTPUT);
	pinMode(CLOCK, OUTPUT);
	pinMode(LATCH, OUTPUT);
	clear_display();
}

void set_pixel(int x, int y, boolean value) {
	display[x][y] = value;
}

void clear_display(void) {
	for (int x=0; x<RES_X; x++) {
		for (int y=0; y<RES_Y; y++) {
			set_pixel(x, y, false);
		}
	}
}

void update_display(void) {
	int line = 0;
	for (int x=0; x<RES_X; x++) {
		if (display[x][current_row]) {
			line |= (1<<(RES_X-x-X_OFFSET));
		}
	}
	digitalWrite(LATCH, LOW);
	shiftOut(DATA, CLOCK, LSBFIRST, 1<<(current_row-Y_OFFSET));
	shiftOut(DATA, CLOCK, MSBFIRST, ~(line));
	digitalWrite(LATCH, HIGH);
	delay(2);
	current_row = (current_row+1)%D_ROWS;
}
