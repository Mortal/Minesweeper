#ifndef MOVE_H
#define MOVE_H
#include <vector>
#include <string>
#include "types.h"
#include "Game.h"

class Move {
	public:
		virtual void act(Game *field) = 0;
};

class GiveUpMove : public Move {
	public:
		virtual void act(Game *field) {};
};

class TileMove : public Move {
	public:
		CoordinateSet getPoint();
	protected:
		TileMove(CoordinateSet p);
	private:
		CoordinateSet point;
};

class FlagOnMove : public TileMove {
	public:
		FlagOnMove(CoordinateSet p);
		virtual void act(Game *field);
};

class FlagOffMove : public TileMove {
	public:
		FlagOffMove(CoordinateSet p);
		virtual void act(Game *field);
};

class PressMove : public TileMove {
	public:
		PressMove(CoordinateSet p);
		virtual void act(Game *field);
};

#endif
