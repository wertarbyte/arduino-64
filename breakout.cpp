#include "WProgram.h"
#include "display.h"
#include "input.h"

// when did we last update the ball position
static unsigned long lastmove;
// update the positions every _ milliseconds
static int tick = 500;

static struct {
	// position (the left edge of the paddle)
	int pos;
	int width;
} paddle;

static struct {
	int x;
	int y;
	int vx;
	int vy;
	boolean thrown;
} ball;

static boolean blocks[RES_X][RES_Y];

static void throw_ball(int x) {
	ball.x = x;
	ball.y = RES_Y-2;
	ball.vx = 1;
	ball.vy = -1;
	ball.thrown = true;
}

static boolean bounce_ball() {
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

static void move_ball() {
	if (! ball.thrown) return;

	// bounce the ball around
	while (bounce_ball());
	// and move it once we have a new direction
	ball.x += ball.vx;
	ball.y += ball.vy;
}

static void init_blocks() {
	for (int x=0; x<RES_X; x++) {
		for (int y=0; y<RES_Y; y++) {
			if (y<1) {
				blocks[x][y] = ( random(3) == 0 );
			} else {
				blocks[x][y] = false;
			}
		}
	}
}

static int blocks_left() {
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

static void draw_items(void) {
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

void breakout_setup() { 
	paddle.width = 2;
	paddle.pos = 1;
	init_blocks();
	lastmove = millis();
}

void breakout_loop() {
	clear_display();

	int val = pot_value(0);
	paddle.pos = map(val, 1023-10, 0, 0, RES_X-paddle.width);

	if (btn_pressed(0)) {
		if (! ball.thrown) {
			throw_ball(paddle.pos);
		}
	}
	draw_items();

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
