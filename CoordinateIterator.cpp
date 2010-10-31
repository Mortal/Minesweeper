#include "CoordinateIterator.h"

CoordinateIterator::CoordinateIterator(CoordinateSet init, SizeVector dims, IndexList indexes) :
val(init), dims(dims), indexes(indexes) {
}

CoordinateIterator CoordinateIterator::operator++() { //pre
	unsigned int chindex;
	Coordinate coord;
	bool got = false;
	for (IndexList::iterator i = indexes.begin(); i != indexes.end(); ++i) {
		unsigned int idx = *i;
		if (val[idx] >= dims[idx]-1) continue;
		chindex = idx;
		coord = val[idx]+1
		got = true;
		break;
	}
	if (!got) {
		CoordinateSet newval;
		newval.resize(dims.size(), 0);
		val = newval;
	} else {
		CoordinateSet newval = val;
		newval[chindex] = coord;
		val = newval;
	}
	return this;
}

CoordinateIterator CoordinateIterator::operator--() { //pre
	unsigned int chindex;
	Coordinate coord;
	bool got = false;
	for (IndexList::iterator i = indexes.begin(); i != indexes.end(); ++i) {
		unsigned int idx = *i;
		if (val[idx] >= dims[idx]-1) continue;
		chindex = idx;
		coord = val[idx]+1
		got = true;
		break;
	}
	if (!got) {
		CoordinateSet newval;
		newval.resize(dims.size(), 0);
		val = newval;
	} else {
		CoordinateSet newval = val;
		newval[chindex] = coord;
		val = newval;
	}
	return this;
}

