#include "PlayerRobot.h"
#include "types.h"
#include "Game.h"
#include "MineFieldFilters.h"
#include <iostream>
#include "Tile.h"
#include <string>
#include <cassert>
#include "Timer.h"
#include <ncurses.h>
#include "Tick.h"

PlayerRobot::PlayerRobot(Game *field, std::ostream *console, ProgramOptions opts, NullTimer *timer):
timer(timer), field(field), croaking(false), opts(opts), console(console), allowcoordreset(false),
coord(field->coordbegin()) {
}

void PlayerRobot::ncroak(std::string msg) {
	if (!opts.verbose) return;
	if (!this->croaking) {
		*console << "BOT: ";
		this->croakstatus();
		this->croaking = true;
	}
	*console << msg;
}
void PlayerRobot::croak(std::string msg) {
	if (!opts.verbose) return;
	this->ncroak(msg);
	this->croakend();
}
void PlayerRobot::croak(std::string msg, CoordinateSet a, CoordinateSet b) {
	if (!opts.verbose) return;
	this->ncroak(msg);
	*console << ' ';
	{
		CoordinateSetIt i = a.begin();
		while (true) {
			*console << *i;
			++i;
			if (i == a.end()) {break;}
			*console << ',';
		}
	}
	*console << ' ';
	{
		CoordinateSetIt i = b.begin();
		while (true) {
			*console << *i;
			++i;
			if (i == b.end()) {break;}
			*console << ',';
		}
	}
	this->croakend();
}
void PlayerRobot::croakend() {
	if (!opts.verbose) return;
	if (this->croaking) {
		*console << std::endl;
		this->croaking = false;
	}
}
void PlayerRobot::croakstatus() {
	if (!opts.verbose) return;
	*console << this->field->totalFlags() << "/" << this->field->totalMines() << ", "
		<< (this->field->totalFlags() + this->field->getPressedCount()) << "/"
		<< this->field->getTileCount() << " ";
}

CoordinateSet PlayerRobot::nextcoord() {
	if (coord == this->field->coordend() && allowcoordreset) {
		coord = this->field->coordbegin();
		allowcoordreset = false;
		return *coord;
	}
	if (coord != this->field->coordend()) {
		return *coord++;
	}
	throw NoMoreCoordinates;
}

Tick *PlayerRobot::tick() {
#define ACT(method) {Tick *ret = this->method(coord, tile); if (ret != NULL) {allowcoordreset = true; return ret;}}
	while (1) {
		CoordinateSet coord;
		Tile *tile;
		try {
			coord = nextcoord();
			tile = this->field->getTile(coord);
		} catch (NoMoreCoordinatesException& e) {
			break;
		}

		if (tile == NULL) continue;

		ACT(act_safemap);

		if (!tile->getDepressed() || !tile->getSurroundings()) continue;

		ACT(act_singleflagging);

		ACT(act_safespots);

		ACT(act_dualcheck);
	}
#undef ACT
	Tick *ret = new Tick("No more tiles");
	ret->addMove(new GiveUpMove());
	return ret;
}

Tick *PlayerRobot::act_singleflagging(CoordinateSet tile, Tile *ptile) {
	TIMERON;
	CoordinateSetList tiles = this->field->neighbourhoodpositions(tile);
	MineFieldFilterUnpressed f;
	tiles = f.filter(tiles, this->field);
	if (tiles.size() > ptile->getSurroundings()) {TIMERRET(NULL);}
	MineFieldFilterUnflagged unflagfilter;
	tiles = unflagfilter.filter(tiles, this->field);
	if (tiles.empty()) {TIMERRET(NULL);} // no unflagged tiles
	Tick *ret = new Tick("This tile has as many surrounding tiles as it has neighbours");
	CoordinateSetListIt i;
	// tiles now only contains unflagged tiles
	for (i = tiles.begin(); i != tiles.end(); ++i) {
		ret->addMove(new FlagOnMove(*i));
	}
	TIMEROFF;
	return ret;
}

Tick *PlayerRobot::act_safespots(CoordinateSet tile, Tile *ptile) {
	TIMERON;
	CoordinateSetList tiles = this->field->neighbourhoodpositions(tile);
	MineFieldFilterUnpressed unpressfilter;
	tiles = unpressfilter.filter(tiles, this->field);
	unsigned int flagged = 0;
	CoordinateSetListIt i;
	// tiles contains only unpressed tiles
	for (i = tiles.begin(); i != tiles.end(); ++i) {
		Tile *neighbour = this->field->getTile(*i);
		if (neighbour != NULL && FLAG_ON(neighbour->getFlag())) ++flagged;
	}
	if (ptile->getSurroundings() != flagged || tiles.size() <= flagged) {
		//*console << "Neighbouring unpressed tiles: " << tiles.size()
		//	<< "Surrounding bombs: " << ptile->getSurroundings()
		//	<< "Flagged neighbouring tiles: " << flagged << std::endl;
		TIMEROFF;
		return NULL;
	}
	Tick *ret = new Tick("The unmarked territory around this tile is safe");
	for (i = tiles.begin(); i != tiles.end(); ++i) {
		Tile *neighbour = this->field->getTile(*i);
		if (neighbour != NULL && FLAG_OFF(neighbour->getFlag())) ret->addMove(new PressMove(*i));
	}
	TIMEROFF;
	return ret;
}

