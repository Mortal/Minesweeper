#include "types.h"
#include "Vector.hpp"
#include "Game.h"
#include <cassert>
#include <list>
#include <iostream>
#include "Tile.h"
#include <algorithm> // sort
#include "Timer.h"
#include <ostream>
#include <ncurses.h>

template<unsigned L>
Game<L>::Game(typename SizeVector<L>::Type dimensions, NullTimer *timer):
	timer(timer), dimensions(dimensions), dimensioncount(L),
	allpositions_initialised(false), minecount(0), flagcount(0), pressedcount(0),
	state(GAMESTATE_INIT), window(NULL) {

	this->inittiles(0);
}

template<unsigned L>
void Game<L>::startgame(int mines) {
	this->inittiles(mines);
}

template<unsigned L>
void Game<L>::setBombField(WINDOW *w) {
	window = w;
}

/********************************************************************
 * Getters
 */

template<unsigned L>
GameState Game<L>::getState() {
	return this->state;
}

template<unsigned L>
unsigned int Game<L>::totalMines() {
	return this->minecount;
}

template<unsigned L>
unsigned int Game<L>::totalFlags() {
	return this->flagcount;
}

template<unsigned L>
typename CoordinateSet<L>::Type Game<L>::origo() {
	typename CoordinateSet<L>::Type res;
	return res;
}

template<unsigned L>
Tile *Game<L>::getTile(typename CoordinateSet<L>::Type pos) {
	unsigned int idx = this->coordstofieldindex(pos);
	assert(this->fieldindextocoords(idx) == pos);
	return this->tiles[idx];
}

/********************************************************************
 * Iterators
 */

template<unsigned L>
typename CoordinateSet<L>::List::const_iterator Game<L>::coordbegin() {
	if (!this->allpositions_initialised) {
		this->allpositionslist = this->allpositions();
		this->allpositions_initialised = true;
	}
	return this->allpositionslist.begin();
}

template<unsigned L>
typename CoordinateSet<L>::List::const_iterator Game<L>::coordend() {
	if (!this->allpositions_initialised) {
		this->allpositionslist = this->allpositions();
		this->allpositions_initialised = true;
	}
	return this->allpositionslist.end();
}

template<unsigned L>
PTileVector::const_iterator Game<L>::tilebegin() {
	return this->tiles.begin();
}

template<unsigned L>
PTileVector::const_iterator Game<L>::tileend() {
	return this->tiles.end();
}

/** coordbegin and coordend helpers */

template<unsigned L>
typename CoordinateSet<L>::List Game<L>::allpositions() {
	typename CoordinateSet<L>::List result;
	this->_allpositions(0, this->origo(), &result);
	return result;
}

template<unsigned L>
void Game<L>::_allpositions(Dimension dim, typename CoordinateSet<L>::Type basis, typename CoordinateSet<L>::List *list) {
	if (dim == this->dimensioncount) list->push_back(basis);
	else {
		for (unsigned int x = 0; x < this->dimensions[dim]; ++x) {
			typename CoordinateSet<L>::Type set = basis;
			set[dim] = x;
			this->_allpositions(dim+1, set, list);
		}
	}
}

/**
 * Populate game tiles with mines and blanks
 * @param mines Number of mines in the field to be created
 */
template<unsigned L>
void Game<L>::inittiles(int mines) {
	this->minecount = mines;
	this->tiles.resize(0);
	this->pressedcount = 0;
	this->flagcount = 0;

	/* figure out number of tiles */
	this->tilecount = 1;
	for (int i = 0; i < L; ++i) {
		this->tilecount *= dimensions[i];
	}

	/* create a tile list */
	this->tiles.resize(this->tilecount, NULL);

	/* replace NULLs in tiles with actual tiles */
	this->filltheblanks();

	/* add mines */
	if (mines) this->deploythemines(mines);

	/* press all the tiles that don't have bomb neighbours */
	if (!this->pressblanks(0, this->origo())) {
		/* if there are no such bombs, press a random non-bomb tile */
		this->pressrandom();
	}

	this->state = GAMESTATE_PLAY;
}

/********************************************************************
 * inittiles() helpers
 */

