#include "Logger.h"

WStream::WStream(WINDOW *w) : std::ostream(new WStreamBuffer(w)) {
}

WStream::~WStream() {
	delete rdbuf();
}

void WStream::setwin(WINDOW *w) {
	(dynamic_cast<WStreamBuffer*>(rdbuf()))->setwin(w);
}

WStreamBuffer::WStreamBuffer(WINDOW *w) : window(w) {
}

int WStreamBuffer::overflow(int ch) {
	if (window != NULL) {
		waddch(window, (char) ch);
	} else {
		strbuf << (char) ch;
	}
	return 1;
}

int WStreamBuffer::sync() {
	wrefresh(window);
	return 0;
}

void WStreamBuffer::setwin(WINDOW *w) {
	window = w;
	const char *s = strbuf.str().c_str();
	wprintw(window, s);
	wrefresh(window);
	strbuf.str("");
}
