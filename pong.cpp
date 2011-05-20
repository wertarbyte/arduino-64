#include "WProgram.h"
#include "display.h"
#include "controller.h"


// when did we last update the ball position
static unsigned long lastmove;
// update the positions every _ milliseconds
static int tick = 300;

struct paddle {
	int row;
	// position (the left edge of the paddle)
	int pos;
	int target;
	const int width;
};

static int serving = 0;

static struct paddle player[2] = {
	{ 0,       RES_Y/2-1, RES_Y/2-1, 3 },
	{ RES_X-1, RES_Y/2-1, RES_Y/2-1, 3 },
};

static struct {
	int xpos;
	int ypos;
	int dx;
	int dy;
	boolean thrown;
} ball = {
	RES_X/2-1,
	RES_Y/2-1,
	1,
	1,
	false
};

static void draw_items(void) {
	// draw paddles
	for (int i=0; i<2; i++) {
		struct paddle *p = &player[i];
		for (int y=p->pos; y<= p->pos+p->width-1; y++) {
			set_pixel( p->row, y, true );
		}
	}
	// draw ball
	if (ball.thrown) {
		set_pixel( ball.xpos, ball.ypos, true );
	}
}

static boolean bounce_ball(void) {
	int nx = ball.xpos + ball.dx;
	int ny = ball.ypos + ball.dy;
	/* collision with upper/lower border */
	if (ny < 0 || ny > RES_Y-1) {
		ball.dy *= -1;
		return 1;
	}
	/* collision with a paddle */
	for (uint8_t i=0; i<2; i++) {
		volatile struct paddle *p = &player[i];
		if ( nx == p->row && ny >= p->pos && ny < (p->pos + p->width)) {
			ball.dx *= -1;
			/* did we hit the edge? */
			if ( ball.ypos > (p->pos + p->width-1) || ball.ypos < p->pos ) {
				ball.dy *= -1;
			}
			/* transfer impulse from paddle to ball */
			//ball.dy += (p->pos - p->last_pos);
			return 1;
		}
	}
	return 0;
}

static void serve_ball(void) {
	int dir = ((player[serving].row - player[!serving].row) > 0 ? -1 : 1);
	ball.xpos = player[serving].row + dir;
	ball.ypos = player[serving].pos;
	ball.dy = 1;
	ball.dx = dir;
	ball.thrown = true;
	serving = !serving;
}

static void move_ball(void) {
	while (bounce_ball());
	ball.xpos += ball.dx;
	ball.ypos += ball.dy;
	if (ball.xpos < 0 || ball.xpos > RES_X-1) {
		ball.thrown = 0;
	}
}

static int aim(int x) {
	int8_t dist_x = x - ball.xpos;
	// how many ticks until it intersects?
	int8_t t = dist_x/ball.dx;
	if (t<0) {
		// the ball is moving away from the paddle
		return -1;
	}
	// where will it be on the y scale then?
	int8_t y = ball.ypos + (t * ball.dy);
	// simulate the mirroring at the border
	while (y < 0 || y > (RES_Y-1)) {
		if (y < 0) y = abs(y);
		else if (y > (RES_Y-1)) y = ((RES_Y-1)-(y-(RES_Y-1)));
	}
	return y;
}

static void move_paddles(void) {
	for (uint8_t i=0; i<2; i++) {
		volatile struct paddle *p = &player[i];
		if (p->target < p->pos) p->pos--;
		else if (p->target > p->pos) p->pos++;
		else {
			int prediction = aim(p->row);
			if (prediction < 0) {
				p->target = random( RES_Y-p->width );
			} else {
				p->target = max(0, min(prediction, RES_Y-p->width));
			}
		}
	}
}

void pong_setup() { 
	lastmove = millis();
}

void pong_loop() {
	clear_display();

	draw_items();
	swap_displays();

	// update the ball positions
	if (millis() > lastmove+tick) {
		if (! ball.thrown) {
			serve_ball();
		}
		move_paddles();
		move_ball();
		lastmove = millis();
	}
}
