#include "WProgram.h"
#include "display.h"
#include "controller.h"

extern Controller input;

#include "pong.h"

Pong::Pong() : serving(0), tick(300) {
	player[0] = (struct paddle){0, RES_Y/2-1, RES_Y/2-1, 3, NULL};
	player[1] = (struct paddle){RES_X-1, RES_Y/2-1, RES_Y/2-1, 3, NULL};

	ball = (struct ball){ RES_X/2-1, RES_Y/2-1, 1, 1, false };
}

void Pong::draw_items(void) {
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

boolean Pong::bounce_ball(void) {
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

void Pong::serve_ball(void) {
	int dir = ((player[serving].row - player[!serving].row) > 0 ? -1 : 1);
	ball.xpos = player[serving].row + dir;
	ball.ypos = player[serving].pos;
	ball.dy = 1;
	ball.dx = dir;
	ball.thrown = true;
	serving = !serving;
}

void Pong::move_ball(void) {
	while (bounce_ball());
	ball.xpos += ball.dx;
	ball.ypos += ball.dy;
	if (ball.xpos < 0 || ball.xpos > RES_X-1) {
		ball.thrown = 0;
	}
}

int Pong::aim(int x) {
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

void Pong::move_paddles(void) {
	for (uint8_t i=0; i<2; i++) {
		volatile struct paddle *p = &player[i];
		if (p->ctl != NULL) {
			// manual control enabled
			if (p->ctl->pressed(Controller::LEFT, false)) p->pos++;
			if (p->ctl->pressed(Controller::RIGHT, false)) p->pos--;
		} else {
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
		// ensure the position is valid
		p->pos = max(0, min(p->pos, RES_Y-p->width));
	}
}

void Pong::init() {
	lastmove = millis();
}

void Pong::loop() {
	clear_display();

	draw_items();
	swap_displays();

	// take over a player?
	if (input.pressed(Controller::START)) {
		if (player[0].ctl == NULL) {
			player[0].ctl = &input;
		} else {
			player[0].ctl = NULL;
		}
	}

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
