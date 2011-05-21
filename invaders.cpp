#include "WProgram.h"
#include "display.h"
#include "controller.h"

extern Controller input;

#include "invaders.h"

Invaders::Invaders() : ticks(1), enemy_direction(1) {
	ship.width = 3;
	missile.x = 0;
	missile.y = 0;
	missile.fired = false;
}

bool Invaders::can_move_x(int dir) {
	for (int i=0; i<n_enemies; i++) {
		int xpos = enemy[i].x+dir;
		if (enemy[i].alive && (xpos > RES_X-1 || xpos < 0)) {
			return false;
		}
	}
	return true;
}

bool Invaders::missile_hit(void) {
	for (int i=0; i<n_enemies; i++) {
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

void Invaders::move_items(void) {
	if (missile.fired && (ticks%2)==0) {
		missile.y--;
		// do we hit something?
		missile_hit();
	}
	// every 10 ticks, we move the enemies
	if (ticks % 20 == 0) {
		if (can_move_x(enemy_direction)) {
			for (int i=0; i<n_enemies; i++) {
				enemy[i].x += enemy_direction;
			}
		} else {
			enemy_direction = (enemy_direction>0?-1:1);
		}
	}
	// every 30 ticks, the invaders advance!
	if (ticks % 60 == 0) {
		for (int i=0; i<n_enemies; i++) {
			enemy[i].y -= -1;
		}
	}

}

Invaders::state_t Invaders::get_gamestate(void) {
	int alive = 0;
	for (int i=0; i<n_enemies; i++) {
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

void Invaders::draw_items(void) {
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
	for (int i=0; i<n_enemies; i++) {
		if (enemy[i].alive && enemy[i].y>=0 && enemy[i].y<RES_Y) {
			set_pixel(enemy[i].x, enemy[i].y, true);
		}
	}
}

void Invaders::fire_missile() {
	missile.x = ship.pos+1;
	missile.y = RES_Y-1;
	missile.fired = true;
}

void Invaders::create_enemies() {
	int x=2;
	int y=RES_Y-4;
	for (int i=0; i<n_enemies; i++) {
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

void Invaders::init() { 
	lastmove = millis();
	create_enemies();
	ship.pos = 0;
	missile.x = 0;
	missile.y = 0;
	missile.fired = false;
	game_state = RUNNING;
}

void Invaders::loop() {
	clear_display();
	if (input.pressed(Controller::START)) {
		init();
	}

	if (input.pressed(Controller::LEFT) && ship.pos < RES_X-ship.width) {
		ship.pos++;
	}
	if (input.pressed(Controller::RIGHT) && ship.pos > 0) {
		ship.pos--;
	}

	if (input.pressed(Controller::A)) {
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
