#ifndef MINEFIELDFILTERS_H
#define MINEFIELDFILTERS_H

#include "types.h"

class MineFieldFilter {
public:
	virtual ~MineFieldFilter() {};
	virtual bool filter_cb(CoordinateSet, class Game *) = 0;
	CoordinateSetList filter(CoordinateSetList, class Game *);
};

class MineFieldFilterBombNeighbours: public MineFieldFilter {
public:
	virtual ~MineFieldFilterBombNeighbours() {};
	virtual bool filter_cb(CoordinateSet coords, class Game *field);
};

class MineFieldFilterUnpressed: public MineFieldFilter {
public:
	virtual ~MineFieldFilterUnpressed() {};
	virtual bool filter_cb(CoordinateSet coords, class Game *field);
};


class MineFieldFilterUnflagged: public MineFieldFilter {
public:
	virtual ~MineFieldFilterUnflagged() {};
	virtual bool filter_cb(CoordinateSet coords, class Game *field);
};

class MineFieldFilterLogicalAnd: public MineFieldFilter {
public:
	virtual ~MineFieldFilterLogicalAnd() {};
	void set(CoordinateSetList);
	virtual bool filter_cb(CoordinateSet coords, class Game *field);
private:
	CoordinateSetList list;
};

class MineFieldFilterLogicalDifference: public MineFieldFilter {
	// The coordinates (A) passed to the filter() are returned if they're not in
	// the list (B), so the operation here is A \ B
public:
	virtual ~MineFieldFilterLogicalDifference() {};
	void set(CoordinateSetList);
	virtual bool filter_cb(CoordinateSet coords, class Game *field);
private:
	CoordinateSetList list;
};

#endif
