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

// pins controlling the rows
const int rows[5] = {
	12,
	11,
	10,
	9,
	8
};

int active_row = 0;

// when did we last update the ball position
int lastmove;
// update the positions every _ milliseconds
int tick = 500;

// player position
int pos = 0;

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
	ball.x = random(7);
	ball.y = 0;
	ball.vx = -1+(random(1)*2);
	ball.vy = -1;
}

void move_ball() {
	// detect collisions
	if (ball.x - ball.vx < 0 || ball.x - ball.vx >= 7) {
		ball.vx *= -1;  
	}
	if (ball.y - ball.vy < 0 || ((ball.x - ball.vx) == pos && ball.y == 3) ) {
		ball.vy *= -1;  
	}
	ball.x -= ball.vx;
	ball.y -= ball.vy;
}

void setup() { 
	pinMode(13, OUTPUT);
	for (int i=0; i<5; i++) {
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

	pos = map(val, 0, 1023-10, 0, 6);

	digitalWrite(13, HIGH);   // set the LED on

	int line = 0;
	// draw the player
	if (active_row == 4) {
		line |= 1<<pos;
	}
	if (active_row == ball.y) {
		line |= 1<<(ball.x);
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
	if (ball.y > 4) {
		delay(2000);
		throw_ball();
	}  

	// advance to next line
	active_row = (active_row+1) % 5;
}
