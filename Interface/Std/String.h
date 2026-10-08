#ifndef StdString_h
#define StdString_h

#include "../Core/String.h"

#include "../Std/Array.h"
#include "../Std/Charseq.h"

// err_t string_init (string_t*, uint32_t capacity);

string_t* string_new    (uint32_t capacity);
void      string_free   (string_t*);
void      string_resize (string_t*, uint32_t new_capacity);

void  string_ensurecapacity (string_t* string, size_t extra);

void  string_fromcharseq (string_t* string, charseq_t seq);
void _string_fromcharseq (string_t* string, charseq_t seq, uint32_t length);


/* ~ ~ Std String Array Declarations  ~ ~ */

__DeclArrayAllocate   (string_t, stringarray_new)
__DeclArrayDeallocate (string_t, stringarray_free)
__DeclArrayReallocate (string_t, stringarray_resize)

__DeclArrayAppend (string_t, stringarray_append)

#endif // StdString_h