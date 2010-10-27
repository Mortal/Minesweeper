#include "PlayerRobot.h"
#include "types.h"
#include "Game.h"
#include "MineFieldFilters.h"
#include <iostream>
#include "Tile.h"
#include <string>
#include <cassert>
#include "Timer.h"

PlayerRobot::PlayerRobot(Game *field, ProgramOptions opts, NullTimer *timer):
field(field), croaking(false), timer(timer), opts(opts) {
	if (!opts.fieldfile.empty()) {
		this->fieldfile.open(opts.fieldfile.c_str());
	}
}
PlayerRobot::~PlayerRobot() {
	if (!this->fieldfile.is_open()) this->fieldfile.close();
}

void PlayerRobot::ncroak(std::string msg) {
	if (!opts.verbose) return;
	if (!this->croaking) {
		std::cout << "BOT: ";
		this->croakstatus();
		this->croaking = true;
	}
	std::cout << msg;
}
void PlayerRobot::croak(std::string msg) {
	if (!opts.verbose) return;
	this->ncroak(msg);
	this->croakend();
}
void PlayerRobot::croakend() {
	if (!opts.verbose) return;
	if (this->croaking) {
		this->field->one_down(&std::cout);
		this->croaking = false;
	}
}
void PlayerRobot::croakstatus() {
	if (!opts.verbose) return;
	std::cout << this->field->totalFlags() << "/" << this->field->totalMines() << ", "
		<< (this->field->totalFlags() + this->field->getPressedCount()) << "/"
		<< this->field->getTileCount() << " ";
}

void PlayerRobot::play() {
	this->field->output(&std::cout);
	while (this->field->getState() == GAMESTATE_PLAY) {
		if (!this->tick()) {
			break;
		}
	}
	if (this->opts.fieldoutput) {
		std::cout << "-";
		this->field->one_down(&std::cout);
	}

	this->field->output(&std::cout);
	if (this->field->getState() == GAMESTATE_WIN) {
		this->croak("I am victorious.");
	} else if (this->field->getState() == GAMESTATE_LOSE) {
		this->croak("I have lost! How can this be?");
	} else {
		this->giveup();
	}
}

bool PlayerRobot::tick() {
	CoordinateSetList::const_iterator field = this->field->coordbegin();
	CoordinateSetStack checkfirst;
	bool returnsuccess = false;
	bool changed = false;
#define ACT(method) if (this->method(i, tile)) returnsuccess = success = changed = true;
	while (1) {
		bool success = false;
		CoordinateSet i;
		if (checkfirst.empty()) {
			if (field == this->field->coordend()) break;
			i = *field;
			++field;
		} else {
			i = checkfirst.top();
			checkfirst.pop();
		}
		if (changed) {
			if (this->fieldfile.is_open()) {
				this->fieldfile.seekp(0);
				this->field->output(&this->fieldfile);
			}
			if (this->opts.fieldoutput) {
				std::cout << "-";
				this->field->one_down(&std::cout);
				this->field->output(&std::cout);
			}
			changed = false;
		}
		Tile *tile = this->field->getTile(i);
		if (tile == NULL) continue;
		ACT(act_safemap);
		if (!tile->getDepressed() || !tile->getSurroundings()) continue;
		bool a = success;
		ACT(act_singleflagging);
		bool b = success;
		ACT(act_safespots);
		bool c = success;
		if (!returnsuccess && this->field->totalFlags() < this->field->totalMines()) {
			ACT(act_dualcheck);
		}
		if (success) {
			CoordinateSetList topush = this->field->neighbourhoodpositions(i);
			CoordinateSetList::reverse_iterator j;
			for (j = topush.rbegin(); j != topush.rend(); ++j) {
				checkfirst.push(*j);
			}
		}
	}
#undef ACT
	return returnsuccess;
}

void PlayerRobot::giveup() {
	this->croak("Alas, I am slain!");
}

bool PlayerRobot::act_singleflagging(CoordinateSet tile, Tile *ptile) {
	TIMERON;
	CoordinateSetList tiles = this->field->neighbourhoodpositions(tile);
	MineFieldFilterUnpressed f;
	tiles = f.filter(tiles, this->field);
	if (tiles.size() > ptile->getSurroundings()) {TIMERRET(false);}
	MineFieldFilterUnflagged unflagfilter;
	tiles = unflagfilter.filter(tiles, this->field);
	if (tiles.empty()) {TIMERRET(false);} // no unflagged tiles
	this->croak("This tile has as many surrounding tiles as it has neighbours");
	CoordinateSetListIt i;
	// tiles now only contains unflagged tiles
	for (i = tiles.begin(); i != tiles.end(); ++i) {
		this->field->flagon(*i);
	}
	TIMEROFF;
	return true;
}

