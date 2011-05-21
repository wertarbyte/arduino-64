#include "program.h"

class Breakout : public Program {
	private:
		// when did we last update the ball position
		unsigned long lastmove;
		// update the positions every _ milliseconds
		int tick;

		struct paddle {
			int pos;
			int width;
		} paddle;

		struct ball {
			int x;
			int y;
			int vx;
			int vy;
			boolean thrown;
		} ball;

		bool blocks[RES_X][RES_Y];

		void init_blocks();
		int blocks_left();

		void throw_ball(int x);
		void move_ball();
		bool bounce_ball();

		void draw_items();
		int aim(int x);
		void move_paddles();

	public:
		Breakout();
		void init();
		void loop();
};
