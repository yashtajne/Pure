#include "../../Interface/Core/Memory.h"

errcode_t
memory_copy(voidptr_t from, voidptr_t to, size_t size)
{
	if ( !from || !to )
		return PURE_ERROR_NULL_POINTER;

	_memory_copy( from, to, size );
	return PURE_OK;
}

errcode_t
memory_set(voidptr_t start, byte_t byte, size_t size)
{
	if ( !start )
		return PURE_ERROR_NULL_POINTER;

	_memory_set( start, byte, size );
	return PURE_OK;
}

void _memory_copy(voidptr_t from, voidptr_t to, size_t size)
{
	byte_t* source = from;
	byte_t* destination = to;

	if ( source == destination || size == 0 )
		return;

	if ( destination < source )
	{
		while ( size-- )
			*destination++ = *source++;
	}
	else
	{
		source += size;
		destination += size;

		while ( size-- )
			*--destination = *--source;
	}
}

void _memory_set(voidptr_t start, byte_t byte, size_t size)
{
	uchar_t *p = start;
	while (size--)
		*p++ = (uchar_t)byte;
}