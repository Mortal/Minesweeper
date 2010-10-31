#ifndef PLAYER_H
#define PLAYER_H
#include "Tick.h"
template<unsigned L>
class Player {
public:
	Player(class Game<L> *field) {};
	virtual ~Player() {};
	virtual Tick<L> *tick() = 0;
protected:
	Player() {};
};
#endif
