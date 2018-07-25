#include "MapDraw.h"

#include <cstdio>

#include "Allocator/IAllocator.h"

#include "Collection/MinPriorityQueue.h"
#include "Collection/UIntSet.h"
#include "Collection/BitArray.h"

const char START = 'S';
const char TARGET = 'T';
const char PATH = 'P';
const char VISITED = '.';

void PrintMap(int startX, int startY, int targetX, int targetY,
	const unsigned char* map, int width, int height) {

	int node = 0;
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j, ++node) {
			char c = map[node] + '0';
			if (j == startX && i == startY)
				c = START;
			if (j == targetX && i == targetY)
				c = TARGET;
			printf("%c ", c);
		}
		printf("\n");
	}
	printf("\n");
}

void PrintMapResult(int startX, int startY, int targetX, int targetY,
	const unsigned char* map, int width, int height,
	int* buffer, const int bufferSize, BitArray* nodes, IAllocator* allocator) {
	

	UIntSet path;
	path.Init(allocator);
	for (int i = 0; i < bufferSize; ++i)
		path.Add(buffer[i]);

	int visitedCount = 0;

	int node = 0;
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j, ++node) {
			char c;
			if (j == startX && i == startY) {
				c = START;
			}
			else if (j == targetX && i == targetY) {
				c = TARGET;
			}
			else {
				c = path.Find(node) ? PATH : (BitArrayIs(nodes,node)? VISITED : map[node] + '0');
				if (c == VISITED)
					++visitedCount;
			}
			printf("%c ", c);
		}
		printf("\n");
	}
	printf("\n");

	printf("Path: %d\n", path.Count());
	printf("Visited: %d\n", visitedCount);
}

