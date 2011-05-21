#include "program.h"
#include "wopr.h"
#include "pong.h"
#include "breakout.h"
#include "conway.h"
#include "invaders.h"

Program *programs[] = {
	new WOPR(),
	new Conway(),
	new Pong(),
	new Breakout(),
	new Invaders(),
};

#define PROGRAM_CNT (sizeof(programs)/sizeof(programs[0]))
