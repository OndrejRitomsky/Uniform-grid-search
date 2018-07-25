#pragma once

#include "IAllocator.h"

//  HeapAllocator 
//    Simple allocator wrapping malloc / free
//    Is thread safe
//    Allocates extra memory to properly align all allocations
//    For debug, counting allocations

struct HeapAllocator : public IAllocator {
	size_t _allocationsCount;
};

IAllocator* InitHeapAllocator(HeapAllocator* allocator);
