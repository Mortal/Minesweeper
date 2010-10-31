#include "Game.h"
#include "PlayerHuman.h"
#include <string>
#include <list>
#include <sstream>
#include <iostream>

std::list<std::string> splitstring(std::string haystack, std::string needle) {
	std::list<std::string> result;
	if (needle.empty() || haystack.empty()) return result;
	std::string::size_type off = 0, lastoff = 0;
	while (1) {
		off = haystack.find(needle, off);
		if (off < 0 || off > haystack.length()-needle.length()) break;
		if (haystack.substr(off, needle.length()) == needle) {
			result.push_back(haystack.substr(lastoff, off-lastoff));
			lastoff = off+needle.length();
		}
		++off;
	}
	result.push_back(haystack.substr(lastoff, haystack.length()-lastoff));
	return result;
}

void trimstring(std::string *s) {
	std::string::size_type i = s->length();
	while (i && ((*s)[i-1] == '\r' || (*s)[i-1] == '\n')) --i;
	s->resize(i);
}

template<unsigned L>
HumanCoordinateInput<L>::HumanCoordinateInput(std::string input, Dimension dimensioncount) {
	trimstring(&input);
	std::list<std::string> coords = splitstring(input, ",");
	this->_valid = false;
	this->_coords = CoordinateSet<L>(dimensioncount, 0);
	if (coords.size() != dimensioncount) return;
	CoordinateSet<L> ret;
	std::list<std::string>::iterator i;
	int j;
	for (i = coords.begin(), j = 0; i != coords.end(); ++i, ++j) {
		int curcoord;
		std::istringstream in(*i, std::istringstream::in);
		in >> curcoord;
		++curcoord;
		ret[j] = curcoord;
	}
	this->_valid = true;
	this->_coords = ret;
}

template<unsigned L>
PlayerHuman<L>::PlayerHuman(Game<L> *field, std::ostream *console) :
field(field), console(console) {
}

template<unsigned L>
Tick<L> *PlayerHuman<L>::tick() {
	this->field->output();
	std::string inputstring;
	std::cin >> inputstring;
	HumanCoordinateInput<L> *input;
	while (1) {
		input = new HumanCoordinateInput<L>(inputstring, field->getDimensioncount());
		if (input->valid()) break;
		delete input;
	}
	Tick<L> *ret = new Tick<L>("Press tile");
	ret->addMove(new PressMove<L>(input->get()));
	return ret;
}

template<unsigned L>
void PlayerHuman<L>::dead() {
	this->field->output();
	std::cout << "A strange game. The only winning move is not to play. "
		"Do you want your possessions identified? [ynq]" << std::endl;
	std::cin.ignore(0xFFFF, '\n');
}
