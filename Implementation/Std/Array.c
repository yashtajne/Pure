#include "PlatformInc.def"
#include "../../Interface/Std/Self.h"
#include "../../Interface/Std/Array.h"
#include "../../Interface/Std/Memory.h"

/* array_t has been removed from the API.
   newArray, freeArray, resizeArray have been replaced by the
   type-safe generic arrayof(T) macros (__ImplArrayAllocate,
   __ImplArrayDeallocate, __ImplArrayReallocate, __ImplArrayAppend). */
