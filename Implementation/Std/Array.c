#include "../../Interface/Std/Self.h"
#include "../../Interface/Std/Array.h"
#include "../../Interface/Std/Memory.h"
#include "../../Platform/Platform.def"

/* array_t has been removed from the API.
   newArray, freeArray, resizeArray have been replaced by the
   type-safe generic arrayof(T) macros (__ImplArrayAllocate,
   __ImplArrayDeallocate, __ImplArrayReallocate, __ImplArrayAppend). */

__ImplArrayAllocate   (voidptr_t, voidptrarray_new)
__ImplArrayReallocate (voidptr_t, voidptrarray_free)
__ImplArrayDeallocate (voidptr_t, voidptrarray_resize)
__ImplArrayAppend (voidptr_t, voidptrarray_append)
