#include "ColourHandler.h"
#include <ostream>
#ifdef _WIN32
#include <windows.h>
#endif

ColourHandler::ColourHandler(std::ostream *out): out(out) {this->setcol(0);}
ColourHandler::~ColourHandler() {this->setcol(0);}

void ColourHandler::setcol(int colour) {
	if (this->colour == colour) return;
	if (!colour) {
#ifdef _WIN32
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
#else
		*this->out << "\033[0m"; // reset
#endif
	} else {
		this->output(colour);
	}
	this->colour = colour;
}

void ColourHandler::output(int colour) {
	bool bright = false;
	if (colour & 0x8) {
		bright = true;
		colour = colour & 0x7;
	}
#ifdef _WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colour);
#else
	*this->out << "\033[" << 30+colour << (bright ? ";1" : "") << "m";
#endif
}
