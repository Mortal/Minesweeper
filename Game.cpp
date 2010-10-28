#include "types.h"
#include "Game.h"
#include <cassert>
#include <ctime> // for seeding
#include <list>
#include <iostream>
#include "Tile.h"
#include <algorithm> // sort
#include "Timer.h"
#include <ostream>
#include "ColourHandler.h"
#include <ncurses.h>

Game::Game(Dimension dimensioncount, SizeVector dimensions, NullTimer *timer):
timer(timer), dimensioncount(dimensioncount), dimensions(dimensions), allpositions_initialised(false), minecount(0),
flagcount(0), pressedcount(0), linecount(0), state(GAMESTATE_INIT), window(NULL) {
	assert(dimensions.size() == dimensioncount);
	assert(dimensioncount > 0);
	this->inittiles(0);
	srand(time(NULL) & 0xFFFFFFFF);
}

GameState Game::getState() {return this->state;}
unsigned int Game::totalMines() {return this->minecount;}
unsigned int Game::totalFlags() {return this->flagcount;}

void Game::inittiles(int mines) {
	this->minecount = mines;
	this->tiles.resize(0);
	this->pressedcount = 0;
	this->flagcount = 0;
	this->tilecount = 1;
	SizeVectorIt i;
	for (i = this->dimensions.begin(); i != this->dimensions.end(); i++) {
		this->tilecount *= *i;
	}
	this->tiles.resize(this->tilecount, NULL);
	this->filltheblanks();
	if (mines) this->deploythemines(mines);
	//this->calculateneighbours(0, this->origo());
	if (!this->pressblanks(0, this->origo())) this->pressrandom();
	this->state = GAMESTATE_PLAY;
}

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

CoordinateSet Game::origo() {
	CoordinateSet res;
	res.resize(this->dimensioncount, 0);
	return res;
}

void Game::filltheblanks() {
	PTileVectorIt j;
	for (j = this->tiles.begin(); j != this->tiles.end(); ++j) {
		assert(*j == NULL);
		*j = new Tile();
	}
}

void Game::calculateneighbours(Dimension dimension, CoordinateSet basis) {
	if (dimension >= this->dimensioncount) {
		Tile *tile = this->getTile(basis);
		if (tile == NULL) return;
		int count = 0;
		PTileSet neighbourhood = this->neighbourhood(basis);
		PTileSetIt i;
		for (i = neighbourhood.begin(); i != neighbourhood.end(); ++i) {
			if (*i != NULL && (*i)->amIDeadNow()) count++;
		}
		tile->setSurroundings(count);
		return;
	}
	for (Coordinate x = 0; x < this->dimensions[dimension]; ++x) {
		CoordinateSet tile = basis;
		tile[dimension] = x;
		this->calculateneighbours(dimension+1, tile);
	}
}

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

Tile *Game::getTile(CoordinateSet pos) {
	unsigned int idx = this->coordstofieldindex(pos);
	assert(this->fieldindextocoords(idx) == pos);
	return this->tiles[idx];
}

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

void Game::startgame(int mines) {
	this->inittiles(mines);
}

void Game::one_down() {
	wprintw(window, "\n");
}

void Game::output() {
	TIMERON;
	werase(window);
	for (CoordinateSetList::const_iterator i = coordbegin(); i != coordend(); ++i) {
		drawtile(*i);
	}
	TIMEROFF;
}

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
	drawtile(pos);
	return false;
}
void Game::press(CoordinateSet pos, bool norecursivespread) {
	Tile *tile = this->getTile(pos);
	tile->press();
	++this->pressedcount;
	if (!tile->amIDeadNow() && (tile->getSurroundings() == 0)) {
		CoordinateSetList neighbours = this->neighbourhoodpositions(pos);
		CoordinateSetListIt i;
		for (i = neighbours.begin(); i != neighbours.end(); ++i) {
			if (norecursivespread && this->getTile(*i)->getSurroundings() == 0) continue;
			this->amIDeadNow(*i);
		}
	}
}
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
bool Game::flagoff(CoordinateSet pos) {
	Tile *tile = this->getTile(pos);
	bool alreadyflag = FLAG_ON(tile->getFlag());
	tile->setFlag(false);
	if (alreadyflag) --this->flagcount;
	drawtile(pos);
	return alreadyflag;
}
bool Game::pressblanks(Dimension dim, CoordinateSet basis) {
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
		CoordinateSet temp = basis;
		temp[dim] = x;
		if (this->pressblanks(dim+1, temp)) result = true;
	}
	return result;
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
int Game::getOutputHeight() {
	int len = 1;
	if (dimensioncount > 2) {
		Dimension d = dimensioncount-2;
		while (1) {
			len *= dimensions[d];
			if (d < 3) break;
			d -= 2;
		}
	}
	len = len * dimensions[dimensioncount-2] + len - 1;
	return len;
}
int Game::getOutputWidth() {
	int len = 1;
	if (dimensioncount > 3) {
		Dimension d = dimensioncount-3;
		while (1) {
			len *= dimensions[d];
			if (d < 2) break;
			d -= 2;
		}
	}
	len = len * dimensions[dimensioncount-1] + len - 1;
	return len;
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
void Game::setBombField(WINDOW *w) {
	window = w;
}
void Game::drawtile(CoordinateSet p) {
	Tile *t = getTile(p);
	chtype output = t->output();
	wmove(window, getOutputRow(p), getOutputColumn(p));
	wechochar(window, output);
}
