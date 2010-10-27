#include "types.h"
#include "Tile.h"
#include <iostream>
#include <ostream>
#include "ColourHandler.h"

Tile::Tile(): blarg(false), flag(false), depressed(false), aroundtheworld(0) {
}

void Tile::setFlag(Flag to) {
	this->flag = to;
}

Flag Tile::getFlag() {
	return this->flag;
}

bool Tile::amIDeadNow() {
	return this->blarg;
}

void Tile::press() {
	this->depressed = true;
}

bool Tile::getDepressed() {
	return this->depressed;
}

void Tile::output(std::ostream *fp, ColourHandler *col) {
	if (this->flag) {
		col->setcol(9);
		*fp << '/';
	} else if (this->depressed) {
		if (this->blarg) {
			col->setcol(9);
			*fp << '@';
		} else {
			col->setcol(0);
			*fp << this->aroundtheworldchar();
		}
	} else {
		col->setcol(6);
		*fp << '.';
	}
}

void Tile::setSurroundings(unsigned int i) {
	this->aroundtheworld = i;
}

void Tile::addNeighbouringBomb() {
	++this->aroundtheworld;
}

void Tile::setBlarg(bool blarg) {
	this->blarg = blarg;
}

char Tile::aroundtheworldchar() {
	if (!this->aroundtheworld) return ' ';
	if (this->aroundtheworld < 10) return '0' + this->aroundtheworld;
	if (this->aroundtheworld < 36) return 'A' + (this->aroundtheworld-10);
	return '?'; // aaaa
}
