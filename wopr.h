#include "program.h"

class WOPR : public Program {
	private:
		// when did we last update the ecosphere
		unsigned long lastupdate;
		// update the positions every _ milliseconds
		int tick;
		int probability;

		void change_display(void);
	public:
		WOPR();
		void loop();
};
