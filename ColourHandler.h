#ifndef COLOURHANDLER_H
#define COLOURHANDLER_H

#include <ostream>

class ColourHandler {
public:
	ColourHandler(std::ostream *out);
	~ColourHandler();
	void setcol(int colour);
private:
	std::ostream *out;
	int colour;
	void output(int colour);
};

#endif
