#pragma once



// Align memory adress to next given alignment
// Alignment must be power of two
void* AlignForward(void* address, size_t alignment);


void MemCopy(void* target, const void* source, size_t size);


void MemMove(void* target, void* source, size_t size);


void MemSet(void* target, int val, size_t size);


inline void* AlignForward(void* address, size_t alignment) {
	return (void*) ((((size_t) address) + alignment - 1) & (~(alignment - 1)));
}