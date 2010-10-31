#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <stdarg.h>
#include "types.h"

template <class T, unsigned L> class Vector {
	public:
		Vector(T first, ...);
		Vector(T coords[L]);

		T operator [](int idx);

		unsigned size();
	private:
		T coordinates[L];
};

template<class T, unsigned L> Vector<T, L>::Vector(T first, ...) {
	va_list ap;
	coordinates[0] = first;
	va_start(ap, first);
	for (int i = 1; i <= L; ++i) {
		coordinates[i] = va_arg(ap, T);
	}
	va_end(ap);
}

template<class T, unsigned L> Vector<T, L>::Vector(T coords[L]) {
	for (int i = 0; i < L; ++i) {
		coordinates[i] = coords[i];
	}
}

template<class T, unsigned L> T Vector<T, L>::operator [](int idx) {
	return coordinates[idx];
}

template<class T, unsigned L> unsigned Vector<T, L>::size() {
	return L;
}

typedef unsigned int Coordinate;

template<unsigned L>
struct CoordinateSet {
	typedef Vector<Coordinate, L> Type;
	typedef std::list<Type> List;
};

template<unsigned L>
struct SizeVector {
	typedef Vector<unsigned int, L> Type;
};

#define CoordinateSetL typename CoordinateSet<L>::Type
#define CoordinateSetLList typename CoordinateSet<L>::List
#endif
