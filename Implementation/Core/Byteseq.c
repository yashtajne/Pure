#include "../../Interface/Core/Byte.h"

errcode_t
_bytearray_appendbyteseq(bytearray_t* array, byteseq_t seq, size_t seqsize)
{
	errcode_t copystatus = memory_copy( seq, &array->elements[array->length], byteseq_countbytes( seq ) );
	if ( copystatus != PURE_OK )
		return copystatus;

	array->length += seqsize;
	return PURE_OK;
}

errcode_t
bytearray_appendbyteseq(bytearray_t* array, byteseq_t seq)
{
	if ( array->length == array->capacity )
		return PURE_ERROR_ARRAY_FULL;

	return _bytearray_appendbyteseq( array, seq, byteseq_countbytes( seq ) );
}