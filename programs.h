#include "breakout.h"
#include "conway.h"
#include "invaders.h"
#include "pong.h"
#include "wopr.h"

struct program {
	void (*init)(void);
	void (*loop)(void);
};

#define PROGRAM_CNT 5
program programs[] = {
	/* WOPR */
	{ &wopr_setup, &wopr_loop },
	/* Breakout */
	{ &breakout_setup, &breakout_loop },
	/* Conway's Game of Life */
	{ &conway_setup, &conway_loop },
	/* Space Invaders */
	{ &invaders_setup, &invaders_loop },
	/* Pong */
	{ &pong_setup, &pong_loop },
};
