#ifndef TYPES_H
#define TYPES_H

#include <list>
#include <vector>
#include <set>
#include <string>

typedef std::vector<class Tile*> PTileVector;
typedef PTileVector::iterator PTileVectorIt;
typedef std::set<class Tile*> PTileSet;
typedef PTileSet::iterator PTileSetIt;
#define MoveLList std::list<Move<L>*>
typedef unsigned int Dimension;
typedef bool Flag;
#define FLAG_ON(f) (f)
#define FLAG_OFF(f) (!f)

struct ProgramOptions {
	bool verbose;
	bool repeat;
	bool ai;
	bool waitonquit;
	bool fullscreen;
	class NullTimer *timer;
};

#endif
