#ifndef TIMER_H
#define TIMER_H
#ifdef WIN32
#include <windows.h>
#else
typedef long long LONGLONG;
#endif
#include <string>
class NullTimer {
public:
	void output() {};
	void starttime(std::string id) {};
	void endtime(std::string id) {};
	static LONGLONG qpc() {return 0;}
};
#ifdef TIMING
#include <map>
#include <list>
struct TimerEntry {
	unsigned int numtimes;
	LONGLONG total;
	bool started;
	LONGLONG start;
};
typedef std::map<std::string, TimerEntry*> TimerMap;
typedef TimerMap::iterator TimerMapIt;
class Timer: public NullTimer {
public:
	void output();
	void starttime(std::string id);
	void endtime(std::string id);
	static LONGLONG qpc();
private:
	TimerEntry *init(std::string id);
	TimerMap times;
};
#define TIMERON this->timer->starttime(__FUNCTION__)
#define TIMEROFF this->timer->endtime(__FUNCTION__)
#define TIMERRET(v) TIMEROFF; return (v)
#else // !TIMING
class Timer: public NullTimer {};
#define TIMERON ((void)0)
#define TIMEROFF ((void)0)
#define TIMERRET(v) return (v)
#endif
#endif
