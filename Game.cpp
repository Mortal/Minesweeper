#include "types.h"
#include "Game.h"
#include <cassert>
#include <list>
#include <iostream>
#include "Tile.h"
#include <algorithm> // sort
#include "Timer.h"
#include <ostream>
#include <ncurses.h>

Game::Game(Dimension dimensioncount, SizeVector dimensions, NullTimer *timer, ProgramOptions opts):
	timer(timer), dimensions(dimensions), dimensioncount(dimensioncount),
	allpositions_initialised(false), minecount(0), flagcount(0), pressedcount(0),
	state(GAMESTATE_INIT), window(NULL), opts(opts) {

	assert(dimensions.size() == dimensioncount);
	assert(dimensioncount > 0);
	this->inittiles(0);
}

void Game::startgame(int mines) {
	this->inittiles(mines);
}

void Game::setBombField(WINDOW *w) {
	window = w;
}

/********************************************************************
 * Getters
 */

GameState Game::getState() {
	return this->state;
}

unsigned int Game::totalMines() {
	return this->minecount;
}

unsigned int Game::totalFlags() {
	return this->flagcount;
}

CoordinateSet Game::origo() {
	CoordinateSet res;
	res.resize(this->dimensioncount, 0);
	return res;
}

Tile *Game::getTile(CoordinateSet pos) {
	unsigned int idx = this->coordstofieldindex(pos);
	assert(this->fieldindextocoords(idx) == pos);
	return this->tiles[idx];
}

/********************************************************************
 * Iterators
 */

CoordinateSetList::const_iterator Game::coordbegin() {
	if (!this->allpositions_initialised) {
		this->allpositionslist = this->allpositions();
		this->allpositions_initialised = true;
	}
	return this->allpositionslist.begin();
}

CoordinateSetList::const_iterator Game::coordend() {
	if (!this->allpositions_initialised) {
		this->allpositionslist = this->allpositions();
		this->allpositions_initialised = true;
	}
	return this->allpositionslist.end();
}

PTileVector::const_iterator Game::tilebegin() {
	return this->tiles.begin();
}

PTileVector::const_iterator Game::tileend() {
	return this->tiles.end();
}

/** coordbegin and coordend helpers */

CoordinateSetList Game::allpositions() {
	CoordinateSetList result;
	this->_allpositions(0, this->origo(), &result);
	return result;
}

void Game::_allpositions(Dimension dim, CoordinateSet basis, CoordinateSetList *list) {
	if (dim == this->dimensioncount) list->push_back(basis);
	else {
		for (unsigned int x = 0; x < this->dimensions[dim]; ++x) {
			CoordinateSet set = basis;
			set[dim] = x;
			this->_allpositions(dim+1, set, list);
		}
	}
}

/**
 * Populate game tiles with mines and blanks
 * @param mines Number of mines in the field to be created
 */
void Game::inittiles(int mines) {
	this->minecount = mines;
	this->tiles.resize(0);
	this->pressedcount = 0;
	this->flagcount = 0;

	/* figure out number of tiles */
	this->tilecount = 1;
	SizeVectorIt i;
	for (i = this->dimensions.begin(); i != this->dimensions.end(); i++) {
		this->tilecount *= *i;
	}

	/* create a tile list */
	this->tiles.resize(this->tilecount, NULL);

	/* replace NULLs in tiles with actual tiles */
	this->filltheblanks();

	/* add mines */
	if (mines) this->deploythemines(mines);

	switch (opts.reveal) {
		case REVEAL_NONE:
			break;
		case REVEAL_ONE:
			pressrandom();
			break;
		case REVEAL_ONEBLANK:
			if (!this->pressblanks(true)) {
				this->pressrandom();
			}
			break;
		case REVEAL_ALLBLANKS:
			if (!this->pressblanks(false)) {
				this->pressrandom();
			}
	}
	this->state = GAMESTATE_PLAY;
}

/********************************************************************
 * inittiles() helpers
 */

void Game::deploythemines(int mines) {
	std::vector<unsigned int> minelist;
	std::vector<unsigned int>::iterator minelistit, minelistit2;
	minelist.reserve(mines);
	while (mines-- > 0 && this->tilecount > minelist.size()) {
		unsigned int loc = ((unsigned int) rand()) % (this->tilecount-minelist.size());
		std::sort(minelist.begin(), minelist.end());
		for (minelistit = minelist.begin(); minelistit != minelist.end(); ++minelistit) {
			if (loc >= *minelistit) ++loc;
		}
		assert(loc < this->tilecount);
		if (opts.noborderbombs) {
			CoordinateSet c = fieldindextocoords(loc);
			SizeVectorIt i;
			CoordinateSetIt j;
			bool border = false;
			for (i = dimensions.begin(), j = c.begin(); i != dimensions.end() && j != c.end(); ++i, ++j) {
				if (*j == 0 || *j+1 == *i) {
					border = true;
					break;
				}
			}
			if (border) {
				++mines;
				continue;
			}
		}
		minelist.push_back(loc);
	}
	for (minelistit = minelist.begin(); minelistit != minelist.end(); ++minelistit) {
		this->tiles[*minelistit]->setBlarg(true);
		PTileSet neighbours = this->neighbourhood(this->fieldindextocoords(*minelistit));
		PTileSetIt i;
		for (i = neighbours.begin(); i != neighbours.end(); ++i) {
			assert(*i != NULL);
			(*i)->addNeighbouringBomb();
		}
	}
}

