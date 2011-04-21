#include "breakout.h"
#include "conway.h"
#include "invaders.h"
#include "pong.h"

struct program {
	void (*init)(void);
	void (*loop)(void);
};

#define PROGRAM_CNT 4
program programs[PROGRAM_CNT] = {
	/* Breakout */
	{ &breakout_setup, &breakout_loop },
	/* Conway's Game of Life */
	{ &conway_setup, &conway_loop },
	/* Space Invaders */
	{ &invaders_setup, &invaders_loop },
	/* Pong */
	{ &pong_setup, &pong_loop },
};
