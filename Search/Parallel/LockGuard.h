#pragma once

template<typename LockType>
class LockGuard {
public:
	LockGuard(LockType& lock);

	LockGuard(const LockGuard& oth) = delete;
	LockGuard& operator=(const LockGuard& rhs) = delete;

	~LockGuard();
private:
	LockType& _lock;
};





template<typename LockType>
LockGuard<LockType>::LockGuard(LockType& lock) :
	_lock(lock) {

	_lock.Lock();
}

template<typename LockType>
LockGuard<LockType>::~LockGuard() {
	_lock.Unlock();
}


