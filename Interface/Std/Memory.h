#ifndef StdMemory_h
#define StdMemory_h

#include "../Core/Error.h"
#include "../Core/Memory.h"

errcode_t _allocate   (size_t size, voidptr_t* outpointer);
errcode_t _reallocate (voidptr_t pointer, size_t size, voidptr_t* outpointer);
errcode_t _deallocate (voidptr_t pointer);

voidptr_t allocate   (size_t size);
voidptr_t reallocate (voidptr_t pointer, size_t size);
void      deallocate (voidptr_t pointer);

#endif // Memory_h