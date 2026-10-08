#ifndef CoreASCII_h
#define CoreASCII_h
#include "../../Pure.h"
#include "Error.h"

err_t _ascii_tointeger (asciiseq_t ascii, integer_t* outinteger);

err_t ascii_frominteger (int64_t integer, ascii_t buffer[], uint8_t buffer_size);
err_t ascii_frompointer (voidptr_t pointer, ascii_t buffer[], uint8_t buffer_size);

boolean ascii_isspace (ascii_t ascii);

#endif // ASCII_h