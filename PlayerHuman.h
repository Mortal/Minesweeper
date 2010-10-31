#ifndef PLAYERHUMAN_H
#define PLAYERHUMAN_H
#include "Tick.h"
#include "Game.h"
#include "Player.h"
#include <string>
#include "types.h"
#include "Vector.hpp"

template<unsigned L>
class HumanCoordinateInput {
public:
	HumanCoordinateInput(std::string input, Dimension dimensioncount);
	CoordinateSetL get() {return this->_coords;};
	bool valid() {return this->_valid;};
private:
	bool _valid;
	CoordinateSetL _coords;
};

template<unsigned L>
class PlayerHuman: public Player<L> {
public:
	PlayerHuman(Game<L> *field, std::ostream *console);
	virtual ~PlayerHuman() {};
	virtual Tick<L> *tick();
private:
	Game<L> *field;
	void dead();
	std::ostream *console;
};

#endif
