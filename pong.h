#include "program.h"
#include "controller.h"

class Pong : public Program {
	private:
		// when did we last update the ball position
		unsigned long lastmove;
		// update the positions every _ milliseconds
		int tick;

		struct paddle {
			int row;
			// position (the left edge of the paddle)
			int pos;
			int target;
			int width;
			Controller *ctl;
		};
		int serving;
		struct paddle player[2];

		struct ball {
			int xpos;
			int ypos;
			int dx;
			int dy;
			boolean thrown;
		} ball;

		void draw_items();
		boolean bounce_ball();
		void serve_ball();
		void move_ball();
		int aim(int x);
		void move_paddles();

	public:
		Pong();
		void init();
		void loop();
};
