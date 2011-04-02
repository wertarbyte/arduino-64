#include "WProgram.h"
#include "display.h"
#include "input.h"

// How do we handle the border of the field?
enum border_type {
	B_DEAD,
	B_ALIVE,
	B_WRAPPED,
};

enum seed_style {
	// load a predefined configuration on start
	S_PRESET,
	// populate the field randomly
	S_RANDOM
};

// when did we last update the ecosphere
static unsigned long lastupdate;
// update the positions every _ milliseconds
static int tick = 300;

static int current = 0;
static boolean field[2][RES_X][RES_Y];

static void clear_field() {
	for (int x=0; x < RES_X; x++) {
		for (int y=0; y < RES_Y; y++) {
			field[current][x][y] = false;
		}
	}
}

typedef struct {
	border_type border;
	seed_style seed;
	/* to make designing presets easier, this is swapped */
	bool field[RES_Y][RES_X];
} conway_preset;

#include <conway-presets.h>

static inline const conway_preset *get_preset() {
	return &presets[active_preset];
}

static void seed_field() {
	const conway_preset *ps = get_preset();
	for (int x=0; x < RES_X; x++) {
		for (int y=0; y < RES_Y; y++) {
			if (ps->seed == S_PRESET) {
				// mirror the y axis to reflect the presentation in the source
				field[current][x][RES_Y-1-y] = ps->field[y][x];
			} else if (ps->seed == S_RANDOM) {
				field[current][x][y] = (random(6)==0) ? true : false;
			}
		}
	}
}

static int cells_alive() {
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

static short neighbours(int x, int y, boolean f[RES_X][RES_Y]) {
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

static int update_field() {
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

static void field_to_display(void) {
	clear_display();
	for (int x=0; x < RES_X; x++) {
                for (int y=0; y < RES_Y; y++) {
			set_pixel(x, y, field[current][x][y]);
		}
	}
}

void conway_setup() { 
	clear_field();
	seed_field();
}

void conway_loop() {
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
