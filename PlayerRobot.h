#ifndef PLAYERROBOT_H
#define PLAYERROBOT_H
#include "Player.h"
#include <string>
#include "types.h"
#include "Timer.h"
#include <fstream>
#include <stack>
typedef std::stack<CoordinateSet> CoordinateSetStack;
class PlayerRobot: public Player {
public:
	PlayerRobot(Game *field, std::ostream *console, ProgramOptions opts, NullTimer *timer = NULL);
	virtual ~PlayerRobot() {};
	virtual Tick *tick();
	void setTimer(NullTimer *timer);
private:
	NullTimer *timer;
	Game *field;

	Tick *act_singleflagging(CoordinateSet tile, Tile *);
	Tick *act_safespots(CoordinateSet tile, Tile *);
	Tick *act_dualcheck(CoordinateSet tile, Tile *);
	Tick *act_safemap(CoordinateSet tile, Tile *);

	bool croaking;
	void ncroak(std::string msg);
	void croak(std::string msg);
	void croak(std::string msg, CoordinateSet a, CoordinateSet b);
	void croakend();
	void croakstatus();

	ProgramOptions opts;
	std::ostream *console;

	bool allowcoordreset;
	CoordinateSetList::const_iterator coord;
	std::stack<CoordinateSet> coords;
	CoordinateSet nextcoord();
};

class NoMoreCoordinatesException : public std::exception {
	virtual const char* what() const throw() {
		return "No more coordinates";
	};

	public:
		virtual ~NoMoreCoordinatesException () throw() {};
} static NoMoreCoordinates;
#endif
