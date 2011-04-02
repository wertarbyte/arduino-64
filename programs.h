#include "breakout.h"
#include "conway.h"

struct program {
	void (*init)(void);
	void (*loop)(void);
};

#define PROGRAM_CNT 2
program programs[PROGRAM_CNT] = {
	/* Breakout */
	{ &breakout_setup, &breakout_loop },
	/* Conway's Game of Life */
	{ &conway_setup, &conway_loop },
};
