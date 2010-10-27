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
typedef unsigned int Coordinate;
typedef std::vector<Coordinate> CoordinateSet;
typedef CoordinateSet::iterator CoordinateSetIt;
typedef std::list<CoordinateSet> CoordinateSetList;
typedef CoordinateSetList::iterator CoordinateSetListIt;
typedef unsigned int Dimension;
typedef bool Flag;
#define FLAG_ON(f) (f)
#define FLAG_OFF(f) (!f)

struct ProgramOptions {
	bool verbose;
	bool ai;
	bool waitonquit;
	class NullTimer *timer;
	std::string fieldfile;
	bool fieldoutput;
};

#endif