Tick *PlayerRobot::act_dualcheck(CoordinateSet a, Tile *pa) {
	/* A is the primary tile that has some known number of bombs as neighbors.
	 *
	 * For each tile B that is a neighbor of A and also has some known number of
	 * bombs as neighbors that is at least as large as the number of A, find the
	 * tiles that are unpressed neighbors of B but not of A. We call this list C.
	 *
	 * If A and B has the same number of bomb neighbors, then necessarily all the
	 * tiles in C are safe to be pressed.
	 *
	 * If B has exactly as many more bomb neighbors than A as the number of tiles
	 * in C, then necessarily all these tiles must be bombs.
	 */
	TIMERON;
	CoordinateSetList tiles = this->field->neighbourhoodpositions(a);
	MineFieldFilterBombNeighbours bfilter;
	MineFieldFilterUnpressed bombfilter;
	MineFieldFilterLogicalDifference cfilter;
	MineFieldFilterLogicalDifference cfilterinv;
	TIMERONS("MineFieldFilterBombNeighbours::filter");
	/* find the "bomb neighbors" of the current tile */
	CoordinateSetList bs = bfilter.filter(tiles, this->field);
	TIMEROFFS("MineFieldFilterBombNeighbours::filter");

	TIMERONS("MineFieldFilterUnpressed::filter");
	/* find the unpressed neighbors of the current tile */
	CoordinateSetList a_neighbours = bombfilter.filter(tiles, this->field);
	TIMEROFFS("MineFieldFilterUnpressed::filter");

	/* this filter will remove the unpressed neighbors of the current tile */
	cfilter.set(a_neighbours);

	CoordinateSetListIt biter;
	for (biter = bs.begin(); biter != bs.end(); ++biter) {
		CoordinateSet b = *biter;
		Tile *pb = this->field->getTile(b);
		assert(pb != NULL);

		if (pb->getSurroundings() < pa->getSurroundings()) continue;
		/* this tile B has as many or more neighbors than the current tile A */

		TIMERONS("MineFieldFilterUnpressed::filter");
		/* find the unpressed neighbors of this tile */
		CoordinateSetList b_neighbours = bombfilter.filter(this->field->neighbourhoodpositions(b), this->field);
		TIMEROFFS("MineFieldFilterUnpressed::filter");

		cfilterinv.set(b_neighbours);

		TIMERONS("MineFieldFilterLogicalDifference::filter");
		/* find the unpressed neighbors of A that are not unpressed neighbors of B */
		CoordinateSetList subsetcheck = cfilterinv.filter(a_neighbours, this->field);
		TIMEROFFS("MineFieldFilterLogicalDifference::filter");

		if (subsetcheck.size()) continue; // a has neighbours that b doesn't have, so it is not a subset

		TIMERONS("MineFieldFilterLogicalDifference::filter");
		/* find the unpressed neighbors of B that are not unpressed neighbors of A */
		CoordinateSetList c = cfilter.filter(b_neighbours, this->field);
		TIMEROFFS("MineFieldFilterLogicalDifference::filter");

		if (!c.size()) continue;
		// B has unpressed neighbors that A doesn't have

		CoordinateSetListIt citer;
		unsigned int unflagged = 0;
		for (citer = c.begin(); citer != c.end(); ++citer) {
			Tile *pc = this->field->getTile(*citer);
			assert(pc != NULL);
			if (FLAG_OFF(pc->getFlag())) ++unflagged;
		}
		if (!unflagged) continue;
		// some of these unpressed neighbors are unflagged

		if (pb->getSurroundings() == pa->getSurroundings()) {
			// face values equal, the set of c are all safe
			Tick *ret = new Tick("Two neighbouring fields with same face values, these fields must be safe");
			for (citer = c.begin(); citer != c.end(); ++citer) {
				ret->addMove(new PressMove(*citer));
			}
			return ret;
		} else if (pb->getSurroundings()-pa->getSurroundings() == c.size()) { // pb has larger face value than pa
			Tick *ret = new Tick("Two neighbouring fields with differing face values, these unknown fields must all be mines");
			CoordinateSetListIt citer;
			for (citer = c.begin(); citer != c.end(); ++citer) {
				ret->addMove(new FlagOnMove(*citer));
			}
			return ret;
		}
	}
	TIMEROFF;
	return NULL;
}

Tick *PlayerRobot::act_safemap(CoordinateSet tile, Tile *ptile) {
	if (this->field->totalMines() > this->field->totalFlags()) return NULL;
	if (!ptile->getDepressed() && FLAG_OFF(ptile->getFlag())) {
		Tick *ret = new Tick("We're done with the map, so surely there's no mine here.");
		ret->addMove(new PressMove(tile));
		return ret;
	}
	return NULL;
}
