#ifndef CoreByte_h
#define CoreByte_h
#include "../../Pure.h"

#include "Error.h"
#include "Array.h"

__ImplArray(byte_t);
typedef arrayof(byte_t) bytearray_t;

__DeclArrayInsert   (byte_t, bytearray_insert);
__DeclArrayInsertAt (byte_t, bytearray_insertat);
__DeclArrayRemoveAt (byte_t, bytearray_removeat);

static inline size_t byteseq_countbytes(byteseq_t charseq);

errcode_t _bytearray_appendbyteseq (bytearray_t* array, byteseq_t seq, size_t seqsize);
errcode_t  bytearray_appendbyteseq (bytearray_t* array, byteseq_t seq);

static inline size_t
byteseq_countbytes(byteseq_t seq)
{
	if ( !seq ) return 0;

	uint64_t count = 0;
	while ( seq[count] != '\0' )
		count++;

	return count;
}

#endif // CoreByte_h