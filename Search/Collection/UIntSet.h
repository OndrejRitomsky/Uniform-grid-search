#pragma once

#include <cassert>

#include "../Allocator/IAllocator.h"
#include "../Utility/Memory.h"
#include "../Utility/Util.h"

//  UIntSet
//    Very simple HashSet for unsigned int (easily extended to unsigned integrals: uchar, ushort, uint, ulong, ulonglong (void*))
//    Unsigned integral dont need extra templates for CompareFunc (bcs its trivial ==) or HashFunc as they are hashes. (Not uniformly distributed?)
//    Signed integrals would be implementation dependant because of operator %. (They can still be converted by callee)
//
//    Other INTERNAL implementation details:
//      FULL RATIO == 0.6
//      Using extra indices (_nexts) to faster look for correct values which had the same hash after operator %. (Extra memory, for faster traversing)
//      FindRes _previous and _index are INVALID_INDEX  -> values count is 0
//              _index is END_OF_CHAIN_INDEX            -> its not inside, (to ADD probing needs to be done from _previous)
//              _index is good index                    -> res ? its on that index : can be added on that index




class UIntSet {
public:
	// Content of the structure is used internaly, its given to user to speedup Find + Add / Remove combination (does only one find)
	// Upon changing the UIntSet FindResult is invalid
	struct FindResult {
		unsigned int _previous;
		unsigned int _index;
	};

private:
	static const unsigned int FIRST_ADD_CAPACITY = 16;

	static const unsigned int INVALID_INDEX      = 0xFFffFFffu;
	static const unsigned int END_OF_CHAIN_INDEX    = 0xFFffFFfeu;

public:
	UIntSet();
	~UIntSet();

	UIntSet(const UIntSet& oth) = delete;
	UIntSet& operator=(const UIntSet& rhs) = delete;

	void Init(IAllocator* allocator);

	void Add(unsigned int value);
	void Add(const FindResult* findRes, unsigned int value);

	void Remove(unsigned int value);
	void Remove(const FindResult* findRes, unsigned int value);

	bool Find(unsigned int value, FindResult* outFindRes = nullptr) const;

	unsigned int Count() const;
	bool Empty() const;

private:
	void Reallocate(unsigned int newCapacity);
	void Probe(FindResult* inOutFindRes) const;

	void InternalAdd(const FindResult* findRes, unsigned int value);
	void InternalRemove(const FindResult* findRes);

	bool IsFull() const;

private:
	unsigned int _count;
	unsigned int _capacity;

	unsigned int* _values;
	unsigned int* _nexts;

	IAllocator* _allocator;
};








inline UIntSet::UIntSet() :
	_count(0),
	_capacity(0),
	_nexts(nullptr),
	_values(nullptr),
	_allocator(nullptr) {
}

inline UIntSet::~UIntSet() {
	if (_values) {
		Deallocate(_allocator, _values);
	}
}

inline void UIntSet::Init(IAllocator* allocator) {
	assert(!_allocator);
	_allocator = allocator;
}

inline void UIntSet::Add(const FindResult* findRes, unsigned int value) {
	InternalAdd(findRes, value);
}

inline void UIntSet::Add(unsigned int value) {
	FindResult find;
	bool found = Find(value, &find);

	if (!found) {
		InternalAdd(&find, value);
	}
}

inline void UIntSet::InternalAdd(const FindResult* findRes, unsigned int value) {
	assert(findRes);

	FindResult find = *findRes;

	if (IsFull()) {
		Reallocate(_capacity == 0 ? FIRST_ADD_CAPACITY : (_capacity << 1));
		Find(value, &find);
	}

	if (find._index == END_OF_CHAIN_INDEX) {
		// The hash index was taken, probe for new spot
		Probe(&find);
	}
	assert(find._index != INVALID_INDEX);

	assert(find._index < _capacity);
	_values[find._index] = value;
	_nexts[find._index] = END_OF_CHAIN_INDEX;

	if (find._previous != INVALID_INDEX) {
		_nexts[find._previous] = find._index;
	}
	++_count;
}

