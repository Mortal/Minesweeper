OPTS=-Wall -O3

.cpp.o:
	g++ $(OPTS) -c $<

OBJECTS = Game.o MineFieldFilters.o minesweeper.o PlayerHuman.o PlayerRobot.o PTileSetOperations.o test.o Tile.o Timer.o t_PTileSetOperations.o ColourHandler.o Logger.o Screen.o Move.o Tick.o
all: $(OBJECTS)
	g++ $(OPTS) -lncurses $(OBJECTS) -o minesweeper

clean:
	rm -v $(OBJECTS) minesweeper || true

ai4d:
	@echo Running a minesweeper example: AI in 4D
	./minesweeper 9 4 8 4 --ai -F -w --verbose

ai2d:
	@echo Running a minesweeper example: AI in 2D
	./minesweeper 75 35 --ai -F -w --verbose
# DO NOT DELETE
