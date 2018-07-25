#pragma once


//  IAllocator
//    Allocator interface for aligned allocations
//    Alignment has to be power of two
//

struct IAllocator {
	void* (*Allocate)(IAllocator* allocator, size_t size, size_t alignment);
	void (*Deallocate)(IAllocator* allocator, void* mem);

	void (*Destruct)(IAllocator* allocator);
};

//  Allocate, Deallocate
//    Main functions to allocate and deallocates 
//    Wrap function pointers in interface

void* Allocate(IAllocator* allocator, size_t size, size_t alignment);


void Deallocate(IAllocator* allocator, void* mem);


void AllocatorDestruct(IAllocator* allocator);











inline void* Allocate(IAllocator* allocator, size_t size, size_t alignment) {
	return (*allocator->Allocate)(allocator, size, alignment);
}

inline void Deallocate(IAllocator* allocator, void* mem) {
	(*allocator->Deallocate)(allocator, mem);
}

inline void AllocatorDestruct(IAllocator* allocator) {
	(*allocator->Destruct)(allocator);
}