void Game::filltheblanks() {
	PTileVectorIt j;
	for (j = this->tiles.begin(); j != this->tiles.end(); ++j) {
		assert(*j == NULL);
		*j = new Tile();
	}
}

bool Game::pressblanks(bool one) {
	CoordinateSetSet blanks = findblanks();
	if (!blanks.size()) return false;
	CoordinateSetSetIt i;
	if (one) {
		unsigned int count = blanks.size();
		int idx = rand()%count;
		for (i = blanks.begin(); idx-- && i != blanks.end(); ++i);
		amIDeadNow(*i);
	} else {
		for (i = blanks.begin(); i != blanks.end(); ++i) {
			amIDeadNow(*i);
		}
	}
	return true;
}

CoordinateSetSet Game::findblanks() {
	CoordinateSetSet res;
	findblanks(0, origo(), &res);
	return res;
}

void Game::findblanks(Dimension dim, CoordinateSet basis, CoordinateSetSet *result) {
	if (dim >= this->dimensioncount) {
		Tile *tile = this->getTile(basis);
		if (tile->getSurroundings() == 0 && !tile->amIDeadNow()) {
			result->insert(basis);
		}
		return;
	}
	for (unsigned int x = 0; x < this->dimensions[dim]; ++x) {
		CoordinateSet temp = basis;
		temp[dim] = x;
		findblanks(dim+1, temp, result);
	}
}

void Game::pressrandom() {
	int range = this->getTileCount()-this->totalMines();
	if (range <= 0) return; // whoops
	int r = rand() % range;
	PTileVector::const_iterator i;
	unsigned int tileindex;
	for (i = this->tilebegin(), tileindex = 0; r && i != this->tileend(); ++i, ++tileindex) {
		if (!(*i)->amIDeadNow() && !--r) this->amIDeadNow(this->fieldindextocoords(tileindex));
	}
}

/********************************************************************
 * Coordinate conversion
 */

unsigned int Game::coordstofieldindex(CoordinateSet pos) {
	unsigned int idx = 0;
	SizeVectorIt i;
	CoordinateSetIt j;
	for (i = this->dimensions.begin(), j = pos.begin();
		i != this->dimensions.end() && j != pos.end();
		++i, ++j) {
		assert(*j < *i);
		idx *= *i;
		idx += *j;
	}
	return idx;
}

CoordinateSet Game::fieldindextocoords(unsigned int idx) {
	SizeVectorIt i, j;
	CoordinateSet result = this->origo();
	Dimension dim;
	unsigned int coord;
	for (i = this->dimensions.begin(), dim = 0; i != this->dimensions.end(); ++i, ++dim) {
		j = i;
		coord = idx;
		for (++j; j != this->dimensions.end(); ++j) {
			coord /= *j;
		}
		coord %= *i; // size of current dimension
		result[dim] = coord;
	}
	return result;
}

int Game::getOutputHeight() {
	CoordinateSet p = dimensions;
	for (Dimension d = 0; d < dimensioncount; ++d) {
		--p[d];
	}
	return getOutputRow(p)+1;
}

int Game::getOutputWidth() {
	CoordinateSet p = dimensions;
	for (Dimension d = 0; d < dimensioncount; ++d) {
		--p[d];
	}
	return getOutputColumn(p)+1;
}

int Game::getOutputColumn(CoordinateSet p) {
	int factor = 1;
	int sum = 0;
	Dimension d = dimensioncount-1;
	while (1) {
		sum += p[d]*factor;
		if (d < 2) break;
		factor *= dimensions[d];
		++factor;
		d -= 2;
	}
	return sum;
}

int Game::getOutputRow(CoordinateSet p) {
	int factor = 1;
	int sum = 0;
	Dimension d = dimensioncount-2;
	while (1) {
		sum += p[d]*factor;
		if (d < 2) break;
		factor *= dimensions[d];
		++factor;
		d -= 2;
	}
	return sum;
}

/**
 * Get the neighbourhood of a tile. Used by PlayerRobot.
 */
CoordinateSetList Game::neighbourhoodpositions(CoordinateSet pos, bool includeself /*= false*/) {
	CoordinateSetList result;
	this->_neighbourhoodpositions(0, pos, includeself, &result);
	return result;
}

void Game::_neighbourhoodpositions(Dimension dim,
                                   CoordinateSet basis,
                                   bool includebasis,
                                   CoordinateSetList *list) {
	if (dim == this->dimensioncount) {
		if (includebasis) list->push_back(basis);
	} else {
		CoordinateSet temp;
		if (basis[dim]) {
			temp = basis;
			--temp[dim];
			this->_neighbourhoodpositions(dim+1, temp, true, list);
		}
		this->_neighbourhoodpositions(dim+1, basis, includebasis, list);
		if (1+basis[dim] < this->dimensions[dim]) {
			temp = basis;
			++temp[dim];
			this->_neighbourhoodpositions(dim+1, temp, true, list);
		}
	}
}

