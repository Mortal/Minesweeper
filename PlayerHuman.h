#ifndef PLAYERHUMAN_H
#define PLAYERHUMAN_H
#include "Player.h"
#include <string>
#include "types.h"
#include "Tick.h"

class HumanCoordinateInput {
public:
	HumanCoordinateInput(std::string input, Dimension dimensioncount);
	CoordinateSet get() {return this->_coords;};
	bool valid() {return this->_valid;};
private:
	bool _valid;
	CoordinateSet _coords;
};

class PlayerHuman: public Player {
public:
	PlayerHuman(Game *field, std::ostream *console);
	virtual Tick *tick();
private:
	Game *field;
	void dead();
	std::ostream *console;
};

#endif
