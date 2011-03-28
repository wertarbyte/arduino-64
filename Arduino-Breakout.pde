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

// shift register pins
#define DATA 6
#define CLOCK 5
#define LATCH 4

#define BTN 7

#define D_ROWS 5
#define D_COLS 7

// pins controlling the rows
const int rows[D_ROWS] = {
	12,
	11,
	10,
	9,
	8
};

int btn_state = 0;

int active_row = 0;

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

boolean blocks[D_ROWS][D_COLS];

void load_line(byte line) {
	digitalWrite(LATCH, LOW);
	shiftOut(DATA, CLOCK, MSBFIRST, ~(line));
	digitalWrite(LATCH, HIGH);
}

void throw_ball(int x) {
	ball.x = x;
	ball.y = D_ROWS-2;
	ball.vx = 1;
	ball.vy = 1;
	ball.thrown = true;
}

int bounce_ball() {
	int bounces = 0;
	// detect collisions with the surroundings
	if (ball.x - ball.vx < 0 || ball.x - ball.vx >= D_COLS) {
		ball.vx *= -1;  
		bounces++;
	}
	if (ball.y - ball.vy < 0) {
		ball.vy *= -1;
		bounces++;
	}
	// does the ball hit the paddle?
	if ( (ball.y - ball.vy >= (D_ROWS-1)) &&
	     (ball.x - ball.vx >= paddle.pos && (ball.x - ball.vx) < paddle.pos+paddle.width) ) {
		ball.vy *= -1;
		bounces++;
		// detect contact with the paddle edge
		if (ball.x < paddle.pos || ball.x >= paddle.pos+paddle.width) {
			ball.vx *= -1;
			bounces++;
		}
	}
	// the ball might have left the field. Let's check...
	if (ball.y - ball.vy >= D_ROWS) {
		return 0;
	}
	// handle block collisions
	if (blocks[ball.y][ball.x - ball.vx]) {
		blocks[ball.y][ball.x - ball.vx] = false;
		ball.vx *= -1;
		bounces++;
	}
	if (blocks[ball.y - ball.vy][ball.x]) {
		blocks[ball.y - ball.vy][ball.x] = false;
		ball.vy *= -1;
		bounces++;
	}
	if (blocks[ball.y - ball.vy][ball.x - ball.vx]) {
		blocks[ball.y - ball.vy][ball.x - ball.vx] = false;
		ball.vy *= -1;
		ball.vx *= -1;
		bounces++;
	}
	return bounces;
}

void move_ball() {
	if (! ball.thrown) return;

	// bounce the ball around
	while (bounce_ball() > 0);
	// and move it once we have a new direction
	ball.x -= ball.vx;
	ball.y -= ball.vy;
}

void init_blocks() {
	for (int r=0; r<D_ROWS; r++) {
		for (int c=0; c<D_COLS; c++) {
			if (r<2) {
				blocks[r][c] = (random(2) == 0 ? true : false);
			} else {
				blocks[r][c] = false;
			}
		}
	}
}

int blocks_left() {
	int n = 0;
	for (int r=0; r<D_ROWS; r++) {
		for (int c=0; c<D_COLS; c++) {
			if (blocks[r][c]) {
				n++;
			}
		}
	}
	return n;
}

void setup() { 
	pinMode(13, OUTPUT);
	for (int i=0; i<D_ROWS; i++) {
		pinMode(rows[i], OUTPUT);  
	}

	pinMode(A0, INPUT);
	// used for seeding the RNG
	pinMode(A1, INPUT);

	pinMode(DATA, OUTPUT);
	pinMode(CLOCK, OUTPUT);
	pinMode(LATCH, OUTPUT);

	paddle.width = 2;

	randomSeed(analogRead(A1));

	init_blocks();
	
	lastmove = millis();
}

void loop() {
	int val = analogRead(A0);

	paddle.pos = map(val, 0, 1023-10, 0, D_COLS-paddle.width);

	int new_val = digitalRead(BTN);
	if (new_val == HIGH && btn_state == LOW) {
		if (! ball.thrown) {
			throw_ball(paddle.pos);
		}
	}
	btn_state = new_val;

	digitalWrite(13, HIGH);   // set the LED on

	int line = 0;
	// draw blocks
	for (int c=0; c<D_COLS; c++) {
		if (blocks[active_row][c]) {
			line |= (B1)<<c;
		}
	}

	// draw the player
	if (active_row == (D_ROWS-1)) {
		line |= ((1<<paddle.width)-1)<<paddle.pos;
	}
	if (ball.thrown && active_row == ball.y) {
		line |= B1<<(ball.x);
	}
	load_line(line);

	digitalWrite(rows[active_row], HIGH);
	delay(2);
	digitalWrite(13, LOW);
	digitalWrite(rows[active_row], LOW);  

	// update the ball positions
	if (millis() > lastmove+tick) {
		move_ball();
		lastmove = millis();
	}
	// lost the ball?
	if (ball.thrown && ball.y > (D_ROWS-1)) {
		ball.thrown = false;
	}  
	// removed all blocks? restart
	if (blocks_left() == 0) {
		init_blocks();
		ball.thrown = false;
	}

	// advance to next line
	active_row = (active_row+1) % D_ROWS;
}
