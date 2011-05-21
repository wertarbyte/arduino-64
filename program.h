#ifndef PROGRAM_H
#define PROGRAM_H

class Program {
	public:
		virtual void init() {};
		virtual void loop() = 0;
};

#endif
