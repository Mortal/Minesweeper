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

#define START(mines) Game field(size.size(), size, timer); field.startgame(mines);
#define DIM(i) size.push_back(i)
#define RULES2D(x,y,mines) DIM(y); DIM(x); START(mines);
#define RULES3D(x,y,z,mines) DIM(z); DIM(y); DIM(x); START(mines);
#define RULES4D(x,y,z,w,mines) DIM(w); DIM(z); DIM(y); DIM(x); START(mines);
#define RULESWMEASY RULES2D(9,9,10) // 81
#define RULESWMNORMAL RULES2D(16,16,40) // 256
#define RULESWMHARD RULES2D(16,30,99) // 480
#define RULES2DEASY RULES2D(20,20,40) // 400
#define RULES2DNORMAL RULES2D(40,30,200) // 1200
#define RULES2DHARD RULES2D(60,40,600) // 2400
#define RULES3DEASY RULES3D(20,8,4,30) // 640
#define RULES3DNORMAL RULES3D(30,9,5,120) // 1350
#define RULES3DHARD RULES3D(40,10,6,400) // 2400
#define RULES4DEASY RULES4D(8,5,4,4,10) // 640

int main(int argc, char* argv[])
{
	std::vector<std::string> args;
	args.reserve(argc-1);
	for (int i = 1; i < argc; ++i) {
		args.push_back(argv[i]);
	}
	std::vector<std::string>::iterator argi;
	unsigned int mines = 10;
	bool automines = true;
	SizeVector size;
	ProgramOptions opts;
	opts.verbose = false;
	opts.ai = false;
	opts.waitonquit = false;
	opts.fieldoutput = false;
	opts.fieldfile = "";
#ifdef TIMING
	opts.timer = new Timer;
#else
	opts.timer = new NullTimer;
#endif
	NullTimer *timer = opts.timer;
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
			opts.ai = !opts.ai;
		} else if (*argi == "-F" || *argi == "--fieldoutput") {
			opts.fieldoutput = !opts.fieldoutput;
		} else if (*argi == "-w" || *argi == "--wait") {
			opts.waitonquit = !opts.waitonquit;
		} else if (*argi == "-f" || *argi == "--fieldfile") {
			++argi;
			if (argi == args.end()) break;
			opts.fieldfile = *argi;
		} else {
			unsigned int input;
			std::stringstream numstream(*argi);
			numstream >> input;
			if (input) size.push_back(input);
		}
	}
	if (size.size() == 1) {
		std::cerr << "Only got one dimension! That's no fun" << std::endl;
		return 1;
	} else if (size.empty()) {
		size.push_back(4);
		size.push_back(8);
		size.push_back(20);
		if (!automines) {
			std::cerr << "You specified a number of mines but not dimensions! I'll choose number of mines instead" << std::endl;
		}
		automines = true;
		mines = 30;
		if (opts.verbose) std::cerr << "Default settings: 3D field of 20x8x4 with 30 mines" << std::endl;
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
			std::cerr << "Settings: " << size.size() << "D field of ";
			for (i = size.rbegin(); i != size.rend(); ++i) {
				if (i != size.rbegin()) std::cerr << "x";
				std::cerr << *i;
			}
			std::cerr << " with " << mines << " mines" << std::endl;
		}
	}
	timer->endtime("Options parsing");
#ifdef TESTING
	if (opts.verbose) std::cout << "Running tests" << std::endl;
	timer->starttime("Unit tests");
	if (!test()) {
		return 42;
	}
	timer->endtime("Unit tests");
#endif
	if (opts.verbose) std::cout << "Establishing gamefield" << std::endl;
	timer->starttime("Establishing gamefield");
	//RULES4DEASY;
	START(mines);
	timer->endtime("Establishing gamefield");

	initscr();
	getch(); // why is this necessary?
	start_color();
	init_pair(1, COLOR_RED, COLOR_BLACK); // flags and blown mines
	init_pair(2, COLOR_CYAN, COLOR_BLACK); // unpressed tiles
	init_pair(3, COLOR_WHITE, COLOR_BLACK); // pressed tiles

	Screen *scr = new Screen();
	std::ostream *console = scr->getConsole();
	scr->setfieldsize(field.getOutputWidth(), field.getOutputHeight());
	field.setBombField(scr->getBombField());
	field.output();

	if (opts.verbose) *console << "Playing game" << std::endl;
	timer->starttime("Create player");
	Player *player;
	if (opts.ai) {
		player = new PlayerRobot(&field, console, opts, timer);
	} else {
		player = new PlayerHuman(&field, console);
	}
	timer->endtime("Create player");

	timer->starttime("Play");
	while (field.getState() == GAMESTATE_PLAY) {
		Tick *tick = player->tick();
		if (tick == NULL) break;
		*console << tick->getDescription() << std::endl;
		MoveList moves = tick->getMoves();
		bool giveup = false;
		for (MoveListIt i = moves.begin(); field.getState() == GAMESTATE_PLAY && i != moves.end(); ++i) {
			Move *m = *i;
			if (NULL != dynamic_cast<GiveUpMove *>(m)) {
				giveup = true;
			} else {
				m->act(&field);
			}
		}
		if (giveup) break;
	}
	timer->endtime("Play");
	{
		const char *msg = "";
		if (field.getState() == GAMESTATE_WIN) msg = "Congratulations!";
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
	delete player;
	delete opts.timer;
	opts.timer = NULL;
	return 0;
}

