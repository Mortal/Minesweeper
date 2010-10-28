#ifndef PLAYER_H
#define PLAYER_H
class Player {
public:
	Player(class Game *field) {};
	virtual class Tick *tick() = 0;
protected:
	Player() {};
};
#endif
