#include "../../Interface/Core/Array.h"
#include "../../Interface/Core/Memory.h"

/* array_t has been removed from the API.
   All runtime-typed array functions (array_elementat, array_append,
   array_insertat, array_removeat) have been replaced by the
   type-safe generic arrayof(T) macros. */