template<unsigned L>
void Game<L>::deploythemines(int mines) {
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

template<unsigned L>
void Game<L>::filltheblanks() {
	PTileVectorIt j;
	for (j = this->tiles.begin(); j != this->tiles.end(); ++j) {
		assert(*j == NULL);
		*j = new Tile();
	}
}

template<unsigned L>
bool Game<L>::pressblanks(Dimension dim, typename CoordinateSet<L>::Type basis) {
	if (dim >= this->dimensioncount) {
		Tile *tile = this->getTile(basis);
		if (tile->getSurroundings() == 0 && !tile->amIDeadNow()) {
			this->press(basis, true);
			return true;
		}
		return false;
	}
	bool result = false;
	for (unsigned int x = 0; x < this->dimensions[dim]; ++x) {
		typename CoordinateSet<L>::Type temp = basis;
		temp[dim] = x;
		if (this->pressblanks(dim+1, temp)) result = true;
	}
	return result;
}

template<unsigned L>
void Game<L>::pressrandom() {
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

template<unsigned L>
unsigned int Game<L>::coordstofieldindex(typename CoordinateSet<L>::Type pos) {
	unsigned int idx = 0;
	for (int i = 0; i < L; ++i) {
		assert(pos[i] < dimensions[i]);
		idx *= dimensions[i];
		idx += pos[i];
	}
	return idx;
}

template<unsigned L>
typename CoordinateSet<L>::Type Game<L>::fieldindextocoords(unsigned int idx) {
	typename CoordinateSet<L>::Type result;
	Dimension dim;
	for (Dimension dim = 0; dim < L; ++dim) {
		Coordinate coord = idx;
		for (int j = dim+1; j < L; ++j) {
			coord /= dimensions[j];
		}
		coord %= dimensions[dim]; // size of current dimension
		result[dim] = coord;
	}
	return result;
}

template<unsigned L>
int Game<L>::getOutputHeight() {
	typename CoordinateSet<L>::Type p = dimensions;
	for (Dimension d = 0; d < dimensioncount; ++d) {
		--p[d];
	}
	return getOutputRow(p)+1;
}

template<unsigned L>
int Game<L>::getOutputWidth() {
	typename CoordinateSet<L>::Type p = dimensions;
	for (Dimension d = 0; d < dimensioncount; ++d) {
		--p[d];
	}
	return getOutputColumn(p)+1;
}

template<unsigned L>
int Game<L>::getOutputColumn(typename CoordinateSet<L>::Type p) {
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

template<unsigned L>
int Game<L>::getOutputRow(typename CoordinateSet<L>::Type p) {
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
template<unsigned L>
typename CoordinateSet<L>::List Game<L>::neighbourhoodpositions(typename CoordinateSet<L>::Type pos, bool includeself /*= false*/) {
	typename CoordinateSet<L>::List result;
	this->_neighbourhoodpositions(0, pos, includeself, &result);
	return result;
}

template<unsigned L>
void Game<L>::_neighbourhoodpositions(Dimension dim,
                                   typename CoordinateSet<L>::Type basis,
                                   bool includebasis,
                                   typename CoordinateSet<L>::List *list) {
	if (dim == this->dimensioncount) {
		if (includebasis) list->push_back(basis);
	} else {
		typename CoordinateSet<L>::Type temp;
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

template<unsigned L>
PTileSet Game<L>::neighbourhood(typename CoordinateSet<L>::Type pos, bool includeself /*= false*/) {
	PTileSet result;
	typename CoordinateSet<L>::List neighbours = this->neighbourhoodpositions(pos, includeself);
	typename CoordinateSet<L>::ListIt i;
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
template<unsigned L>
bool Game<L>::amIDeadNow(typename CoordinateSet<L>::Type pos) {
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
template<unsigned L>
void Game<L>::press(typename CoordinateSet<L>::Type pos, bool norecursivespread) {
	Tile *tile = this->getTile(pos);
	tile->press();
	++this->pressedcount;
	drawtile(pos);
	if (!tile->amIDeadNow() && (tile->getSurroundings() == 0)) {
		typename CoordinateSet<L>::List neighbours = this->neighbourhoodpositions(pos);
		typename CoordinateSet<L>::ListIt i;
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
template<unsigned L>
bool Game<L>::flagon(typename CoordinateSet<L>::Type pos) {
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
template<unsigned L>
bool Game<L>::flagoff(typename CoordinateSet<L>::Type pos) {
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
template<unsigned L>
void Game<L>::output() {
	TIMERON;
	immedok(window, false);
	werase(window);
	for (typename CoordinateSet<L>::List::const_iterator i = coordbegin(); i != coordend(); ++i) {
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
template<unsigned L>
void Game<L>::drawtile(typename CoordinateSet<L>::Type p) {
	Tile *t = getTile(p);
	chtype output = t->output();
	wmove(window, getOutputRow(p), getOutputColumn(p));
	waddch(window, output);
}

/**
 * Draw borders
 */
template<unsigned L>
void Game<L>::drawborders() {
	if (dimensioncount <= 2) return;
	int x = 0, y = 0, w = getOutputWidth(), h = getOutputHeight();
	drawhorzborders(&x, w, dimensioncount % 2);
	drawvertborders(&y, h, 1 - dimensioncount % 2);
}

template<unsigned L>
void Game<L>::drawhorzborders(int *y, int w, Dimension d) {
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

template<unsigned L>
void Game<L>::drawvertborders(int *x, int h, Dimension d) {
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
