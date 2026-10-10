#ifndef CoreCharseq_h
#define CoreCharseq_h

#include "Char.h"
#include "Array.h"

typedef charseq_t* charseqptr_t;

/* ~ ~ Charseq Interface ~ ~ */
static inline size_t charseq_countbytes (charseq_t charseq);

boolean charseq_startswith (charseq_t original, charseq_t prefix);
boolean charseq_endswith   (charseq_t original, charseq_t suffix);

boolean charseq_isequals (charseq_t original, charseq_t compare_with);

boolean charseq_iswhitespace (charseq_t);
boolean charseq_isempty      (charseq_t); // <-- not implemented!!!

void charseq_touppercase (charseq_t);
void charseq_tolowercase (charseq_t);

void charseq_trimleft  (charseq_t);
void charseq_trimright (charseq_t);

boolean charseq_contains (charseq_t, charseq_t subseq);
boolean charseq_find     (charseq_t, charseq_t subseq, uint64_t* position);

// static inline void charseq_replace (charseq_t this, charseq_t target, charseq_t replacement);
charseq_t charseq_firstoccurence   (charseq_t seq, char_t c);

/* ~ ~ Array of charseq_t ~ ~ */
__ImplArray(charseq_t)
__DeclArrayAppend   (charseq_t, charseqarray_append)
__DeclArrayInsertAt (charseq_t, charseqarray_insertat)
__DeclArrayRemoveAt (charseq_t, charseqarray_removeat)

/* ~ ~ KeyValuePairs KvCharseq & KvCharseqArray Interface ~ ~ */
__ImplArray(kvcharseq_t)
__DeclArrayAppend   (kvcharseq_t, kvcharseqarray_append)
__DeclArrayInsertAt (kvcharseq_t, kvcharseqarray_insertat)
__DeclArrayRemoveAt (kvcharseq_t, kvcharseqarray_removeat)

kvcharseq_t* kvcharseqarray_find(arrayof(kvcharseq_t)*, charseq_t key);

#endif // CoreCharseq_h