bool PlayerRobot::act_safespots(CoordinateSet tile, Tile *ptile) {
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
		//std::cout << "Neighbouring unpressed tiles: " << tiles.size()
		//	<< "Surrounding bombs: " << ptile->getSurroundings()
		//	<< "Flagged neighbouring tiles: " << flagged << std::endl;
		TIMEROFF;
		return false;
	}
	this->croak("The unmarked territory around this tile is safe");
	for (i = tiles.begin(); i != tiles.end(); ++i) {
		Tile *neighbour = this->field->getTile(*i);
		if (neighbour != NULL && FLAG_OFF(neighbour->getFlag())) this->field->amIDeadNow(*i);
	}
	TIMEROFF;
	return true;
}

bool PlayerRobot::act_dualcheck(CoordinateSet a, Tile *pa) {
	/* For each tile A (CoordinateSet tile),
	 * check each bomb-neighbour neighbour B to see if
	 * the unpressed, unflagged neighbours of A are a perfect
	 * subset of the unflagged neighbours of B. In this case,
	 * if the face value of A and B are the same, then
	 * C = A_neighbours xor B_neighbours are all safe.
	 * If the face value of B is higher than A, and the number
	 * of elements in C is equal to the difference between the
	 * face values of B and A, then C are all mines. */
	TIMERON;
	CoordinateSetList tiles = this->field->neighbourhoodpositions(a);
	MineFieldFilterBombNeighbours bfilter;
	MineFieldFilterUnpressed bombfilter;
	MineFieldFilterLogicalDifference cfilter;
	MineFieldFilterLogicalDifference cfilterinv;
	if (this->timer != NULL) this->timer->starttime("MineFieldFilterBombNeighbours::filter");
	CoordinateSetList bs = bfilter.filter(tiles, this->field);
	if (this->timer != NULL) {
		this->timer->endtime("MineFieldFilterBombNeighbours::filter");
		this->timer->starttime("MineFieldFilterUnpressed::filter");
	}
	CoordinateSetList a_neighbours = bombfilter.filter(tiles, this->field);
	if (this->timer != NULL) this->timer->endtime("MineFieldFilterUnpressed::filter");
	cfilter.set(a_neighbours);
	CoordinateSetListIt biter;
	for (biter = bs.begin(); biter != bs.end(); ++biter) {
		CoordinateSet b = *biter;
		Tile *pb = this->field->getTile(b);
		assert(pb != NULL);
		if (pb->getSurroundings() < pa->getSurroundings()) continue;
		if (this->timer != NULL) this->timer->starttime("MineFieldFilterUnpressed::filter");
		CoordinateSetList b_neighbours = bombfilter.filter(this->field->neighbourhoodpositions(b), this->field);
		if (this->timer != NULL) this->timer->endtime("MineFieldFilterUnpressed::filter");
		cfilterinv.set(b_neighbours);
		if (this->timer != NULL) this->timer->starttime("MineFieldFilterLogicalDifference::filter");
		CoordinateSetList subsetcheck = cfilterinv.filter(a_neighbours, this->field);
		if (this->timer != NULL) this->timer->endtime("MineFieldFilterLogicalDifference::filter");
		if (subsetcheck.size()) continue; // a has neighbours that b doesn't have = not subset
		if (this->timer != NULL) this->timer->starttime("MineFieldFilterLogicalDifference::filter");
		CoordinateSetList c = cfilter.filter(b_neighbours, this->field);
		if (this->timer != NULL) this->timer->endtime("MineFieldFilterLogicalDifference::filter");
		if (!c.size()) continue;
		CoordinateSetListIt citer;
		unsigned int unflagged = 0;
		for (citer = c.begin(); citer != c.end(); ++citer) {
			Tile *pc = this->field->getTile(*citer);
			assert(pc != NULL);
			if (FLAG_OFF(pc->getFlag())) ++unflagged;
		}
		if (!unflagged) continue;
		if (pb->getSurroundings() == pa->getSurroundings()) {
			// face values equal, the set of c are all safe
			this->croak("Two neighbouring fields with same face values, these fields must be safe");
			for (citer = c.begin(); citer != c.end(); ++citer) {
				if (this->field->amIDeadNow(*citer)) {TIMERRET(true);}
			}
		} else if (pb->getSurroundings()-pa->getSurroundings() == c.size()) { // pb has larger face value than pa
			this->croak("Two neighbouring fields with differing face values, these unknown fields must all be mines");
			CoordinateSetListIt citer;
			for (citer = c.begin(); citer != c.end(); ++citer) {
				this->field->flagon(*citer);
			}
		}
	}
	TIMEROFF;
	return false;
}

bool PlayerRobot::act_safemap(CoordinateSet tile, Tile *ptile) {
	if (this->field->totalMines() > this->field->totalFlags()) return false;
	if (!ptile->getDepressed() && FLAG_OFF(ptile->getFlag())) {
		this->croak("We're done with the map, so surely there's no mine here.");
		this->field->amIDeadNow(tile);
		return true;
	}
	return false;
}
