#ifndef TICK_H
#define TICK_H

#include "types.h"
#include "Move.h"

template<unsigned L>
class Tick {
	public:
		Tick(std::string d);
		void addMove(Move<L> *move);
		MoveLList getMoves();
		std::string getDescription();
	private:
		MoveLList moves;
		std::string desc;
};

#endif
