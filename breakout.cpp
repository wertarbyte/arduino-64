#include "WProgram.h"
#include "display.h"
#include "controller.h"

extern Controller input;

#include "breakout.h"

Breakout::Breakout() : tick(500) {
	init();
}

void Breakout::init() {
	paddle.width = 3;
	paddle.pos = RES_X/2-(paddle.width/2);
	ball.x = 0;
	ball.y = RES_Y-2;
	ball.vx = 1;
	ball.vy = -1;
	ball.thrown = false;

	init_blocks();
	lastmove = millis();
}

void Breakout::throw_ball(int x) {
	ball.x = x;
	ball.y = RES_Y-2;
	ball.vx = 1;
	ball.vy = -1;
	ball.thrown = true;
}

bool Breakout::bounce_ball() {
	// detect collisions with the surroundings
	if (ball.x + ball.vx < 0 || ball.x + ball.vx >= RES_X) {
		ball.vx *= -1;  
		return true;
	}
	if (ball.y + ball.vy < 0) {
		ball.vy *= -1;
		return true;
	}
	// does the ball hit the paddle?
	if ( (ball.y + ball.vy >= (RES_Y-1)) && (ball.vy > 0) &&
	     (ball.x + ball.vx >= paddle.pos && (ball.x + ball.vx) < paddle.pos+paddle.width) ) {
		ball.vy *= -1;
		// detect contact with the paddle edge
		if (ball.x < paddle.pos || ball.x >= paddle.pos+paddle.width) {
			ball.vx *= -1;
		}
		return true;
	}
	// did the ball pass the paddle?
	if (ball.y + ball.vy > (RES_Y-1)) {
		return false;
	}
	// handle block collisions
	if (blocks[ball.x][ball.y + ball.vy]) {
		blocks[ball.x][ball.y + ball.vy] = false;
		ball.vy *= -1;
		return true;
	}
	if (blocks[ball.x + ball.vx][ball.y]) {
		blocks[ball.x + ball.vx][ball.y] = false;
		ball.vx *= -1;
		return true;
	}
	if (blocks[ball.x + ball.vx][ball.y + ball.vy]) {
		blocks[ball.x + ball.vx][ball.y + ball.vy] = false;
		ball.vx *= -1;
		ball.vy *= -1;
		return true;
	}
	return false;
}

void Breakout::move_ball() {
	if (! ball.thrown) return;

	// bounce the ball around
	while (bounce_ball());
	// and move it once we have a new direction
	ball.x += ball.vx;
	ball.y += ball.vy;
}

void Breakout::init_blocks() {
	for (int x=0; x<RES_X; x++) {
		for (int y=0; y<RES_Y; y++) {
			if (y<RES_Y-2) {
				blocks[x][y] = ( random(y+1) == 0 );
			} else {
				blocks[x][y] = false;
			}
		}
	}
}

int Breakout::blocks_left() {
	int n = 0;
	for (int x=0; x<RES_X; x++) {
		for (int y=0; y<RES_Y; y++) {
			if (blocks[x][y] == true) {
				n++;
			}
		}
	}
	return n;
}

void Breakout::draw_items(void) {
	/* paddle */
	for (int i=paddle.pos; i<paddle.pos+paddle.width; i++) {
		set_pixel(i, RES_Y-1, true);
	}
	/* ball */
	if (ball.thrown) {
		set_pixel(ball.x, ball.y, true);
	}
	/* blocks */
	for (int x=0; x<RES_X; x++) {
		for (int y=0; y<RES_Y; y++) {
			if (blocks[x][y]) {
				set_pixel(x, y, true);
			}
		}
	}
}

void Breakout::loop() {
	clear_display();

	if (input.pressed(Controller::START)) {
		init();
		return;
	}

	if (input.pressed(Controller::RIGHT) && paddle.pos > 0) {
		paddle.pos--;
	}
	if (input.pressed(Controller::LEFT) && paddle.pos < RES_X-paddle.width) {
		paddle.pos++;
	}

	if (input.pressed(Controller::A)) {
		if (! ball.thrown) {
			throw_ball(paddle.pos);
		}
	}
	draw_items();
	swap_displays();

	// update the ball positions
	if (millis() > lastmove+tick) {
		move_ball();
		lastmove = millis();
	}
	// lost the ball?
	if (ball.thrown && ball.y > (RES_Y-1)) {
		ball.thrown = false;
	}  
	// removed all blocks? restart
	if (blocks_left() == 0) {
		init_blocks();
		ball.thrown = false;
	}
}
