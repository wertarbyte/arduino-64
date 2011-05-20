#include "WProgram.h"
#include "display.h"
#include "controller.h"

extern Controller input;

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
	if (input.pressed(Controller::UP, false)) {
		tick += 1;
	}
	if (input.pressed(Controller::DOWN, false)) {
		tick -= 1;
	}
	if (tick < 0) tick = 0;

	if (millis() > lastupdate+tick || input.pressed(Controller::A)) {
		change_display();
		lastupdate = millis();
	}
}
