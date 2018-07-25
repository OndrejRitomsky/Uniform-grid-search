#pragma once

#include <cassert>
#include "../Utility/Memory.h"

struct BitArray {
	int capacity; // in bytes
	char* data;
};

inline BitArray BitArrayMake(char* data, int capacity) {
	return BitArray{capacity,data};
}

inline void BitArraySet(BitArray* arr, int index) {
	assert((index >> 3) < arr->capacity);
	arr->data[(index >> 3)] |= (1 << ((index) & 7));
}

inline bool BitArrayIs(BitArray* arr, int index) {
	assert((index >> 3) < arr->capacity);
	return arr->data[index >> 3] & (1 << (index & 7));
}

inline void BitArrayClear(BitArray* arr) {
	MemSet(arr->data, 0, arr->capacity);
}