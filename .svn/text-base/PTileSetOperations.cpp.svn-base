#include "PTileSetOperations.h"
#include "types.h"

PTileSet intersecttilesets(PTileSet a, PTileSet b) {
	PTileSet result;
	PTileSetIt ait;
	for (ait = a.begin(); ait != a.end(); ++ait) {
		if (b.find(*ait) != b.end()) result.insert(*ait);
	}
	return result;
}
