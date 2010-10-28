#ifndef MINEFIELD_H
#define MINEFIELD_H

#include "types.h"
#include "Tile.h"
#include <ostream>
#include "ColourHandler.h"
#include <ncurses.h>

enum GameState {
	GAMESTATE_INIT,
	GAMESTATE_PLAY,
	GAMESTATE_WIN,
	GAMESTATE_LOSE
};

class Game {
public:
	Game(Dimension dimensioncount, SizeVector dimensions, class NullTimer *timer);
	void startgame(int mines);
	void setBombField(WINDOW *w);

	// Getters
	GameState getState();
	unsigned int totalMines();
	unsigned int totalFlags();
	CoordinateSet origo();
	Tile *getTile(CoordinateSet pos);
	Dimension getDimensioncount() {return this->dimensioncount;}
	unsigned int getPressedCount() {return this->pressedcount;}
	unsigned int getTileCount() {return this->tilecount;}

	// Iterators
	CoordinateSetList::const_iterator coordbegin();
	CoordinateSetList::const_iterator coordend();
	PTileVector::const_iterator tilebegin();
	PTileVector::const_iterator tileend();

	// Coordinate conversion
	int getOutputWidth();
	int getOutputHeight();
	int getOutputColumn(CoordinateSet p);
	int getOutputRow(CoordinateSet p);

	// used by PlayerRobot
	CoordinateSetList neighbourhoodpositions(CoordinateSet pos, bool includeself = false);
	PTileSet neighbourhood(CoordinateSet pos, bool includeself = false);

	// Player interface (called by subclasses of Move)
	bool amIDeadNow(CoordinateSet pos);
	bool flagon(CoordinateSet pos);
	bool flagoff(CoordinateSet pos);

	// Drawing
	void output();
	void drawtile(CoordinateSet p);

private:
	NullTimer *timer;

	SizeVector dimensions;
	Dimension dimensioncount;

	PTileVector tiles;
	unsigned int tilecount;

	CoordinateSetList allpositionslist;
	bool allpositions_initialised;

	unsigned int minecount;
	unsigned int flagcount;
	unsigned int pressedcount;

	GameState state;
	WINDOW *window;

	// coordbegin and coordend helpers
	CoordinateSetList allpositions();
	void _allpositions(Dimension dim, CoordinateSet basis, CoordinateSetList *list);

	// tile initialisation
	void inittiles(int mines);
	void deploythemines(int);
	void filltheblanks();
	bool pressblanks(Dimension dim, CoordinateSet basis);
	void pressrandom();

	// Coordinate conversion
	unsigned int coordstofieldindex(CoordinateSet pos);
	CoordinateSet fieldindextocoords(unsigned int idx);

	// used by PlayerRobot
	void _neighbourhoodpositions(Dimension dim, CoordinateSet basis,
		bool includebasis, CoordinateSetList *list);

	// amIDeadNow helper
	void press(CoordinateSet pos, bool norecursivespread = false);
};

#endif
