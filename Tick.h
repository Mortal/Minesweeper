#ifndef TICK_H
#define TICK_H

#include "Move.h"
#include "types.h"

class Tick {
	public:
		Tick(std::string d);
		void addMove(Move *move);
		MoveList getMoves();
		std::string getDescription();
	private:
		MoveList moves;
		std::string desc;
};

#endif
