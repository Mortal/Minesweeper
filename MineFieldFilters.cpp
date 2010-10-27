#include "MineFieldFilters.h"
#include "Tile.h"
#include "Game.h"
#include "types.h"
CoordinateSetList MineFieldFilter::filter(CoordinateSetList list, Game *field) {
	CoordinateSetListIt i;
	CoordinateSetList result;
	for (i = list.begin(); i != list.end(); ++i) {
		if (this->filter_cb(*i, field)) result.push_back(*i);
	}
	return result;
}

/* A field is a "bomb neighbor" if it has a positive number on its face. */
bool MineFieldFilterBombNeighbours::filter_cb(CoordinateSet coords, Game *field) {
	Tile *tile = field->getTile(coords);
	if (tile == NULL) return false;
	/* The tile must be pressed, not be a mine, and have a positive "surroundings" */
	return (tile->getDepressed() && !tile->amIDeadNow() && tile->getSurroundings() > 0);
}

bool MineFieldFilterUnpressed::filter_cb(CoordinateSet coords, Game *field) {
	Tile *tile = field->getTile(coords);
	if (tile == NULL) return false;
	return !(tile->getDepressed());
}
bool MineFieldFilterUnflagged::filter_cb(CoordinateSet coords, Game *field) {
	Tile *tile = field->getTile(coords);
	if (tile == NULL) return false;
	return FLAG_OFF(tile->getFlag());
}
void MineFieldFilterLogicalAnd::set(CoordinateSetList list) {
	this->list = list;
}
bool MineFieldFilterLogicalAnd::filter_cb(CoordinateSet coords, Game *field) {
	CoordinateSetListIt i;
	for (i = this->list.begin(); i != this->list.end(); ++i) {
		if (*i == coords) return true;
	}
	return false;
}
void MineFieldFilterLogicalDifference::set(CoordinateSetList list) {
	this->list = list;
}
bool MineFieldFilterLogicalDifference::filter_cb(CoordinateSet coords, Game *field) {
	CoordinateSetListIt i;
	for (i = this->list.begin(); i != this->list.end(); ++i) {
		if (*i == coords) return false;
	}
	return true;
}

