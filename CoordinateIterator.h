#ifndef COORDINATEITERATOR_H
#define COORDINATEITERATOR_H
#include "types.h"
#include <vector>

typedef std::vector<unsigned int> IndexList;

class CoordinateIterator {
	public:
		CoordinateIterator(CoordinateSet init, SizeVector dims, IndexList indexes);
		CoordinateIterator operator++(); //pre
		CoordinateIterator operator++(int); //post
		CoordinateIterator operator--(); //pre
		CoordinateIterator operator--(int); //post
	private:
		CoordinateSet val;
		SizeVector dims;
		IndexList indexes;
}

#endif
