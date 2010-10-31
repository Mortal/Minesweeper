#ifndef MINEFIELDFILTERS_H
#define MINEFIELDFILTERS_H

#include "types.h"
#include "Vector.hpp"
#include "Game.h"

template<unsigned L>
class MineFieldFilter {
public:
	virtual ~MineFieldFilter() {};
	virtual bool filter_cb(CoordinateSetLList, class Game<L> *) = 0;
	CoordinateSetLList filter(CoordinateSetLList, class Game<L> *);
};

template<unsigned L>
class MineFieldFilterBombNeighbours: public MineFieldFilter<L> {
public:
	virtual ~MineFieldFilterBombNeighbours() {};
	virtual bool filter_cb(CoordinateSetL coords, class Game<L> *field);
};

template<unsigned L>
class MineFieldFilterUnpressed: public MineFieldFilter<L> {
public:
	virtual ~MineFieldFilterUnpressed() {};
	virtual bool filter_cb(CoordinateSetL coords, class Game<L> *field);
};

template<unsigned L>
class MineFieldFilterUnflagged: public MineFieldFilter<L> {
public:
	virtual ~MineFieldFilterUnflagged() {};
	virtual bool filter_cb(CoordinateSetL coords, class Game<L> *field);
};

template<unsigned L>
class MineFieldFilterLogicalAnd: public MineFieldFilter<L> {
public:
	virtual ~MineFieldFilterLogicalAnd() {};
	void set(CoordinateSetLList);
	virtual bool filter_cb(CoordinateSetLList coords, class Game<L> *field);
private:
	CoordinateSetLList list;
};

template<unsigned L>
class MineFieldFilterLogicalDifference: public MineFieldFilter<L> {
	// The coordinates (A) passed to the filter() are returned if they're not in
	// the list (B), so the operation here is A \ B
public:
	virtual ~MineFieldFilterLogicalDifference() {};
	void set(CoordinateSetLList);
	virtual bool filter_cb(CoordinateSetL coords, class Game<L> *field);
private:
	CoordinateSetLList list;
};

#endif
