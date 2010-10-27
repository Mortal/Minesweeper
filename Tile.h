#ifndef TILE_H
#define TILE_H

#include "types.h"
#include <ostream>
#include <ncurses.h>

class Tile {
public:
	Tile();
	void setFlag(Flag to);
	Flag getFlag();
	void setBlarg(bool blarg);
	bool amIDeadNow();
	chtype output();
	bool getDepressed();
	void press();
	void setSurroundings(unsigned int);
	unsigned int getSurroundings() {return this->aroundtheworld;};
	void addNeighbouringBomb();

private:
	unsigned int aroundtheworld;
	bool depressed;
	bool blarg;
	Flag flag;
	char aroundtheworldchar();
};

#endif
