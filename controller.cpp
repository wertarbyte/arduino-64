#include <stdint.h>
#include "WProgram.h"
#include "controller.h"

Controller::Controller(uint8_t istrobe, uint8_t iclock, uint8_t idata) :
	strobe(istrobe), clock(iclock), data(idata), state(~0), oldstate(~0) {
	pinMode(strobe, OUTPUT);
	pinMode(clock, OUTPUT);
	pinMode(data, INPUT);
}

void Controller::fetchState() {
	uint8_t result = 0;
	digitalWrite(strobe, HIGH);
	delayMicroseconds(20);
	digitalWrite(strobe, LOW);
	delayMicroseconds(20);
	for (uint8_t i=0; i<8; i++) {
		result |= (digitalRead(data)<<i);
		delayMicroseconds(20);
		digitalWrite(clock, HIGH);
		delayMicroseconds(20);
		digitalWrite(clock, LOW);
	}
	oldstate = state;
	state = ~result;
}

void Controller::refresh() {
	// shift in the data
	fetchState();
}

bool Controller::pressed(Controller::Button btn) {
	pressed(btn, true);
}

bool Controller::pressed(Controller::Button btn, bool edge) {
	if (edge) {
		return (state & 1<<btn) && (~oldstate & 1<<btn);
	} else {
		return (state & 1<<btn);
	}
}
