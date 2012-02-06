#include "Arduino.h"
#include "display.h"
#include "controller.h"

extern Controller input;

#include "conway.h"

#include "conway-presets.h"

Conway::Conway() : tick(300), current(0), active_preset(0), lastupdate(0) {}

void Conway::clear_field() {
	for (int x=0; x < RES_X; x++) {
		for (int y=0; y < RES_Y; y++) {
			field[current][x][y] = false;
		}
	}
}

void Conway::seed_field() {
	const conway_preset *ps = get_preset();
	for (int x=0; x < RES_X; x++) {
		for (int y=0; y < RES_Y; y++) {
			if (ps->seed == S_PRESET) {
				// mirror the y axis to reflect the presentation in the source
				field[current][x][y] = ps->field[y][x];
			} else if (ps->seed == S_RANDOM) {
				field[current][x][y] = (random(6)==0) ? true : false;
			}
		}
	}
}

int Conway::cells_alive() {
	int n = 0;
	for (int x=0; x < RES_X; x++) {
		for (int y=0; y < RES_Y; y++) {
			if (field[current][x][y]) {
				n++;
			}
		}
	}
	return n;
}

short Conway::neighbours(int x, int y, bool f[RES_X][RES_Y]) {
	const conway_preset *ps = get_preset();
	int n = 0;
	for (int dx=-1; dx<=+1; dx++) {
		for (int dy=-1; dy<=+1; dy++) {
			if (dx == 0 && dy == 0) continue;
			if (ps->border == B_DEAD) {
				if (x+dx >= RES_X || x+dx < 0);
				else if (y+dy >= RES_Y || y+dy < 0);
				else if (f[x+dx][y+dy]) n++;
			} else if (ps->border == B_ALIVE) {
				if (x+dx >= RES_X || x+dx < 0) n++;
				else if (y+dy >= RES_Y || y+dy < 0) n++;
				else if (f[x+dx][y+dy]) n++;
			} else if (ps->border == B_WRAPPED) {
				// make sure x+dx and y+dy are > 0
				if (f[(RES_X+x+dx)%RES_X][(RES_Y+y+dy)%RES_Y]) n++;
                        }
                }
        }
        return n;
}

int Conway::update_field() {
        int changes = 0;
        int next = 1 - current;
        for (int x=0; x < RES_X; x++) {
                for (int y=0; y < RES_Y; y++) {
                        // calculate number of neihgbours
                        int n = neighbours(x,y,field[current]);
                        // a living cell dies if it has less than 2 neighbours
                        if (n<2 && field[current][x][y]) {
                                field[next][x][y] = false;
                                changes++;
                        // a living cell dies if it has more than 3 neighbours
                        } else if (n>3 && field[current][x][y]) {
                                field[next][x][y] = false;
                                changes++;
                        // a dead cell comes alive if it has exactly 3 neighbours
                        } else if (n == 3 && ! field[current][x][y]) {
                                field[next][x][y] = true;
                                changes++;
                        } else {
                                field[next][x][y] = field[current][x][y];
                        }
                }
        }
        current = next;
        return changes;
}

void Conway::field_to_display(void) {
	for (int x=0; x < RES_X; x++) {
                for (int y=0; y < RES_Y; y++) {
			set_pixel(x, y, field[current][x][y]);
		}
	}
	swap_displays();
}

void Conway::init() { 
	clear_field();
	seed_field();
	field_to_display();
	lastupdate = millis();
}

void Conway::loop() {
	if (input.pressed(Controller::UP, false)) {
		tick++;
	}
	if (input.pressed(Controller::DOWN, false)) {
		tick--;
	}
	if (tick < 0) tick = 0;

	if (input.pressed(Controller::A)) {
		clear_field();
		active_preset = (active_preset+1)%N_PRESETS;
		seed_field();
		delay(100);
	}
	if (millis() > lastupdate+tick) {
		int changes = update_field();
		field_to_display();
		// reboot the field once a sta(b)le configuration is formed
		if (changes == 0) {
			seed_field();
		}
		lastupdate = millis();
	}
}
