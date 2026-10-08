#include "../../Interface/Core/Memory.h"

err_t
memory_copy(voidptr_t from, voidptr_t to, uint64_t size)
{
	if ( !from || !to )
		return PURE_ERROR_NULL_POINTER;

	byte_t* source = from;
	byte_t* destination = to;

	if ( source == destination || size == 0 )
		return PURE_OK;

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

	return PURE_OK;
}

err_t
memory_set(voidptr_t start, byte_t c, uint64_t n)
{
	if ( !start )
		return PURE_ERROR_NULL_POINTER;

	uchar_t *p = start;
	while (n--)
		*p++ = (uchar_t)c;

	return PURE_OK;
}