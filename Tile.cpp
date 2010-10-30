#include "types.h"
#include "Tile.h"
#include <iostream>
#include <ostream>
#include <ncurses.h>

Tile::Tile(): aroundtheworld(0), depressed(false), blarg(false), flag(false) {
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

chtype Tile::output() {
	if (this->flag) {
		return '/' | COLOR_PAIR(1);
	} else if (this->depressed) {
		if (this->blarg) {
			return '@' | COLOR_PAIR(2);
		} else {
			return this->aroundtheworldchar() | COLOR_PAIR(3);
		}
	} else {
		return '.' | COLOR_PAIR(2);
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
