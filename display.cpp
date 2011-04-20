#include "WProgram.h"
#include "display.h"

static byte display[RES_Y];

/*
 *   7x5 LED Matrix connected through two shift registers
 */

// shift register pins
#define DATA 6
#define CLOCK 3
#define LATCH 4

#define X_OFFSET 0
#define Y_OFFSET 0

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
	//display[x][y] = value;
	if (value) {
		display[y] |= 1<<x;
	} else {
		display[y] &= ~(1<<x);
	}
}

boolean get_pixel(int x, int y) {
	return (display[y] & 1<<x) != 0;
}

void clear_display(void) {
	for (int x=0; x<RES_X; x++) {
		for (int y=0; y<RES_Y; y++) {
			set_pixel(x, y, false);
		}
	}
}

void update_display(void) {
	digitalWrite(LATCH, LOW);
	shiftOut(DATA, CLOCK, LSBFIRST, 1<<(current_row-Y_OFFSET));
	shiftOut(DATA, CLOCK, LSBFIRST, (display[current_row]<<X_OFFSET));
	digitalWrite(LATCH, HIGH);
	current_row = (current_row+1)%D_ROWS;
}
