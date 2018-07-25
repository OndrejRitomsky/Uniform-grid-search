#include "Tests.h"

#include "Collection/UIntSet.h"
#include "Collection/MinPriorityQueue.h"

#include "Allocator/IAllocator.h"
#include "Allocator/HeapAllocator.h"

#include <cstdio>

#include <time.h>
#include <algorithm>


static void TestAssert(bool res, const char* msg) {
	if (!res) {
		printf("ERROR: %s\n", msg);
		assert(false);
	}
}


static void TestPriorityQueue() {
	HeapAllocator allocator;
	InitHeapAllocator(&allocator);
	
	{
		MinPriorityQueue<int> q;
		q.Init(&allocator);

		int vals[] = {55, 22, 88, 1919, 1414, 11, 1010};
		int weights[] = {5, 2, 8, 19, 14, 1, 10};

		int sorted[] = {11, 22, 55, 88, 1010, 1414, 1919};
		int sortedWeights[] = {1, 2, 5, 8, 10, 14, 19};

		for (int i = 0; i < sizeof(weights) / sizeof(int); ++i)
			q.Add(vals[i], weights[i]);

		int i = 0;
		int w, v;
		while (!q.Empty()) {
			v = q.First();
			w = q.FirstWeight();
			q.PopFirst();

			TestAssert(v == sorted[i], "PriorityQueue Pop -ing value should match ordered array");
			TestAssert(w == sortedWeights[i], "PriorityQueue Pop -ing weight should match ordered array");
			++i;
		}

		TestAssert(q.Empty(), "PriorityQueue should be empty");
	}

	{
		MinPriorityQueue<int> q;
		q.Init(&allocator);
		TestAssert(q.Empty(), "PriorityQueue should be empty");
	}

	{
		// Random tests, they are hard to reproduce, but good as bruteforce to find if any problem exist
		//srand(time(NULL));
		const int COUNT = 50;
		const int MAX_VALS = 500;

		int weights[MAX_VALS + 10];

		for (int i = 0; i < COUNT; ++i) {
			MinPriorityQueue<int> q;
			q.Init(&allocator);
			int n = rand() % MAX_VALS;

			for (int j = 0; j < n; ++j) {
				weights[j] = rand() % MAX_VALS;
				q.Add(weights[j], weights[j]);
			}
			std::sort(weights, &weights[n]);

			int k = 0;
			int w;
			while (!q.Empty()) {
				w = q.FirstWeight();
				q.PopFirst();

				TestAssert(w == weights[k], "PriorityQueue Pop -ing should match ordered array");
				++k;
			}

			TestAssert(q.Empty(), "PriorityQueue should be empty");
		}
	}

	AllocatorDestruct(&allocator);
}



static void TestUIntSet() {
	HeapAllocator allocator;
	InitHeapAllocator(&allocator);

	{
		UIntSet set;
		set.Init(&allocator);
		set.Add(1);
		TestAssert(set.Count() == 1, "Set should have correct number of elements");
	}

	{
		UIntSet set;
		set.Init(&allocator);
		set.Add(1);
		set.Add(4);
		set.Add(6);
		bool find[] = {0,1,0,0,1,0,1,0,0,0};
		for (int i = 0; i < 10; i++) {
			TestAssert(set.Find(find[i]) == find[i], "Set find returned incorrect value");
		}
		TestAssert(set.Count() == 3, "Set should have correct number of elements");
	}


	{
		UIntSet set;
		set.Init(&allocator);
		unsigned int add[] = {1,4,19,18,10,13};
		unsigned int remove[] = {4,18};
		unsigned int has[] = {1,19,10,13};

		for (int i = 0; i < 6; i++)
			set.Add(add[i]);

		for (int i = 0; i < 2; i++)
			set.Remove(remove[i]);

		for (int i = 0; i < 4; i++)
			TestAssert(set.Find(has[i]), "Set find returned false where it should return true");

		for (int i = 0; i < 2; i++)
			TestAssert(!set.Find(remove[i]), "Set find returned true where it should return false");

		TestAssert(set.Count() == 4, "Set should have correct number of elements");
	}

	{
		UIntSet set;
		set.Init(&allocator);
		unsigned int add[] = {1,11,21,4,8};
		unsigned int remove[] = {11,4,21};
		unsigned int has[] = {1,8};

		for (int i = 0; i < 5; i++) 
			set.Add(add[i]);

		for (int i = 0; i < 3; i++) 
			set.Remove(remove[i]);

		for (int i = 0; i < 2; i++) 
			TestAssert(set.Find(has[i]), "Set find returned false where it should return true");

		for (int i = 0; i < 3; i++) 
			TestAssert(!set.Find(remove[i]), "Set find returned true where it should return false");

		TestAssert(set.Count() == 2, "Set should have correct number of elements");
	}

	{
		UIntSet set;
		set.Init(&allocator);
		const unsigned int N = 60;
		for (unsigned int i = 0; i < N; i += 2)
			set.Add(i);

		for (unsigned int i = 0; i < N; i += 4)
			set.Remove(i);

		for (unsigned int i = 0; i < N; i++) {
			bool has = set.Find(i);
			TestAssert(has == (i % 4 == 2), "Set find returned false where it should return true");
		}
	}

	{
		UIntSet set;
		set.Init(&allocator);
		const unsigned int N = 100;
		for (unsigned int i = 0; i < N; i += 2)
			set.Add(i);

		for (unsigned int i = 0; i < N; i += 4)
			set.Remove(i);

		for (unsigned int i = 0; i < N; i += 8)
			set.Add(i);

		for (unsigned int i = 0; i < N; i++) {
			bool has = set.Find(i);
			TestAssert(has == (i % 4 == 2 || i % 8 == 0), "Set find returned false where it should return true");
		}
	}
}

void TestAll() {
	TestPriorityQueue();

	TestUIntSet();
}