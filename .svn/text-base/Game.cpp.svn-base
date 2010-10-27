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

Game::Game(int dimensioncount, SizeVector dimensions, NullTimer *timer):
dimensioncount(dimensioncount), dimensions(dimensions), state(GAMESTATE_INIT), linecount(0),
minecount(0), flagcount(0), pressedcount(0), allpositions_initialised(false), timer(timer) {
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

void Game::back_up(std::ostream *fp) {
	if (!this->linecount) return;
#if defined(_WIN32) && !defined(__CYGWIN__)
	COORD pos;
	CONSOLE_SCREEN_BUFFER_INFO CSBI;
	HANDLE consoleoutput = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(consoleoutput, &CSBI);
	pos.Y = CSBI.dwCursorPosition.Y-this->linecount;
	pos.X = 0;
	SetConsoleCursorPosition(consoleoutput, pos);
	this->linecount = 0;
#else
	while (this->linecount > 0) {
		--this->linecount;
		*fp << "\033[A";
	}
#endif
}

void Game::one_down(std::ostream *fp) {
	++this->linecount;
	*fp << std::endl;
}

void Game::output(std::ostream *fp) {
	TIMERON;
	CoordinateSet basis = this->origo();
	this->back_up(fp);
	ColourHandler col(fp);
	this->outputdimensions(fp, this->dimensioncount % 2, basis, &col);
	TIMEROFF;
}

void Game::outputrow(std::ostream *fp, Dimension dim, CoordinateSet basis, ColourHandler *col) {
	// we're only outputting rows on the last, third to last, etc. dimensions
	// if we have an even number of dimensions, then the dimensions numbered
	// {..., count-3, count-1} will all be odd
	// and vice versa
	// thus if dimensioncount is odd then the dimension we're outputting must be even
	if (dim == 0 && (this->dimensioncount % 2) == 0) ++dim;
	assert(this->dimensioncount % 2 != dim % 2);
	if (dim == this->dimensioncount-1) { // row characters
		for (Coordinate i = 0; i < this->dimensions[dim]; i++) {
			CoordinateSet tile = basis;
			tile[dim] = i;
			this->getTile(tile)->output(fp, col);
		}
		return;
	}
	for (Coordinate i = 0; i < this->dimensions[dim]; ++i) {
		if (i > 0) {
			fp->put(' ');
		}
		CoordinateSet subbasis = basis;
		subbasis[dim] = i;
		this->outputrow(fp, dim+2, subbasis, col);
	}
}

void Game::outputdimensions(std::ostream *fp, Dimension dim, CoordinateSet basis, ColourHandler *col) {
	if (this->dimensioncount % 2 != dim % 2) {
		this->outputrow(fp, dim, basis, col);
		return;
	}
	if (dim == this->dimensioncount-2) { // 2-dimensional field
		/*if (dim) {
			for (int i = 0; i < dim; i++) {
				if (i) *fp << ',';
				*fp << basis[i];
			}
			*fp << ",y,x" << std::endl;
		}*/
		for (Coordinate y = 0; y < this->dimensions[dim]; y++) {
			CoordinateSet rowbasis = basis;
			rowbasis[dim] = y;
			this->outputrow(fp, 0, rowbasis, col);
			this->one_down(fp);
		}
	} else {
		std::string blank;
		// dn = this->dimensions[this->dimensioncount-1]:
		// for 1 or 2 dimensions, len2 = d1
		// for 3 or 4 dimensions,
		// len4 = d3*d1   each row output for each outer dimension
		//      + d3-1    a space is output for each outer dimension except the first
		// for 5 or 6 dimensions,
		// len6 = d5*len4 each subiteration is output for each outer dimension
		//      + d5-1    plus a space for each except the first iteration
		//      = d5*d3*d1 + d5*d3-d5 + d5-1 = d5*d3*d1 + d5*d3 - 1
		// for 7 or 8 dimensions,
		// len8 = d7*len6 yada
		//      + d7-1    yada
		//      = d7*d5*d3*d1 + d7*d5*d3 - 1
		
		// so first we calculate the common part in those two terms (d3*d5*d7*...):
		int len = 1;
		for (Dimension d = this->dimensioncount-3; d >= 0; d -= 2) {
			len *= this->dimensions[d];
		}
		// then we substitute it into the formula (d3*d5*d7*...)*d1 + (d3*d5*d7*...) - 1
		len = len*this->dimensions[this->dimensioncount-1]+len-1;
		blank.resize(len, ' '); // these should all be spaces
		for (Coordinate i = 0; i < this->dimensions[dim]; i++) {
			CoordinateSet newbasis = basis;
			newbasis[dim] = i;
			this->outputdimensions(fp, dim+2, newbasis, col);
			*fp << blank;
			this->one_down(fp);
		}
	}
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
	return !alreadyflag;
}
bool Game::flagoff(CoordinateSet pos) {
	Tile *tile = this->getTile(pos);
	bool alreadyflag = FLAG_ON(tile->getFlag());
	tile->setFlag(false);
	if (alreadyflag) --this->flagcount;
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
