#ifndef LOGGER_H
#define LOGGER_H
#include <iostream>
#include <ncurses.h>

class WStream : public std::ostream {
	public:
		WStream(WINDOW *w);
		~WStream();
};

class WStreamBuffer : public std::streambuf {
	public:
		WStreamBuffer(WINDOW *w);
		virtual int overflow(int ch);
		virtual int sync();
	private:
		WINDOW *window;
};

#endif
