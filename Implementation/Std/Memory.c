#include "../../Interface/Std/Memory.h"
#include "../../Interface/Std/Self.h"
#include "../../Platform/Platform.def"

struct HeapAllocationHeader
{
	uint64_t __reserved__;
	uint64_t requested_bytes;
};

voidptr_t
allocate(uint64_t bytes)
{
	voidptr_t pointer = NULL;
	Self.LastError = _allocate(bytes, &pointer);
	return pointer;
}

voidptr_t
reallocate(voidptr_t pointer, uint64_t bytes)
{
	voidptr_t out_pointer = NULL;
	Self.LastError = _reallocate(pointer, bytes, &out_pointer);
	return out_pointer;
}

void
deallocate(voidptr_t pointer)
{
	Self.LastError = _deallocate(pointer);
}

errcode_t
_allocate(uint64_t number_of_bytes, voidptr_t* out_pointer)
{
	// if ( !out_pointer )
	// 	return PURE_ERROR_NULL_POINTER;

#if IS_PLATFORM_WINDOWS

	if ( number_of_bytes > UINT64_MAX - sizeof(struct HeapAllocationHeader) )
	{
		*out_pointer = NULL;
		return PURE_ERROR_OUT_OF_MEMORY;
	}

	uint64_t allocated_bytes = number_of_bytes + sizeof(struct HeapAllocationHeader);

	void* allocated = VirtualAlloc(
		NULL,
		allocated_bytes,
		MEM_RESERVE | MEM_COMMIT,
		PAGE_READWRITE
	);

	if ( !allocated )
	{
		*out_pointer = NULL;
		return PURE_ERROR_OUT_OF_MEMORY;
	}

	struct HeapAllocationHeader* header = allocated;
	header->requested_bytes = number_of_bytes;
	*out_pointer = header + 1;

	return PURE_OK;

#elif IS_PLATFORM_LINUX

	if ( number_of_bytes > UINT64_MAX - sizeof(struct HeapAllocationHeader) )
	{
		*out_pointer = NULL;
		return PURE_ERROR_OUT_OF_MEMORY;
	}

	uint64_t allocated_bytes = number_of_bytes + sizeof(struct HeapAllocationHeader);

	voidptr_t allocated = linux_mmap(
		NULL,
		allocated_bytes,
		LINUX_PROT_READ | LINUX_PROT_WRITE,
		LINUX_MAP_PRIVATE | LINUX_MAP_ANONYMOUS,
		-1,
		0
	);

	if ( allocated == LINUX_MAP_FAILED )
	{
		*out_pointer = NULL;
		return PURE_ERROR_OUT_OF_MEMORY;
	}

	struct HeapAllocationHeader* header = allocated;
	header->requested_bytes = number_of_bytes;
	*out_pointer = header + 1;

	return PURE_OK;

#endif
}

errcode_t
_reallocate(voidptr_t pointer, uint64_t bytes, voidptr_t* out_pointer)
{
	// if ( !out_pointer )
	// 	return PURE_ERROR_NULL_POINTER;

	if ( !pointer )
		return _allocate( bytes, out_pointer );

	struct HeapAllocationHeader* old_header = (struct HeapAllocationHeader*)pointer - 1;
	uint64_t old_bytes = old_header->requested_bytes;

	if ( bytes == old_bytes )
	{
		*out_pointer = pointer;
		return PURE_OK;
	}

	voidptr_t new_pointer = NULL;
	errcode_t err = _allocate( bytes, &new_pointer );
	if ( err )
	{
		*out_pointer = NULL;
		return err;
	}

	*out_pointer = new_pointer;

	uint64_t copy_bytes = old_bytes < bytes ? old_bytes : bytes;
	err = memory_copy( pointer, new_pointer, copy_bytes );
	if ( err ) return err;

	err = _deallocate( pointer );
	if ( err ) return err;

	return PURE_OK;
}

errcode_t
_deallocate(voidptr_t pointer)
{
	// if ( !pointer )
	// 	return PURE_ERROR_NULL_POINTER;

#if IS_PLATFORM_WINDOWS

	struct HeapAllocationHeader* header = (struct HeapAllocationHeader*)pointer - 1;

	if ( !VirtualFree( header, 0, MEM_RELEASE ) )
		return PURE_ERROR_INVALID_ARGUMENT;
	return PURE_OK;

#elif IS_PLATFORM_LINUX

	struct HeapAllocationHeader* header = (struct HeapAllocationHeader*)pointer - 1;
	uint64_t allocated_bytes = header->requested_bytes + sizeof(struct HeapAllocationHeader);

	if ( linux_munmap( header, allocated_bytes ) != 0 )
		return PURE_ERROR_INVALID_ARGUMENT;
	return PURE_OK;

#endif
}