#ifndef StdCharseq_h
#define StdCharseq_h
#include "../../Interface/Std/Array.h"
#include "../../Interface/Core/Char.h"

charseq_t charseq_new    (uint64_t size);
void      charseq_free   (charseq_t seq);
void      charseq_resize (charseq_t* seq, uint64_t new_size);

charseq_t charseq_fromcharseq (charseq_t seq);
charseq_t charseq_fromcharseqwithsize (charseq_t seq, uint64_t new_size);

__DeclArrayAllocate   (kvcharseq_t, kvcharseqarray_new)
__DeclArrayReallocate (kvcharseq_t, kvcharseqarray_free)
__DeclArrayDeallocate (kvcharseq_t, kvcharseqarray_resize)

__DeclArrayAppend (kvcharseq_t, kvcharseqarray_append)

#endif // StdCharseq_h