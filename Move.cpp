#include "Move.h"

TileMove::TileMove(CoordinateSet p) : point(p) {
}

CoordinateSet TileMove::getPoint() {
	return point;
}

FlagOnMove::FlagOnMove(CoordinateSet p) : TileMove(p) {
}

void FlagOnMove::act(Game *field) {
	field->flagon(getPoint());
}

FlagOffMove::FlagOffMove(CoordinateSet p) : TileMove(p) {
}

void FlagOffMove::act(Game *field) {
	field->flagoff(getPoint());
}

PressMove::PressMove(CoordinateSet p) : TileMove(p) {
}

void PressMove::act(Game *field) {
	field->amIDeadNow(getPoint());
}
