#ifdef TESTING
#include "test_declare.h"
#include "t_PTileSetOperations.h"
#include "types.h"
#include "PTileSetOperations.h"
#include "test_define.h"
DEFINETEST(t_intersecttilesets) {
	PTileSet empty, result;
	result = intersecttilesets(empty, empty);
	if (result.size() != 0) return false;
	PTileSet primes;
	primes.insert((Tile*) 2);
	primes.insert((Tile*) 3);
	primes.insert((Tile*) 5);
	primes.insert((Tile*) 7);
	PTileSet even;
	even.insert((Tile*) 2);
	even.insert((Tile*) 4);
	even.insert((Tile*) 6);
	even.insert((Tile*) 8);
	even.insert((Tile*) 10);
	result = intersecttilesets(primes, empty);
	if (result.size() != 0) return false;
	result = intersecttilesets(empty, primes);
	if (result.size() != 0) return false;
	result = intersecttilesets(primes, even);
	if (result.size() != 1) return false;
	if (result.find((Tile*) 2) == result.end()) return false;
	return true;
}
#endif
