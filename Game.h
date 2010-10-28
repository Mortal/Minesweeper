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
	CoordinateSetList neighbourhoodpositions(CoordinateSet pos, bool includeself = false);
	PTileSet neighbourhood(CoordinateSet pos, bool includeself = false);
	CoordinateSetList::const_iterator coordbegin();
	CoordinateSetList::const_iterator coordend();
	PTileVector::const_iterator tilebegin();
	PTileVector::const_iterator tileend();
	Tile *getTile(CoordinateSet pos);
	void startgame(int mines);
	void output();
	bool amIDeadNow(CoordinateSet pos);
	bool flagon(CoordinateSet pos);
	bool flagoff(CoordinateSet pos);
	Dimension getDimensioncount() {return this->dimensioncount;}
	CoordinateSet origo();
	GameState getState();
	unsigned int totalMines();
	unsigned int totalFlags();
	unsigned int getPressedCount() {return this->pressedcount;}
	unsigned int getTileCount() {return this->tilecount;}
	void one_down();
	int getOutputWidth();
	int getOutputHeight();
	int getOutputColumn(CoordinateSet p);
	int getOutputRow(CoordinateSet p);
	void drawtile(CoordinateSet p);
	void setBombField(WINDOW *w);
private:
	NullTimer *timer;
	Dimension dimensioncount;
	SizeVector dimensions;
	PTileVector tiles;
	unsigned int tilecount;
	CoordinateSetList allpositionslist;
	bool allpositions_initialised;
	CoordinateSetList allpositions();
	void inittiles(int mines);
	unsigned int minecount;
	unsigned int flagcount;
	unsigned int pressedcount;
	unsigned int coordstofieldindex(CoordinateSet pos);
	CoordinateSet fieldindextocoords(unsigned int idx);
	int linecount;
	void deploythemines(int);
	void filltheblanks();
	bool pressblanks(Dimension dim, CoordinateSet basis);
	void pressrandom();
	void calculateneighbours(Dimension dim, CoordinateSet basis);
	void _neighbourhoodpositions(Dimension dim, CoordinateSet basis,
		bool includebasis, CoordinateSetList *list);
	void _allpositions(Dimension dim, CoordinateSet basis, CoordinateSetList *list);
	GameState state;
	void press(CoordinateSet pos, bool norecursivespread = false);
	WINDOW *window;
};

#endif
