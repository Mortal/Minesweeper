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
	PlayerRobot(Game *field, ProgramOptions opts, NullTimer *timer = NULL);
	~PlayerRobot();
	virtual void play();
	void setTimer(NullTimer *timer);
private:
	std::ofstream fieldfile;
	NullTimer *timer;
	Game *field;
	bool tick();
	void giveup();
	bool act_singleflagging(CoordinateSet tile, Tile *);
	bool act_safespots(CoordinateSet tile, Tile *);
	bool act_dualcheck(CoordinateSet tile, Tile *);
	bool act_safemap(CoordinateSet tile, Tile *);
	bool croaking;
	void ncroak(std::string msg);
	void croak(std::string msg);
	void croakend();
	void croakstatus();
	ProgramOptions opts;
};
#endif
