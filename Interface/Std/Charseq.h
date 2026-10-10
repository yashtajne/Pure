#ifndef StdCharseq_h
#define StdCharseq_h
#include "../../Interface/Std/Array.h"
#include "../../Interface/Core/Charseq.h"

charseq_t charseq_new    (size_t size);
void      charseq_free   (charseq_t seq);
void      charseq_resize (charseq_t* seq, size_t new_size);

charseq_t charseq_fromcharseq (charseq_t seq);
charseq_t charseq_fromcharseqwithsize (charseq_t seq, size_t new_size);

__DeclArrayAllocate   (charseq_t, charseqarray_new)
__DeclArrayReallocate (charseq_t, charseqarray_free)
__DeclArrayDeallocate (charseq_t, charseqarray_resize)

__DeclArrayAllocate   (kvcharseq_t, kvcharseqarray_new)
__DeclArrayReallocate (kvcharseq_t, kvcharseqarray_free)
__DeclArrayDeallocate (kvcharseq_t, kvcharseqarray_resize)

#endif // StdCharseq_h