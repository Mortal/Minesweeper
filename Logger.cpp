#include "Logger.h"

WStream::WStream(WINDOW *w) : std::ostream(new WStreamBuffer(w)) {
}

WStream::~WStream() {
	delete rdbuf();
}

WStreamBuffer::WStreamBuffer(WINDOW *w) : window(w) {
}
int WStreamBuffer::overflow(int ch) {
	waddch(window, (char) ch);
	return 1;
}
int WStreamBuffer::sync() {
	wrefresh(window);
	return 0;
}
