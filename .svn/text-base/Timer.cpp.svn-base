#ifdef TIMING
#include "Timer.h"
#include <windows.h>
#include <map>
#include <string>
#include <iostream>
LONGLONG Timer::qpc() {
	LARGE_INTEGER *result = new LARGE_INTEGER;
	if (QueryPerformanceCounter(result)) {
		LONGLONG res = result->QuadPart;
		delete result;
		return res;
	}
	delete result;
	return 0;
}
TimerEntry *Timer::init(std::string id) {
	TimerMapIt i = this->times.find(id);
	if (i != this->times.end()) return i->second;
	TimerEntry *result = new TimerEntry();
	result->numtimes = 0;
	result->total = 0;
	result->started = false;
	result->start = 0;
	this->times.insert(std::pair<std::string, TimerEntry *>(id, result));
	return result;
}
void Timer::starttime(std::string id) {
	TimerEntry *entry = this->init(id);
	entry->started = true;
	entry->start = Timer::qpc();
}
void Timer::endtime(std::string id) {
	LONGLONG now = Timer::qpc();
	TimerEntry *entry = this->init(id);
	if (entry->started) {
		entry->started = false;
		entry->total += now-entry->start;
		++entry->numtimes;
	}
}
void Timer::output() {
	TimerMapIt i;
	for (i = this->times.begin(); i != this->times.end(); ++i) {
		std::cout << i->first << ": " << i->second->total << "/" << i->second->numtimes << std::endl;
	}
}
#endif
