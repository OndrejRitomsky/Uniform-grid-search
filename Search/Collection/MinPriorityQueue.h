#pragma once

#include <cassert>

#include "../Allocator/IAllocator.h"
#include "../Utility/Move.h"
#include "../Utility/Memory.h"


template<typename T>
class MinPriorityQueue {
private:
	static const unsigned int FIRST_ADD_CAPACITY = 16;

public:
	MinPriorityQueue();
	~MinPriorityQueue();

	MinPriorityQueue(const MinPriorityQueue& oth) = delete;
	MinPriorityQueue& operator=(const MinPriorityQueue& rhs) = delete;

	void Init(IAllocator* allocator);

	void Add(const T& value, unsigned int weight);
	void Add(T&& value, unsigned int weight);

	void PopFirst();

	int FirstWeight() const;

	const T& First() const;
	T&& First();

	bool Empty() const;

private:
	void Reallocate(unsigned int newCapacity);

	// Moves weight from new last position to outIndex
	// Value has to be set on outIndex after the function is called
	void BubbleUp(unsigned int weight, unsigned int& outIndex);

private:
	unsigned int _count;
	unsigned int _capacity;

	T* _values;
	unsigned int* _weights;

	IAllocator* _allocator;
};








template<typename T>
inline MinPriorityQueue<T>::MinPriorityQueue() :
	_count(0),
	_capacity(0),
	_weights(nullptr),
	_values(nullptr),
	_allocator(nullptr) {
}

template<typename T>
inline MinPriorityQueue<T>::~MinPriorityQueue() {
	if (_values) {
		for (unsigned int i = 0; i < _count; ++i) {
			_values[i].~T();
		}

		Deallocate(_allocator, _values);
	}
}

template<typename T>
inline void MinPriorityQueue<T>::Init(IAllocator* allocator) {
	assert(!_allocator);
	_allocator = allocator;
}


template<typename T>
inline void MinPriorityQueue<T>::Add(const T& value, unsigned int weight) {
	unsigned int index;
	BubbleUp(weight, index);

	_values[index] = value;
}

template<typename T>
inline void MinPriorityQueue<T>::Add(T&& value, unsigned int weight) {
	unsigned int index;
	BubbleUp(weight, index);

	_values[index] = Move(value);
}

template<typename T>
inline int MinPriorityQueue<T>::FirstWeight() const {
	assert(!Empty());
	return _weights[0];
}

template<typename T>
inline const T& MinPriorityQueue<T>::First() const {
	assert(!Empty());
	return _values[0];
}

template<typename T>
inline T&& MinPriorityQueue<T>::First() {
	assert(!Empty());
	return Move(_values[0]);
}

template<typename T>
inline bool MinPriorityQueue<T>::Empty() const {
	return _count == 0;
}

template<typename T>
inline void MinPriorityQueue<T>::PopFirst() {
	assert(!Empty());

	_values[0].~T();

	--_count;

	// last is "moved" to first position and bubling down
	unsigned int weight = _weights[_count];
	T&& value = Move(_values[_count]);

	unsigned int i = 0, left = 1, right = 2;
	while (left < _count) {
		if (right >= _count || _weights[left] < _weights[right]) {
			if (_weights[left] <= weight) {   // last will stay "last"
				_weights[i] = _weights[left];
				_values[i] = Move(_values[left]);
				i = left;
			}
			else {
				break;
			}
		}
		else {
			if (_weights[right] <= weight) {
				_weights[i] = _weights[right];
				_values[i] = Move(_values[right]);
				i = right;
			}
			else {
				break;
			}
		}
		left = (i << 1) + 1;
		right = left + 1;
	}

	_weights[i] = weight;
	_values[i] = Move(value);
}

template<typename T>
inline void MinPriorityQueue<T>::BubbleUp(unsigned int weight, unsigned int& outIndex) {
	if (_count == _capacity)
		Reallocate(_count == 0 ? FIRST_ADD_CAPACITY : _count * 2);

	unsigned int i = _count;
	unsigned int parent = 0;
	while (i != 0) {
		parent = (i & 1) ? i >> 1 : (i >> 1) - 1; // i % 2 ? i/2 : i/2 - 1

		if (_weights[parent] >= weight) { // Newer has priority
			_values[i] = Move(_values[parent]);
			_weights[i] = _weights[parent];
			i = parent;
		}
		else {
			break;
		}
	}

	_weights[i] = weight;

	outIndex = i;
	++_count;
}

template<typename T>
inline void MinPriorityQueue<T>::Reallocate(unsigned int newCapacity) {
	assert(_allocator);

	size_t sizeNeeded = newCapacity * (sizeof(T) + sizeof(unsigned int)) + alignof(unsigned int);

	T* newValues = static_cast<T*>(Allocate(_allocator, sizeNeeded, alignof(T)));
	unsigned int* newWeights = static_cast<unsigned int*>(AlignForward(newValues + newCapacity, alignof(unsigned int)));

	if (_count != 0) {
		for (unsigned int i = 0; i < _count; ++i) {
			newValues[i] = Move(_values[i]);
			_values[i].~T();
		}

		MemCopy(newWeights, _weights, _count * sizeof(unsigned int));
		Deallocate(_allocator, _values);
	}

	_capacity = newCapacity;
	_values = newValues;
	_weights = newWeights;
}
