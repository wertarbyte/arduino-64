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
	if (input.pressed(Controller::UP)) {
		tick += 50;
	}
	if (input.pressed(Controller::DOWN)) {
		tick -= 50;
	}
	if (tick < 10) tick = 10;

	if (millis() > lastupdate+tick || input.pressed(Controller::A)) {
		change_display();
		lastupdate = millis();
	}
}
