#ifndef CoreSelf_h
#define CoreSelf_h

#include "Error.h"

extern errorcode_t LastError;

#define __PURECORE \
	errorcode_t LastError = PURE_OK;

#endif // CoreSelf_h