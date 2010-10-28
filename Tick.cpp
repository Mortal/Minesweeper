#include "Tick.h"

Tick::Tick(std::string d) : desc(d) {
}

void Tick::addMove(Move *move) {
	moves.push_back(move);
}

MoveList Tick::getMoves() {
	return moves;
}

std::string Tick::getDescription() {
	return desc;
}
