#pragma once

#include <atomic>

class SpinLock {
public:
	SpinLock();

	SpinLock(const SpinLock& oth) = delete;
	SpinLock& operator=(const SpinLock& rhs) = delete;

	void Lock();
	void Unlock();

private:
	std::atomic_flag _atomicFlag;
};

