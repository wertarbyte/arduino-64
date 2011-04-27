#include "WProgram.h"
#include "display.h"
#include "input.h"

// when did we last update the ecosphere
static unsigned long lastupdate;
// update the positions every _ milliseconds
static int tick = 300;

static int probability = 4;

static void change_display(void) {
	for (int x=0; x < RES_X; x++) {
                for (int y=0; y < RES_Y; y++) {
			set_pixel(x, y, random(4)==0);
		}
	}
	swap_displays();
}

void wopr_setup() { 
}

void wopr_loop() {
	int pval = pot_value(0);
	tick = map(pval, 0, 1023, 50, 2000);
	if (millis() > lastupdate+tick) {
		change_display();
		lastupdate = millis();
	}
}
