#ifndef COORDINATESET_H
#define COORDINATESET_H

class CoordinateSets {
	public:
		static setSettings(Dimension size, Coordinate *coords) {
			if (coords != NULL) {
				delete coords;
			}
			unsigned int fields = 1;
			for (Dimension i = 0; i < size; ++i) {
				fields *= coords[i];
			}
			this.coords = new Coordinate[fields][size];
			Coordinate *coord = new Coordinate[size];
		}

		static Coordinate **coords;
};

class CoordinateSet {
	public:
		CoordinateSet(int index);
		CoordinateSetIt begin() {return CoordinateSetIt(this, 0);};
};

class CoordinateSetIt {
	public:
		CoordinateSetIt(CoordinateSet *coord, Dimension dim);
		CoordinateSetIt(CoordinateSetIt c): coord(c.getCoord()), dim(c.getDim()) {};
		CoordinateSet *getCoord() {return coord;};
		Dimension *getDim() {return dim;};
		CoordinateSetIt operator++() {++dim; return *this;};
		CoordinateSetIt operator++(int) {++dim; return *this;};
		CoordinateSetIt operator--() {--dim; return *this;};
		CoordinateSetIt operator--(int) {--dim; return *this;};
		Coordinate operator*() {return coord[dim];};
		bool operator==(CoordinateSetIt other) {return dim == other.getDim() && coord = other.getCoord();};
	private:
		CoordinateSet *coord;
		Dimension dim;
};

#endif
