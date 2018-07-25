#pragma once

#include "../Config.h"

#if PROFILE and MSVC

#include <cstdio> // tmp
#include <intrin.h>

#include <windows.h>

inline long long int QueryCycles() {
	return __rdtsc(); // in cycles .... for nano -> * 1000000 / QueryPerformanceFrequency LARGE_INTEGER.QuadPart;
}

struct TimedBlock {
	TimedBlock(const char* name) {
		_cycleCount = QueryCycles();
		_name = name;
	}

	~TimedBlock() {
		printf("%s time: %lld \n", _name, QueryCycles() - _cycleCount);
	}
	const char* _name;
	long long int _cycleCount;
};

#define PROFILE_BLOCK() TimedBlock block##__FUNCTION__##__LINE__(__FUNCTION__);

#define PROFILE_START(name) auto name = QueryCycles();
#define PROFILE_END(name) QueryCycles() - name 

#define PROFILE_END_LOG(name) printf("%s time: %lld \n", #name, PROFILE_END(name));


#endif