PTileSet Game::neighbourhood(CoordinateSet pos, bool includeself /*= false*/) {
	PTileSet result;
	CoordinateSetList neighbours = this->neighbourhoodpositions(pos, includeself);
	CoordinateSetListIt i;
	for (i = neighbours.begin(); i != neighbours.end(); ++i) {
		Tile *tile = this->getTile(*i);
		if (tile != NULL) result.insert(tile);
	}
	return result;
}

/********************************************************************
 * Player interface. These methods are called by Moves.
 */

/**
 * Press a tile. Returns true if the pressed tile was a bomb. Oops!
 */
bool Game::amIDeadNow(CoordinateSet pos) {
	Tile *tile = this->getTile(pos);
	if (tile == NULL) return false;
	if (tile->getDepressed()) return false;
	this->press(pos);
	if (this->pressedcount == this->tiles.size()-this->minecount) {
		this->state = GAMESTATE_WIN;
	}
	if (tile->amIDeadNow()) {
		this->state = GAMESTATE_LOSE;
		return true;
	}
	return false;
}

/**
 * Called by amIDeadNow: Press a tile. If !norecursivespread and the tile is
 * not a bomb and doesn't have any bomb neighbours, press all tiles in its
 * neighbourhood.
 */
void Game::press(CoordinateSet pos, bool norecursivespread) {
	Tile *tile = this->getTile(pos);
	tile->press();
	++this->pressedcount;
	drawtile(pos);
	if (!tile->amIDeadNow() && (tile->getSurroundings() == 0)) {
		CoordinateSetList neighbours = this->neighbourhoodpositions(pos);
		CoordinateSetListIt i;
		for (i = neighbours.begin(); i != neighbours.end(); ++i) {
			if (norecursivespread && this->getTile(*i)->getSurroundings() == 0) continue;
			this->amIDeadNow(*i);
		}
	}
}

/**
 * Toggle flag on for a tile. Used to mark that the player believes the tile
 * contains a bomb.
 */
bool Game::flagon(CoordinateSet pos) {
	Tile *tile = this->getTile(pos);
	bool alreadyflag = FLAG_ON(tile->getFlag());
	tile->setFlag(true);
	if (!tile->amIDeadNow()) {
		std::cout << "Whoops, setting flag on a safe tile" << std::endl;
	}
	if (!alreadyflag) ++this->flagcount;
	drawtile(pos);
	return !alreadyflag;
}

/**
 * Toggle flag off for a tile.
 */
bool Game::flagoff(CoordinateSet pos) {
	Tile *tile = this->getTile(pos);
	bool alreadyflag = FLAG_ON(tile->getFlag());
	tile->setFlag(false);
	if (alreadyflag) --this->flagcount;
	drawtile(pos);
	return alreadyflag;
}

/********************************************************************
 * Drawing
 */

/**
 * Output the whole thing
 */
void Game::output() {
	TIMERON;
	immedok(window, false);
	werase(window);
	for (CoordinateSetList::const_iterator i = coordbegin(); i != coordend(); ++i) {
		drawtile(*i);
	}
	drawborders();
	immedok(window, true);
	wrefresh(window);
	TIMEROFF;
}

/**
 * Output just a single tile
 */
void Game::drawtile(CoordinateSet p) {
	Tile *t = getTile(p);
	chtype output = t->output();
	wmove(window, getOutputRow(p), getOutputColumn(p));
	waddch(window, output);
}

/**
 * Draw borders
 */
void Game::drawborders() {
	if (dimensioncount <= 2) return;
	int x = 0, y = 0, w = getOutputWidth(), h = getOutputHeight();
	drawhorzborders(&x, w, dimensioncount % 2);
	drawvertborders(&y, h, 1 - dimensioncount % 2);
}

void Game::drawhorzborders(int *y, int w, Dimension d) {
	if (d >= dimensioncount-2) {
		*y += dimensions[d];
	} else {
		for (Coordinate yy = 0; yy < dimensions[d]; ++yy) {
			if (yy) {
				for (Dimension dd = d; dd < dimensioncount-2; dd += 2) {
					mvwhline(window, (*y)++, 0, '-', w);
				}
			}
			drawhorzborders(y, w, d+2);
		}
	}
}

void Game::drawvertborders(int *x, int h, Dimension d) {
	if (d >= dimensioncount-2) {
		*x += dimensions[d];
	} else {
		for (Coordinate xx = 0; xx < dimensions[d]; ++xx) {
			if (xx) {
				for (Dimension dd = d; dd < dimensioncount-2; dd += 2) {
					mvwvline(window, 0, (*x)++, '|', h);
				}
			}
			drawvertborders(x, h, d+2);
		}
	}
}
