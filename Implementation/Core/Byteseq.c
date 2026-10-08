#include "../../Interface/Core/Byte.h"

err_t
_bytearray_appendbyteseq(bytearray_t* array, byteseq_t seq, size_t seqsize)
{
	if ( array->length == array->capacity )
		return PURE_ERROR_ARRAY_FULL;

	memory_copy( seq, &array->elements[array->length], seqsize );
	array->length += seqsize;

	return PURE_OK;
}

err_t
bytearray_appendbyteseq(bytearray_t* array, byteseq_t seq)
{
	if ( array->length == array->capacity )
		return PURE_ERROR_ARRAY_FULL;

	size_t seqsize = byteseq_countbytes( seq );
	memory_copy( seq, &array->elements[array->length], seqsize );
	array->length += seqsize;

	return PURE_OK;
}