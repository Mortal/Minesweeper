#ifndef MOVE_H
#define MOVE_H
#include <vector>
#include <string>
#include "types.h"
#include "Game.h"

class Move {
	public:
		virtual ~Move() {};
		virtual void act(Game *field) = 0;
};

class GiveUpMove : public Move {
	public:
		virtual ~GiveUpMove() {};
		virtual void act(Game *field) {};
};

class TileMove : public Move {
	public:
		virtual ~TileMove() {};
		CoordinateSet getPoint();
	protected:
		TileMove(CoordinateSet p);
	private:
		CoordinateSet point;
};

class FlagOnMove : public TileMove {
	public:
		FlagOnMove(CoordinateSet p);
		virtual ~FlagOnMove() {};
		virtual void act(Game *field);
};

class FlagOffMove : public TileMove {
	public:
		FlagOffMove(CoordinateSet p);
		virtual ~FlagOffMove() {};
		virtual void act(Game *field);
};

class PressMove : public TileMove {
	public:
		PressMove(CoordinateSet p);
		virtual ~PressMove() {};
		virtual void act(Game *field);
};

#endif
