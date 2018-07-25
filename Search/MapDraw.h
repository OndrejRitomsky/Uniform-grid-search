#pragma once

class UIntSet;

struct IAllocator;
struct BitArray;

void PrintMap(int startX, int startY, int targetX, int targetY,
	const unsigned char* map, int width, int height);

void PrintMapResult(int startX, int startY, int targetX, int targetY,
	const unsigned char* map, int width, int height,
	int* buffer, const int bufferSize, BitArray* nodes, IAllocator* allocator);