#ifndef TILE_H
#define TILE_H

#include "types.h"
#include <ostream>

class Tile {
public:
	Tile();
	void setFlag(Flag to);
	Flag getFlag();
	void setBlarg(bool blarg);
	bool amIDeadNow();
	void output(std::ostream *fp, class ColourHandler *col);
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
