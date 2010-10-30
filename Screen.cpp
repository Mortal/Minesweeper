#include "Screen.h"
#include <iostream>

Screen::Screen() : screen(stdscr), bombfield(NULL), console(NULL), consolestream(new WStream(NULL)) {
}

Screen::~Screen() {
}

void Screen::delwins() {
	if (bombfield != NULL) {
		delwin(bombfield);
		bombfield = NULL;
	}
	if (console != NULL) {
		delwin(console);
		console = NULL;
	}
}

void Screen::setfieldsize(int width, int height) {
	/* get screen size */
	int ww, wh;
	getmaxyx(stdscr, wh, ww);

	delwins();
	/* allocate bombfield of given size */
	bombfield = newwin(height, width, 0, 0);

	int cw = ww-width;
	if (cw < 0) cw = 0;
	int ch = wh-height;
	if (ch < 0) ch = 0;
	if (ch*10 > cw) {
		console = newwin(0, 0, height, 0);
	} else {
		console = newwin(0, 0, 0, width);
	}
	scrollok(console, TRUE);
	/* pass the new WINDOW to the console stream */
	consolestream->setwin(console);
}

/* todo: operator overloading instead of this */
void Screen::puts(const char *s) {
	*consolestream << s;
}

WINDOW *Screen::getBombField() {
	return bombfield;
}

std::ostream *Screen::getConsole() {
	return consolestream;
}
