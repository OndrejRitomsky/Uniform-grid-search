#include "SpinLock.h"

SpinLock::SpinLock() {
	_atomicFlag.clear();
}

void SpinLock::Lock() {
	while (_atomicFlag.test_and_set(std::memory_order_acquire));
}

void SpinLock::Unlock() {
	_atomicFlag.clear(std::memory_order_release);
}
