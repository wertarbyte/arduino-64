#include "WProgram.h"
#include "display.h"
#include "input.h"

// when did we last update the positions
static unsigned long lastmove;
// update the positions every _ milliseconds
static int clock = 50;
// How often did we update the positions?
static unsigned long ticks = 1;

static struct {
	// position (the left edge of the ship)
	int pos;
	int width;
} ship;

static struct {
	int x;
	int y;
	boolean fired;
} missile;

enum state {
	RUNNING,
	WON,
	LOST,
} game_state;

short enemy_direction = 1;

#define ENEMIES 17
static struct {
	int x;
	int y;
	boolean alive;
} enemy[ENEMIES];

boolean can_move_x(int dir) {
	for (int i=0; i<ENEMIES; i++) {
		int xpos = enemy[i].x+dir;
		if (enemy[i].alive && (xpos > RES_X-1 || xpos < 0)) {
			return false;
		}
	}
	return true;
}

static boolean missile_hit(void) {
	for (int i=0; i<ENEMIES; i++) {
		if (enemy[i].alive && missile.fired &&
		    enemy[i].x == missile.x &&
		    enemy[i].y == missile.y
		   ) {
			enemy[i].alive = false;
			missile.fired = false;
			return true;
		}
	}
	return true;
}

static void move_items(void) {
	if (missile.fired && (ticks%2)==0) {
		missile.y--;
		// do we hit something?
		missile_hit();
	}
	// every 10 ticks, we move the enemies
	if (ticks % 20 == 0) {
		if (can_move_x(enemy_direction)) {
			for (int i=0; i<ENEMIES; i++) {
				enemy[i].x += enemy_direction;
			}
		} else {
			enemy_direction = (enemy_direction>0?-1:1);
		}
	}
	// every 30 ticks, the invaders advance!
	if (ticks % 60 == 0) {
		for (int i=0; i<ENEMIES; i++) {
			enemy[i].y -= -1;
		}
	}

}

static state get_gamestate(void) {
	int alive = 0;
	for (int i=0; i<ENEMIES; i++) {
		if (enemy[i].alive && enemy[i].y >= RES_Y-1) {
			return LOST; // invaders won
		}
		if (enemy[i].alive) {
			alive++;
		}
	}
	if (alive) {
		return RUNNING; // no winner yet
	} else {
		return WON; // player won
	}

}

static void draw_items(void) {
	/* ship */
	for (int i=ship.pos; i<ship.pos+ship.width; i++) {
		set_pixel(i, RES_Y-1, true);
	}
	/* missile */
	if (missile.fired) {
		// we make the missile flicker
		set_pixel(missile.x, missile.y, ((ticks%2)==0));
	}
	/* enemies */
	for (int i=0; i<ENEMIES; i++) {
		if (enemy[i].alive && enemy[i].y>=0 && enemy[i].y<RES_Y) {
			set_pixel(enemy[i].x, enemy[i].y, true);
		}
	}
}

static void fire_missile() {
	missile.x = ship.pos+1;
	missile.y = RES_Y-1;
	missile.fired = true;
}

static void create_enemies() {
	int x=2;
	int y=RES_Y-4;
	for (int i=0; i<ENEMIES; i++) {
		if (x>=RES_X-2) {
			x = 1;
			y -= 2;
		}
		enemy[i].x = x;
		enemy[i].y = y;
		enemy[i].alive = true;
		x += 2;
	}
}

void invaders_setup() { 
	ticks = 1;
	ship.width = 3;
	ship.pos = 0;
	lastmove = millis();
	create_enemies();
	game_state = RUNNING;
}

void invaders_loop() {
	clear_display();

	int val = pot_value(0);
	ship.pos = map(val, 1023-10, 0, 0, RES_X-ship.width);

	if (btn_pressed(0)) {
		if (! missile.fired && game_state == RUNNING) {
			fire_missile();
		}
	}
	draw_items();
	swap_displays();

	// update the positions
	if (millis() > lastmove+clock && game_state == RUNNING) {
		move_items();
		lastmove = millis();
		ticks++;
	}
	// missile left the field?
	if (missile.fired && missile.y < 0) {
		missile.fired = false;
	} 
	// invaders did...invade?
	game_state = get_gamestate();
}
