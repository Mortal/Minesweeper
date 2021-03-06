OPTS=-Wall -O3

.cpp.o:
	g++ $(OPTS) -c $<

OBJECTS = Game.o MineFieldFilters.o Minesweeper.o PlayerHuman.o PlayerRobot.o PTileSetOperations.o test.o Tile.o Timer.o t_PTileSetOperations.o Logger.o Screen.o Move.o Tick.o
all: $(OBJECTS)
	g++ $(OPTS) -lncurses $(OBJECTS) -o minesweeper

clean:
	rm -v $(OBJECTS) minesweeper || true

ai4d:
	@echo Running a minesweeper example: AI in 4D
	./minesweeper 9 4 8 4

ai2d:
	@echo Running a minesweeper example: AI in 2D
	./minesweeper 75 35

screensaver:
	./minesweeper --fullscreen --no-wait --repeat
# DO NOT DELETE
