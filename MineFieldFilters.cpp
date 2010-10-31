#include "MineFieldFilters.h"
#include "Tile.h"
#include "Game.h"
#include "types.h"
template<unsigned L>
CoordinateSetLList MineFieldFilter<L>::filter(CoordinateSetLList list, Game<L> *field) {
	CoordinateSetLList::iterator i;
	CoordinateSetLList result;
	for (i = list.begin(); i != list.end(); ++i) {
		if (this->filter_cb(*i, field)) result.push_back(*i);
	}
	return result;
}

/* A field is a "bomb neighbor" if it has a positive number on its face. */
template<unsigned L>
bool MineFieldFilterBombNeighbours<L>::filter_cb(CoordinateSetL coords, Game<L> *field) {
	Tile *tile = field->getTile(coords);
	if (tile == NULL) return false;
	/* The tile must be pressed, not be a mine, and have a positive "surroundings" */
	return (tile->getDepressed() && !tile->amIDeadNow() && tile->getSurroundings() > 0);
}

template<unsigned L>
bool MineFieldFilterUnpressed<L>::filter_cb(CoordinateSetL coords, Game<L> *field) {
	Tile *tile = field->getTile(coords);
	if (tile == NULL) return false;
	return !(tile->getDepressed());
}

template<unsigned L>
bool MineFieldFilterUnflagged<L>::filter_cb(CoordinateSetL coords, Game<L> *field) {
	Tile *tile = field->getTile(coords);
	if (tile == NULL) return false;
	return FLAG_OFF(tile->getFlag());
}

template<unsigned L>
void MineFieldFilterLogicalAnd<L>::set(CoordinateSetLList list) {
	this->list = list;
}

template<unsigned L>
bool MineFieldFilterLogicalAnd<L>::filter_cb(CoordinateSetLList coords, Game<L> *field) {
	CoordinateSetLList::iterator i;
	for (i = this->list.begin(); i != this->list.end(); ++i) {
		if (*i == coords) return true;
	}
	return false;
}

template<unsigned L>
void MineFieldFilterLogicalDifference<L>::set(CoordinateSetLList list) {
	this->list = list;
}

template<unsigned L>
bool MineFieldFilterLogicalDifference<L>::filter_cb(CoordinateSetL coords, Game<L> *field) {
	CoordinateSetLList::iterator i;
	for (i = this->list.begin(); i != this->list.end(); ++i) {
		if (*i == coords) return false;
	}
	return true;
}

