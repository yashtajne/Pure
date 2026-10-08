#ifndef StdString_h
#define StdString_h

#include "../Core/String.h"

#include "../Std/Array.h"
#include "../Std/Charseq.h"

// err_t string_init (string_t*, uint32_t capacity);

string_t* string_new    (uint32_t capacity, err_t* occured);
err_t     string_free   (string_t*);
err_t     string_resize (string_t*, uint32_t new_capacity);

err_t  string_ensurecapacity (string_t* string, uint64_t extra);

err_t  string_fromcharseq (string_t* string, charseq_t seq);
err_t _string_fromcharseq (string_t* string, charseq_t seq, uint32_t length);

// err_t _string_dynappendcharseq()

__DeclArrayAllocate   (string_t, stringarray_new)
__DeclArrayDeallocate (string_t, stringarray_free)
__DeclArrayReallocate (string_t, stringarray_resize)

__DeclArrayAppend (string_t, stringarray_append)

#endif // StdString_h