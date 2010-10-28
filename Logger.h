#ifndef LOGGER_H
#define LOGGER_H
#include <iostream>
#include <ncurses.h>

class WStream : public std::ostream {
	public:
		WStream(WINDOW *w);
		~WStream();
		void setwin(WINDOW *w);
};

class WStreamBuffer : public std::streambuf {
	public:
		WStreamBuffer(WINDOW *w);
		virtual int overflow(int ch);
		virtual int sync();
		void setwin(WINDOW *w);
	private:
		WINDOW *window;
};

#endif
