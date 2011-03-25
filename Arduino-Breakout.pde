/*
 Implement a breakout style game on a 5x7 LED matrix
 
 The 5 rows of the LED matrix are connected directly
 to arduino pins, while the 7 columns are controlled
 by a 74HC595 shift register.
*/

// shift register pins
#define DATA 6
#define CLOCK 5
#define LATCH 4

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

int active_row = 0;

// when did we last update the ball position
unsigned long lastmove;
// update the positions every _ milliseconds
int tick = 500;

// player position (the left edge of the paddle)
int pos = 0;
// paddle width
int p_width = 2;

struct {
	int x;
	int y;
	int vx;
	int vy;
} ball;

void load_line(byte line) {
	digitalWrite(LATCH, LOW);
	shiftOut(DATA, CLOCK, MSBFIRST, ~(line));
	digitalWrite(LATCH, HIGH);
}

void throw_ball() {
	ball.x = random(D_COLS);
	ball.y = 0;
	ball.vx = -1+(random(1)*2);
	ball.vy = -1;
}

void move_ball() {
	// detect collisions
	if (ball.x - ball.vx < 0 || ball.x - ball.vx >= D_COLS) {
		ball.vx *= -1;  
	}
	if (ball.y - ball.vy < 0) {
		ball.vy *= -1;
	}
	// does the ball hit the paddle?
	if ( (ball.y - ball.vy == (D_ROWS-1)) &&
	     (ball.x - ball.vx >= pos && ball.x - ball.vx - p_width < pos) ) {
		ball.vy *= -1;  
		// detect contact with the paddle edge
		if (ball.x < pos || ball.x >= pos+p_width) {
			ball.vx *= -1;
		}
	}
	ball.x -= ball.vx;
	ball.y -= ball.vy;
}

void setup() { 
	pinMode(13, OUTPUT);
	for (int i=0; i<D_ROWS; i++) {
		pinMode(rows[i], OUTPUT);  
	}

	pinMode(A0, INPUT);

	pinMode(DATA, OUTPUT);
	pinMode(CLOCK, OUTPUT);
	pinMode(LATCH, OUTPUT);
	
	lastmove = millis();
	throw_ball();
}

void loop() {
	int val = analogRead(A0);

	pos = map(val, 0, 1023-10, 0, D_COLS-p_width);

	digitalWrite(13, HIGH);   // set the LED on

	int line = 0;
	// draw the player
	if (active_row == (D_ROWS-1)) {
		line |= ((1<<p_width)-1)<<pos;
	}
	if (active_row == ball.y) {
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
	// lost the ball? wait a moment, then throw in a new one
	if (ball.y > (D_ROWS-1)) {
		delay(2000);
		throw_ball();
	}  

	// advance to next line
	active_row = (active_row+1) % D_ROWS;
}
