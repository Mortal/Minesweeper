#ifndef SCREEN_H
#define SCREEN_H
#include <iostream>
#include <ncurses.h>
#include "Logger.h"

class Screen {
	public:
		Screen();
		~Screen();
		void setfieldsize(int width, int height);
		void puts(const char *s);
		WINDOW *getBombField();
		std::ostream *getConsole();
	private:
		WINDOW *screen;
		WINDOW *bombfield;
		WINDOW *console;
		WStream *consolestream;
		void delwins();
};

#endif
