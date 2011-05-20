#ifndef CONTROLLER_H
#define CONTROLLER_H

class Controller {
	private:
		uint8_t strobe, clock, data;
		uint8_t state, oldstate;
		void fetchState();
	public:
		Controller(uint8_t strobe, uint8_t clock, uint8_t data);
		void refresh();
		enum Button {
			A = 0,
			B,
			SELECT,
			START,
			UP,
			DOWN,
			LEFT,
			RIGHT,
		};
		bool virtual pressed(Controller::Button btn);
		bool virtual pressed(Controller::Button btn, bool edge);
};

#endif
