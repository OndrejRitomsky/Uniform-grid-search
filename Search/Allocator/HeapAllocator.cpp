#include "HeapAllocator.h"

#include <malloc.h>
#include <cassert>
#include <cstdint>

#include "../Utility/Util.h"
#include "../Utility/Memory.h"

#include "../Parallel/SpinLock.h"
#include "../Parallel/LockGuard.h"


void HeapDeallocate(IAllocator* allocator, void* mem) {
	assert(allocator);
	HeapAllocator* heapAllocator = static_cast<HeapAllocator*>(allocator);

	void* real = *((void**) mem - 1);

	SpinLock lock;
	LockGuard<SpinLock> lockGuard(lock);

	assert(heapAllocator->_allocationsCount > 0);
	--heapAllocator->_allocationsCount;

	free(real); // Just to be sure lock free too
}

void* HeapAllocate(IAllocator* allocator, size_t size, size_t alignment) {
	assert(allocator);
	assert(IsPowerOfTwo(alignment));

	HeapAllocator* heapAllocator = static_cast<HeapAllocator*>(allocator);

	size_t extraSize = alignment - 1 + sizeof(void*);

	char* mem;
	{
		SpinLock lock;
		LockGuard<SpinLock> lockGuard(lock);
		++heapAllocator->_allocationsCount;
		mem = (char*) malloc(size + extraSize); // Just to be sure lock malloc too
	}

	void* res = AlignForward(mem + sizeof(void*), alignment);
	*((void**) res - 1) = mem;

	assert((uintptr_t) res % alignment == 0);

	return res;
}

void HeapDestruct(IAllocator* allocator) {
	assert(allocator);
	HeapAllocator* heapAllocator = static_cast<HeapAllocator*>(allocator);

	SpinLock lock;
	LockGuard<SpinLock> lockGuard(lock);
	assert(heapAllocator->_allocationsCount == 0);
	*heapAllocator = {};
}


IAllocator* InitHeapAllocator(HeapAllocator* allocator) {
	assert(allocator);
	// Heap allocator dosnt need context, only one number as debug counter
	allocator->_allocationsCount = 0;
	allocator->Allocate = HeapAllocate;
	allocator->Deallocate = HeapDeallocate;
	allocator->Destruct = HeapDestruct;

	return allocator;
}

