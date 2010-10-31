#ifndef MOVE_H
#define MOVE_H
#include <vector>
#include <string>
#include "Game.h"
#include "types.h"
#include "Vector.hpp"

template<unsigned L>
class Move {
	public:
		virtual ~Move<L>() {};
		virtual void act(Game<L> *field) = 0;
};

template<unsigned L>
class GiveUpMove : public Move<L> {
	public:
		virtual ~GiveUpMove<L>() {};
		virtual void act(Game<L> *field) {};
};

template<unsigned L>
class TileMove : public Move<L> {
	public:
		virtual ~TileMove<L>() {};
		CoordinateSetL getPoint();
	protected:
		TileMove(CoordinateSetL p);
	private:
		CoordinateSetL point;
};

template<unsigned L>
class FlagOnMove : public TileMove<L> {
	public:
		FlagOnMove(CoordinateSetL p);
		virtual ~FlagOnMove<L>() {};
		virtual void act(Game<L> *field);
};

template<unsigned L>
class FlagOffMove : public TileMove<L> {
	public:
		FlagOffMove(CoordinateSetL p);
		virtual ~FlagOffMove<L>() {};
		virtual void act(Game<L> *field);
};

template<unsigned L>
class PressMove : public TileMove<L> {
	public:
		PressMove(CoordinateSetL p);
		virtual ~PressMove<L>() {};
		virtual void act(Game<L> *field);
};

#endif
