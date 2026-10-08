#ifndef CoreError_h
#define CoreError_h
#include "../../Pure.h"

typedef enum
{
#define PURE_ERROR(name, msg) name,
#include "../../Interface/Errors.def"
#undef PURE_ERROR
	PURE_ERROR_COUNT
} _Error;

#define errtomsg(code) errorcode_to_message(code)
charseq_t errorcode_to_message(err_t code);

#endif // Error_h