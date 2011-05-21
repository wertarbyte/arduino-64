#include "program.h"

class Invaders : public Program {
	private:
		unsigned long lastmove;
		// update the positions every _ milliseconds
		static const unsigned short clock = 50;
		// How often did we update the positions?
		unsigned long ticks;

		struct ship {
			// position (the left edge of the ship)
			int pos;
			int width;
		} ship;

		struct missile {
			int x;
			int y;
			bool fired;
		} missile;

		enum state_t {
			RUNNING,
			WON,
			LOST,
		};

		state_t game_state;

		short enemy_direction;

		const static uint8_t n_enemies = 10;
		struct enemy {
			int x;
			int y;
			boolean alive;
		} enemy[n_enemies];
		void create_enemies();

		bool can_move_x(int dir);

		void move_items();
		state_t get_gamestate();

		void draw_items();

		void fire_missile();
		bool missile_hit();

	public:
		Invaders();
		void init();
		void loop();
};
