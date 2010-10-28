#include "test.h"
#include "Game.h"
#include "types.h"
#include "PlayerHuman.h"
#include "PlayerRobot.h"
#include "Timer.h"
#include "Logger.h"
#include "Screen.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <ncurses.h>

class Minesweeper {
	public:
		Minesweeper(int argc, char* argv[]);
		~Minesweeper();
		int getExitCode();
	private:
		int exitcode;
		unsigned int mines;
		bool automines;
		SizeVector size;
		ProgramOptions opts;
		NullTimer *timer;
		std::ostream *console;
		Screen *scr;
		Game *field;

		void initfields();
		void parseargs(int argc, char* argv[]);
		void tests();
		void screenInit();
		void gameInit();
		void play();
		void conclusion();
};

Minesweeper::Minesweeper(int argc, char* argv[]) {
	initfields();
	try {
		screenInit();
		parseargs(argc, argv);
		tests();
		do {
			gameInit();
			play();
		} while (opts.repeat);
		conclusion();
	} catch (int e) {
		exitcode = e;
	}
}

int Minesweeper::getExitCode() {
	return exitcode;
}

void Minesweeper::initfields() {
	exitcode = 0;
	mines = 10;
	automines = true;
	opts.verbose = false;
	opts.repeat = false;
	opts.ai = true;
	opts.waitonquit = true;
#ifdef TIMING
	opts.timer = new Timer;
#else
	opts.timer = new NullTimer;
#endif
	timer = opts.timer;
	scr = NULL;
	console = NULL;
	field = NULL;
}

void Minesweeper::parseargs(int argc, char* argv[]) {
	std::vector<std::string> args;
	args.reserve(argc-1);
	for (int i = 1; i < argc; ++i) {
		args.push_back(argv[i]);
	}
	std::vector<std::string>::iterator argi;
	timer->starttime("Options parsing");
	for (argi = args.begin(); argi != args.end(); ++argi) {
		if (*argi == "-m" || *argi == "--mines") {
			++argi;
			if (argi == args.end()) break;
			std::stringstream minestream(*argi);
			minestream >> mines;
			automines = false;
		} else if (*argi == "--verbose") {
			opts.verbose = !opts.verbose;
		} else if (*argi == "--ai") {
			opts.ai = true;
		} else if (*argi == "--no-ai") {
			opts.ai = false;
		} else if (*argi == "-w") {
			opts.waitonquit = !opts.waitonquit;
		} else if (*argi == "--wait") {
			opts.waitonquit = true;
		} else if (*argi == "--no-wait") {
			opts.waitonquit = false;
		} else if (*argi == "-r" || *argi == "--repeat") {
			opts.repeat = !opts.repeat;
		} else {
			unsigned int input;
			std::stringstream numstream(*argi);
			numstream >> input;
			if (input) size.push_back(input);
		}
	}
	if (size.size() == 1) {
		*console << "Only got one dimension! That's no fun" << std::endl;
		throw 1;
	} else if (size.empty()) {
		size.push_back(4);
		size.push_back(8);
		size.push_back(20);
		if (!automines) {
			*console << "You specified a number of mines but not dimensions! I'll choose number of mines instead" << std::endl;
		}
		automines = true;
		mines = 30;
		if (opts.verbose) *console << "Default settings: 3D field of 20x8x4 with 30 mines" << std::endl;
	} else {
		SizeVector size2;
		size2.reserve(size.size());
		SizeVector::reverse_iterator i;
		for (i = size.rbegin(); i != size.rend(); ++i) size2.push_back(*i);
		size.swap(size2);
		if (automines) {
			unsigned int fields = 1;
			SizeVectorIt i;
			for (i = size.begin(); i != size.end(); ++i) fields *= *i;
			mines = fields/(size.size()*size.size()*size.size());
		}
		if (opts.verbose) {
			*console << "Settings: " << size.size() << "D field of ";
			for (i = size.rbegin(); i != size.rend(); ++i) {
				if (i != size.rbegin()) *console << "x";
				*console << *i;
			}
			*console << " with " << mines << " mines" << std::endl;
		}
	}
	timer->endtime("Options parsing");
}

void Minesweeper::tests() {
#ifdef TESTING
	if (opts.verbose) std::cout << "Running tests" << std::endl;
	timer->starttime("Unit tests");
	if (!test()) {
		return 42;
	}
	timer->endtime("Unit tests");
#endif
}

void Minesweeper::screenInit() {
	scr = new Screen();
	console = scr->getConsole();
}

void Minesweeper::gameInit() {
	if (opts.verbose) *console << "Establishing gamefield" << std::endl;
	if (field != NULL) delete field;
	timer->starttime("Establishing gamefield");
	field = new Game(size.size(), size, timer);
	field->startgame(mines);
	timer->endtime("Establishing gamefield");

	initscr();
	if (opts.waitonquit) {
		printw("Press any key to begin\n");
		refresh();
		getch(); // why is this necessary?
	}
	start_color();
	init_pair(1, COLOR_RED, COLOR_BLACK); // flags and blown mines
	init_pair(2, COLOR_CYAN, COLOR_BLACK); // unpressed tiles
	init_pair(3, COLOR_WHITE, COLOR_BLACK); // pressed tiles

	scr->setfieldsize(field->getOutputWidth()+1, field->getOutputHeight()+1);
	field->setBombField(scr->getBombField());
	field->output();
}

void Minesweeper::play() {
	if (opts.verbose) *console << "Playing game" << std::endl;
	timer->starttime("Create player");
	Player *player;
	if (opts.ai) {
		player = new PlayerRobot(field, console, opts, timer);
	} else {
		player = new PlayerHuman(field, console);
	}
	timer->endtime("Create player");

	timer->starttime("Play");
	while (field->getState() == GAMESTATE_PLAY) {
		Tick *tick = player->tick();
		if (tick == NULL) break;
		if (opts.verbose) *console << tick->getDescription() << std::endl;
		MoveList moves = tick->getMoves();
		bool giveup = false;
		for (MoveListIt i = moves.begin(); field->getState() == GAMESTATE_PLAY && i != moves.end(); ++i) {
			Move *m = *i;
			if (NULL != dynamic_cast<GiveUpMove *>(m)) {
				giveup = true;
			} else {
				m->act(field);
			}
		}
		if (giveup) break;
	}
	timer->endtime("Play");
	delete player;
}

void Minesweeper::conclusion() {
	{
		const char *msg = "";
		if (field->getState() == GAMESTATE_WIN) msg = "Congratulations!";
		else msg = "Game lost!";
		if (opts.waitonquit) {
			*console << msg << std::endl;
			getch();
			endwin();
		} else {
			endwin();
			std::cout << msg << std::endl;
		}
	}
	timer->output();
}

Minesweeper::~Minesweeper() {
	if (scr != NULL) {
		delete scr;
		scr = NULL;
	}
	if (field != NULL) {
		delete field;
		field = NULL;
	}
	if (opts.timer != NULL) {
		delete opts.timer;
		opts.timer = timer = NULL;
	}
}

int main(int argc, char* argv[]) {
	Minesweeper m(argc, argv);
	return m.getExitCode();
}
