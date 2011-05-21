#include "program.h"

class Conway : public Program {
	private:
		// How do we handle the border of the field?
		enum border_type {
			B_DEAD,
			B_ALIVE,
			B_WRAPPED,
		};

		enum seed_style {
			// load a predefined configuration on start
			S_PRESET,
			// populate the field randomly
			S_RANDOM,
		};

		// when did we last update the ecosphere
		unsigned long lastupdate;
		// update the positions every _ milliseconds
		int tick;

		int current;
		bool field[2][RES_X][RES_Y];

		void clear_field();

		typedef struct {
			border_type border;
			seed_style seed;
			/* to make designing presets easier, this is swapped */
			bool field[RES_Y][RES_X];
		} conway_preset;

		int active_preset;
		static Conway::conway_preset presets[];

		conway_preset *get_preset() {
			return &presets[active_preset];
		}

		void seed_field();
		int cells_alive();
		short neighbours(int x, int y, bool f[RES_X][RES_Y]);
		int update_field();
		void field_to_display();

	public:
		Conway();
		void init();
		void loop();
};
