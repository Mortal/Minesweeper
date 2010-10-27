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

HumanCoordinateInput::HumanCoordinateInput(std::string input, Dimension dimensioncount) {
	trimstring(&input);
	std::list<std::string> coords = splitstring(input, ",");
	this->_valid = false;
	this->_coords = CoordinateSet(dimensioncount, 0);
	if (coords.size() != dimensioncount) return;
	CoordinateSet ret;
	ret.reserve(dimensioncount);
	std::list<std::string>::iterator i;
	for (i = coords.begin(); i != coords.end(); ++i) {
		int curcoord;
		std::istringstream in(*i, std::istringstream::in);
		in >> curcoord;
		++curcoord;
		ret.push_back(curcoord);
	}
	this->_valid = true;
	this->_coords = ret;
}

PlayerHuman::PlayerHuman(Game *field) {
	this->field = field;
}

void PlayerHuman::play() {
	while (1) {
		this->field->output();
		std::string inputstring;
		std::cin >> inputstring;
		HumanCoordinateInput input(inputstring, field->getDimensioncount());
		if (!input.valid()) continue;
		if (this->field->amIDeadNow(input.get())) {
			this->dead();
			break;
		}
	}
}

void PlayerHuman::dead() {
	this->field->output();
	std::cout << "A strange game. The only winning move is not to play. "
		"Do you want your possessions identified? [ynq]" << std::endl;
	std::cin.ignore(0xFFFF, '\n');
}
