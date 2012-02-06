#include "Arduino.h"
#include "display.h"
#include "controller.h"

extern Controller input;

#include "wopr.h"

void WOPR::change_display(void) {
	for (int x=0; x < RES_X; x++) {
		for (int y=0; y < RES_Y; y++) {
			set_pixel(x, y, random(4)==0);
		}
	}
	swap_displays();
}

WOPR::WOPR() : tick(300), probability(4), lastupdate(0) {
}

void WOPR::loop() {
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
