#include "WProgram.h"
#include "input.h"

#define BTN_CNT 1
static struct { uint8_t pin; boolean state; } button[BTN_CNT] = {
	{ 7, LOW },
};

#define POT_CNT 1
static struct { uint8_t pin; } pot[POT_CNT] = {
	{ A0 },
};

void setup_input(void) {
	for (int i=0; i<BTN_CNT; i++) {
		pinMode(button[i].pin, INPUT);
	}
	for (int i=0; i<POT_CNT; i++) {
		pinMode(pot[i].pin, INPUT);
	}
}

boolean btn_pressed(int id) {
	int r = false;
	int new_val = digitalRead(button[id].pin);
	if (new_val == HIGH && button[id].state == LOW) {
		r = true;
	}
	button[id].state = new_val;
	return r;
}

int pot_value(int id) {
	return analogRead(pot[id].pin);
}
