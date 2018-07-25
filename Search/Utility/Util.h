#pragma once


bool IsPowerOfTwo(size_t x);



inline bool IsPowerOfTwo(size_t x) {
	return x != 0 && !(x & (x - 1));
}
