OPTS=-Wall -O3

.cpp.o:
	g++ $(OPTS) -c $<

OBJECTS = Game.o MineFieldFilters.o minesweeper.o PlayerHuman.o PlayerRobot.o PTileSetOperations.o test.o Tile.o Timer.o t_PTileSetOperations.o ColourHandler.o
all: $(OBJECTS)
	g++ $(OPTS) $(OBJECTS) -o minesweeper
# DO NOT DELETE
