#ifndef CoreMemory_h
#define CoreMemory_h
#include "../../Pure.h"
#include "Error.h"

void _memory_copy (voidptr_t from, voidptr_t to, size_t size);
void _memory_set  (voidptr_t start, byte_t byte, size_t size);

errcode_t memory_copy (voidptr_t from, voidptr_t to, size_t size);
errcode_t memory_set  (voidptr_t start, byte_t byte, size_t size);

static inline voidptr_t memory_alignpointer (voidptr_t ptr, uint64_t alignment);

static inline uint64_t  memory_alignforward   (uint64_t address, uint64_t alignment);
static inline uint64_t  memory_nextpoweroftwo (size_t bytes);

static inline uint64_t
memory_alignforward(uint64_t address, uint64_t alignment)
{
	return (address + (alignment - 1)) & ~(alignment - 1);
}

static inline voidptr_t
memory_alignpointer(voidptr_t ptr, uint64_t alignment)
{
	return (voidptr_t)memory_alignforward((uint64_t)ptr, alignment);
}

static inline uint64_t
memory_nextpoweroftwo(uint64_t bytes)
{
	if (bytes <= 1) return 1;

	bytes--;
	bytes |= bytes >> 1;
	bytes |= bytes >> 2;
	bytes |= bytes >> 4;
	bytes |= bytes >> 8;
	bytes |= bytes >> 16;
	bytes |= bytes >> 32;
	bytes++;

	return bytes;
}

#endif // CoreMemory_h