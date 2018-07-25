#include "Memory.h"

#include <Utility>

void MemCopy(void* target, const void* source, size_t size) {
	memcpy(target, source, size);
}


void MemMove(void* target, void* source, size_t size) {
	memmove(target, source, size);
}

void MemSet(void* target, int val, size_t size) {
	memset(target, val, size);
}