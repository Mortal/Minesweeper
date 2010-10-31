#ifndef PLAYERROBOT_H
#define PLAYERROBOT_H
#include "Player.h"
#include <string>
#include "types.h"
#include "Vector.hpp"
#include "Timer.h"
#include "Tick.h"
#include <fstream>
#include <stack>

template<unsigned L>
class PlayerRobot: public Player<L> {
public:
	PlayerRobot<L>(Game<L> *field, std::ostream *console, ProgramOptions opts, NullTimer *timer = NULL);
	virtual ~PlayerRobot<L>() {};
	virtual Tick<L> *tick();
	void setTimer(NullTimer *timer);
private:
	NullTimer *timer;
	Game<L> *field;

	Tick<L> *act_singleflagging(CoordinateSetL tile, Tile *);
	Tick<L> *act_safespots(CoordinateSetL tile, Tile *);
	Tick<L> *act_dualcheck(CoordinateSetL tile, Tile *);
	Tick<L> *act_safemap(CoordinateSetL tile, Tile *);

	bool croaking;
	void ncroak(std::string msg);
	void croak(std::string msg);
	void croak(std::string msg, CoordinateSetL a, CoordinateSetL b);
	void croakend();
	void croakstatus();

	ProgramOptions opts;
	std::ostream *console;

	bool allowcoordreset;
	CoordinateSetLList::const_iterator coord;
	CoordinateSetLStack coords;
	CoordinateSetL nextcoord();
};

class NoMoreCoordinatesException : public std::exception {
	virtual const char* what() const throw() {
		return "No more coordinates";
	};

	public:
		virtual ~NoMoreCoordinatesException () throw() {};
} static NoMoreCoordinates;
#endif
