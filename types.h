#ifndef TYPES_H
#define TYPES_H

#include <list>
#include <vector>
#include <set>
#include <string>

typedef std::vector<class Tile*> PTileVector;
typedef PTileVector::iterator PTileVectorIt;
typedef std::vector<unsigned int> SizeVector;
typedef SizeVector::iterator SizeVectorIt;
typedef std::set<class Tile*> PTileSet;
typedef PTileSet::iterator PTileSetIt;
typedef unsigned short Coordinate;
typedef std::vector<Coordinate> CoordinateSet;
typedef CoordinateSet::iterator CoordinateSetIt;
typedef std::list<CoordinateSet> CoordinateSetList;
typedef CoordinateSetList::iterator CoordinateSetListIt;
typedef std::set<CoordinateSet> CoordinateSetSet;
typedef CoordinateSetSet::iterator CoordinateSetSetIt;
typedef std::list<class Move*> MoveList;
typedef MoveList::iterator MoveListIt;
typedef unsigned short Dimension;
typedef bool Flag;
#define FLAG_ON(f) (f)
#define FLAG_OFF(f) (!f)

enum Reveal {
	REVEAL_NONE,
	REVEAL_ONE,
	REVEAL_ONEBLANK,
	REVEAL_ALLBLANKS
};

struct ProgramOptions {
	bool verbose;
	bool repeat;
	bool ai;
	bool waitonquit;
	bool fullscreen;
	class NullTimer *timer;
	Reveal reveal;
	bool noborderbombs;
	bool revealborders;
};

#endif
