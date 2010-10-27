#ifndef PLAYERHUMAN_H
#define PLAYERHUMAN_H
#include "Player.h"
#include <string>
#include "types.h"

class HumanCoordinateInput {
public:
	HumanCoordinateInput(std::string input, int dimensioncount);
	CoordinateSet get() {return this->_coords;};
	bool valid() {return this->_valid;};
private:
	bool _valid;
	CoordinateSet _coords;
};

class PlayerHuman: public Player {
public:
	PlayerHuman(Game *field);
	virtual void play();
private:
	Game *field;
	void dead();
};

#endif
