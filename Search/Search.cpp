
#include "config.h"

#include "Tests.h"
#include "MapDraw.h"

#include "Utility/Timer.h"

#include "Collection/BitArray.h"
#include "Collection/MinPriorityQueue.h"

#include "Allocator/IAllocator.h"
#include "Allocator/HeapAllocator.h"

#include <cmath>

#include <Windows.h>

IAllocator* g_Allocator;

// A*
int FindPath(const int nStartX, const int nStartY,
	const int nTargetX, const int nTargetY,
	const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
	int* pOutBuffer, const int nOutBufferSize) {

	int nodesCount = nMapWidth * nMapHeight;
	int start = nStartX + nStartY * nMapWidth;
	int target = nTargetX + nTargetY * nMapWidth;

	MinPriorityQueue<int> queue;
	queue.Init(g_Allocator);
	queue.Add(start, 0);

	int allocSize = nodesCount * sizeof(int) * 2 + alignof(int);

	int bitArraySize = (nodesCount / 8) + 1;
	allocSize += bitArraySize + alignof(char);

	void* mem = Allocate(g_Allocator, allocSize, alignof(int));
	int* costs = static_cast<int*>(mem);
	int* fromNode = static_cast<int*>(AlignForward(costs + nodesCount, alignof(int)));

	BitArray closed = BitArrayMake(static_cast<char*>(AlignForward(fromNode + nodesCount, alignof(char))), bitArraySize);
	BitArrayClear(&closed);

	int nbdx[4] = {0, 1, 0,-1};
	int nbdy[4] = {-1, 0, 1, 0};

	for (int i = 0; i < nodesCount; i++)
		costs[i] = 0x7fffffff;

	costs[start] = 0;
	fromNode[start] = start;

	bool found = false;

	PROFILE_START(search)

	while (!queue.Empty()) {
		int node = queue.First();

		queue.PopFirst();

		if (node == target) {
			found = true;
			break;
		}

		int y = node / nMapWidth;
		int x = node % nMapWidth;

		assert(node < nodesCount);
		int cost = costs[node] + 1;

		BitArraySet(&closed, node);

		for (int i = 0; i < 4; ++i) {
			int nby = y + nbdy[i];
			int nbx = x + nbdx[i];

			// This can be removed, if map is garuanteed
			if (nbx < 0 || nbx >= nMapWidth || nby < 0 || nby >= nMapHeight)
				continue;

			int nb = nbx + nby * nMapWidth;

			if (pMap[nb] == 0 || BitArrayIs(&closed, nb))
				continue;

			assert(nb < nodesCount);
			if (cost >= costs[nb]) 
				continue;

			// With Manhatten heur. in uniform cost grid, we always get to the point in one of the fastest paths
			int heur = (abs(nTargetX - nbx) + abs(nTargetY - nby));

			// Since the cost is uniform (+1), the cost in queue never has to be updated
			queue.Add(nb, cost + heur);
			fromNode[nb] = node;
			costs[nb] = cost;
		}
	}


	PROFILE_END_LOG(search);

	int pathCost = costs[target];

	if (found && pathCost < nOutBufferSize) {
		int node = target;
		int i = 0;
		while (node != start) {
			pOutBuffer[i++] = node;
			node = fromNode[node];
		}
	}

#if defined(_DEBUG)
	PrintMap(nStartX, nStartY, nTargetX, nTargetY, pMap, nMapWidth, nMapHeight);

	PrintMapResult(nStartX, nStartY, nTargetX, nTargetY, pMap, nMapWidth, nMapHeight, pOutBuffer, found ? costs[target] : 0, &closed, g_Allocator);
#endif

	Deallocate(g_Allocator, mem);
	return pathCost;
}

int main() {	
	// TestAll();

	HeapAllocator allocator;
	g_Allocator = InitHeapAllocator(&allocator);

	unsigned char map[] = {1,1,1,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,
													1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,
													1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,
													1,1,1,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,1,1,1,1,0,1,1,1,1,
													1,1,1,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,0,1,1,1,1,
													1,1,1,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,0,1,1,1,1,
													1,1,1,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,0,1,1,1,1,
													1,1,1,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,0,1,1,1,1,
													1,1,1,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,0,1,1,1,1,
													1,1,1,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,
													1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,
													1,1,1,1,1,1,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,1,1,1,1,
													1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1};
	
	int result[800];

	FindPath(0, 0, 31, 12, map, 32, 13, result, 800);
	

	return 0;
}

