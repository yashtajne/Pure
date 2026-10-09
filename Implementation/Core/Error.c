#include "../../Interface/Core/Error.h"

static const char* const pure_error_messages[] = {
#define PURE_ERROR(name, msg) msg,
#include "../../Interface/Errors.def"
#undef PURE_ERROR
};

charseq_t errorcode_to_message(errcode_t err)
{
	if (err < 0 || err >= PURE_ERROR_COUNT)
	{
		return "Unknown error code";
	}
	return (charseq_t)pure_error_messages[err];
}