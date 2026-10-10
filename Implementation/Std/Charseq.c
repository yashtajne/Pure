#include "../../Pure.h"
#include "../../Platform/Platform.def"

#include "../../Interface/Std/Memory.h"
#include "../../Interface/Std/Charseq.h"
#include "../../Interface/Std/Self.h"

__ImplArrayAllocate   (kvcharseq_t, kvcharseqarray_new)
__ImplArrayReallocate (kvcharseq_t, kvcharseqarray_free)
__ImplArrayDeallocate (kvcharseq_t, kvcharseqarray_resize)

__ImplArrayAllocate   (charseq_t, charseqarray_new)
__ImplArrayReallocate (charseq_t, charseqarray_free)
__ImplArrayDeallocate (charseq_t, charseqarray_resize)

///

charseq_t
charseq_new(uint64_t size)
{
	charseq_t allocated = NULL;
	Self.LastError = _allocate( size, (voidptr_t*)&allocated );
	return allocated;
}

void
charseq_free(charseq_t seq)
{
	Self.LastError = _deallocate( seq );
}

void
charseq_resize(charseq_t* seq, uint64_t new_size)
{
	if ( !seq )
	{
		Self.LastError = PURE_ERROR_NULL_POINTER;
		return;
	}
	voidptr_t out_ptr = NULL;
	Self.LastError = _reallocate( *seq, new_size, &out_ptr );
	*seq = (charseq_t)out_ptr;
}

charseq_t
charseq_fromcharseq(charseq_t seq)
{
	uint64_t seqsize = charseq_countbytes( seq );
	return charseq_fromcharseqwithsize( seq, seqsize );
}

charseq_t
charseq_fromcharseqwithsize(charseq_t seq, uint64_t size)
{
	charseq_t allocatedseq = charseq_new( size );
	memory_copy( seq, allocatedseq, size );
	return allocatedseq;
}