/*
 Implement a breakout style game on a 5x7 LED matrix
 
 The 5 rows of the LED matrix are connected directly
 to arduino pins, while the 7 columns are controlled
 by a 74HC595 shift register.

 Wiring instructions:

    Arduino  6       -> 74HC595 14 (DATA)
    Arduino  5       -> 74HC595  9 (CLOCK)
    Arduino  4       -> 74HC595 12 (LATCH)

    Arduino 12       -> Matrix  1 (Row 1)
    Arduino 11       -> Matrix  3 (Row 2)
    Arduino 10       -> Matrix  9 (Row 3)
    Arduino  9       -> Matrix  8 (Row 4)
    Arduino  8       -> Matrix  7 (Row 5)

    74HC595 15 (Q0)  -> Matrix 12 (Col 1)
    74HC595  1 (Q1)  -> Matrix 11 (Col 2)
    74HC595  2 (Q2)  -> Matrix  2 (Col 3)
    74HC595  3 (Q3)  -> Matrix  9 (Col 4)
    74HC595  4 (Q4)  -> Matrix  4 (Col 5)
    74HC595  5 (Q5)  -> Matrix  5 (Col 6)
    74HC595  6 (Q6)  -> Matrix  6 (Col 7)

    74HC959  8 (GND) -> +5V
    74HC959 10 (MR)  -> +5V
    74HC959 13 (OE)  -> GND
    74HC959 16 (VCC) -> GND

    Arduino A0       -> Poti 10 kOhm
    Arduino  7       -> push button

*/

// functions for handling the display
#include "display.h"

#define BTN 7

int btn_state = 0;

// when did we last update the ball position
unsigned long lastmove;
// update the positions every _ milliseconds
int tick = 500;

struct {
	// position (the left edge of the paddle)
	int pos;
	int width;
} paddle;

struct {
	int x;
	int y;
	int vx;
	int vy;
	boolean thrown;
} ball;

boolean blocks[RES_X][RES_Y];

void throw_ball(int x) {
	ball.x = x;
	ball.y = RES_Y-2;
	ball.vx = 1;
	ball.vy = 1;
	ball.thrown = true;
}

boolean bounce_ball() {
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
	if ( (ball.y + ball.vy >= (RES_Y-1)) &&
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

void move_ball() {
	if (! ball.thrown) return;

	// bounce the ball around
	while (bounce_ball());
	// and move it once we have a new direction
	ball.x += ball.vx;
	ball.y += ball.vy;
}

void init_blocks() {
	for (int x=0; x<RES_X; x++) {
		for (int y=0; y<RES_Y; y++) {
			if (y<3) {
				blocks[x][y] = ( random(3) == 0 );
			} else {
				blocks[x][y] = false;
			}
		}
	}
}

int blocks_left() {
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

void setup() { 
	pinMode(13, OUTPUT);

	pinMode(A0, INPUT);
	// used for seeding the RNG
	pinMode(A1, INPUT);

	setup_display();

	paddle.width = 2;

	randomSeed(analogRead(A1));

	init_blocks();
	
	lastmove = millis();
}

void draw_items(void) {
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

void loop() {
	int val = analogRead(A0);
	clear_display();

	paddle.pos = map(val, 1023-10, 0, 0, RES_X-paddle.width);

	int new_val = digitalRead(BTN);
	if (new_val == HIGH && btn_state == LOW) {
		if (! ball.thrown) {
			throw_ball(paddle.pos);
		}
	}
	btn_state = new_val;

	draw_items();

	update_display();

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
