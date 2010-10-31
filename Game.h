#ifndef MINEFIELD_H
#define MINEFIELD_H

#include "types.h"
#include "Vector.hpp"
#include "Tile.h"
#include <ostream>
#include <ncurses.h>

enum GameState {
	GAMESTATE_INIT,
	GAMESTATE_PLAY,
	GAMESTATE_WIN,
	GAMESTATE_LOSE
};

template<unsigned L>
class Game {
public:
	Game(typename SizeVector<L>::Type dimensions, class NullTimer *timer);
	void startgame(int mines);
	void setBombField(WINDOW *w);

	// Getters
	GameState getState();
	unsigned int totalMines();
	unsigned int totalFlags();
	typename CoordinateSet<L>::Type origo();
	Tile *getTile(typename CoordinateSet<L>::Type pos);
	Dimension getDimensioncount() {return this->dimensioncount;}
	unsigned int getPressedCount() {return this->pressedcount;}
	unsigned int getTileCount() {return this->tilecount;}

	// Iterators
	typename CoordinateSet<L>::List::const_iterator coordbegin();
	typename CoordinateSet<L>::List::const_iterator coordend();
	PTileVector::const_iterator tilebegin();
	PTileVector::const_iterator tileend();

	// Coordinate conversion
	int getOutputWidth();
	int getOutputHeight();
	int getOutputColumn(typename CoordinateSet<L>::Type p);
	int getOutputRow(typename CoordinateSet<L>::Type p);

	// used by PlayerRobot
	typename CoordinateSet<L>::List neighbourhoodpositions(typename CoordinateSet<L>::Type pos, bool includeself = false);
	PTileSet neighbourhood(typename CoordinateSet<L>::Type pos, bool includeself = false);

	// Player interface (called by subclasses of Move)
	bool amIDeadNow(typename CoordinateSet<L>::Type pos);
	bool flagon(typename CoordinateSet<L>::Type pos);
	bool flagoff(typename CoordinateSet<L>::Type pos);

	// Drawing
	void output();
	void drawtile(typename CoordinateSet<L>::Type p);

private:
	NullTimer *timer;

	typename SizeVector<L>::Type dimensions;
	Dimension dimensioncount;

	PTileVector tiles;
	unsigned int tilecount;

	typename CoordinateSet<L>::List allpositionslist;
	bool allpositions_initialised;

	unsigned int minecount;
	unsigned int flagcount;
	unsigned int pressedcount;

	GameState state;
	WINDOW *window;

	// coordbegin and coordend helpers
	typename CoordinateSet<L>::List allpositions();
	void _allpositions(Dimension dim, typename CoordinateSet<L>::Type basis, typename CoordinateSet<L>::List *list);

	// tile initialisation
	void inittiles(int mines);
	void deploythemines(int);
	void filltheblanks();
	bool pressblanks(Dimension dim, typename CoordinateSet<L>::Type basis);
	void pressrandom();

	// Coordinate conversion
	unsigned int coordstofieldindex(typename CoordinateSet<L>::Type pos);
	typename CoordinateSet<L>::Type fieldindextocoords(unsigned int idx);

	// used by PlayerRobot
	void _neighbourhoodpositions(Dimension dim, typename CoordinateSet<L>::Type basis,
		bool includebasis, typename CoordinateSet<L>::List *list);

	// amIDeadNow helper
	void press(typename CoordinateSet<L>::Type pos, bool norecursivespread = false);

	// Drawing
	void drawborders();
	void drawhorzborders(int *y, int w, Dimension d);
	void drawvertborders(int *x, int h, Dimension d);
};

#endif