inline void UIntSet::Remove(unsigned int value) {
	FindResult find;
	bool found = Find(value, &find);

	if (found) {
		InternalRemove(&find);
	}
}

inline void UIntSet::Remove(const FindResult* findRes, unsigned int value) {
	assert(findRes);
	assert(findRes->_index < _capacity);
	assert(_values[findRes->_index] == value);
	InternalRemove(findRes);
}

inline void UIntSet::InternalRemove(const FindResult* findRes) {
	assert(findRes);
	assert(findRes->_index < _capacity);

	if (findRes->_previous != INVALID_INDEX) {
		assert(findRes->_previous < _capacity);
		_nexts[findRes->_previous] = _nexts[findRes->_index];
	}

	_nexts[findRes->_index] = INVALID_INDEX;
	--_count;
}


inline bool UIntSet::Find(unsigned int value, FindResult* outFindRes) const {
	unsigned int index = INVALID_INDEX;
	unsigned int prev = INVALID_INDEX;
	bool found = false;

	if (_capacity > 0) {
		assert(IsPowerOfTwo(_capacity));
		index = (2654435769u * value) & (_capacity - 1);

		// Might be good idea to check collision count and reallocate if its too high 
		while (_nexts[index] != INVALID_INDEX && _nexts[index] != END_OF_CHAIN_INDEX) {
			if (_values[index] == value) {
				found = true;
				break;
			}

			prev = index;
			index = _nexts[index];
		}

		if (_nexts[index] == END_OF_CHAIN_INDEX) {
			found = _values[index] == value;
			if (!found) {
				prev = index;
				index = END_OF_CHAIN_INDEX;
			}
		}
	}

	if (outFindRes) {
		outFindRes->_index = index;
		outFindRes->_previous = prev;
	}

	return found;
}

inline void UIntSet::Reallocate(unsigned int newCapacity) {
	assert(_allocator);

	size_t sizeNeeded = newCapacity * (2 * sizeof(unsigned int)) + alignof(unsigned int);

	void* mem = Allocate(_allocator, sizeNeeded, alignof(unsigned int));

	unsigned int* oldNexts = _nexts;
	unsigned int* oldValues = _values;
	unsigned int oldCapacity = _capacity;

	_values = static_cast<unsigned int*>(mem);
	_nexts = static_cast<unsigned int*>(AlignForward(static_cast<char*>(mem) + newCapacity * sizeof(unsigned int), alignof(unsigned int)));
	_capacity = newCapacity;

	for (unsigned int i = 0; i < _capacity; ++i) {
		_nexts[i] = INVALID_INDEX; 
	}

	if (_count != 0) {
		_count = 0;
		// copy all, data arent contiguous
		for (unsigned int i = 0; i < oldCapacity; ++i) {
			if (oldNexts[i] != INVALID_INDEX)
				Add(oldValues[i]);
		}

		Deallocate(_allocator, oldValues);
	}
}

inline void UIntSet::Probe(FindResult* inOutFindRes) const {
	assert(inOutFindRes->_index == END_OF_CHAIN_INDEX);

	for (unsigned int i = inOutFindRes->_previous + 1; i < _capacity; ++i) {
		if (_nexts[i] == INVALID_INDEX) {
			inOutFindRes->_index = i;
			return;
		}
	}

	for (unsigned int i = 0; i < inOutFindRes->_previous; ++i) {
		if (_nexts[i] == INVALID_INDEX) {
			inOutFindRes->_index = i;
			return;
		}
	}

	assert(false);
}

inline bool UIntSet::IsFull() const {
	const float FULL_RATIO = 0.6f;
	return _count >= static_cast<unsigned int>(_capacity * FULL_RATIO);
}

inline unsigned int UIntSet::Count() const {
	return _count;
}

inline bool UIntSet::Empty() const {
	return _count == 0;
}