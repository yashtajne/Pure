#ifndef CoreASCII_h
#define CoreASCII_h

#include "Error.h"
#include "../../Pure.h"

errcode_t ascii_toint(ascii_t a);

errcode_t _asciiseq_tointeger (asciiseq_t ascii, int* outinteger);

errcode_t _asciiseq_frominteger (int64_t integer, ascii_t buffer[], size_t buffer_size);
errcode_t _asciiseq_frompointer (voidptr_t pointer, ascii_t buffer[], size_t buffer_size);

boolean ascii_isspace (ascii_t ascii);

#define _DeclASCIItoInteger(fname, type) \
    errcode_t fname(asciiseq_t ascii, type *out);

#define _DeclASCIItoUnsignedInteger(fname, type) \
    errcode_t fname(asciiseq_t ascii, type *out);

_DeclASCIItoInteger(_asciiseq_toint8, int8_t)
_DeclASCIItoUnsignedInteger(_asciiseq_touint8, uint8_t)

_DeclASCIItoInteger(_asciiseq_toint16, int16_t)
_DeclASCIItoUnsignedInteger(_asciiseq_touint16, uint16_t)

_DeclASCIItoInteger(_asciiseq_toint32, int32_t)
_DeclASCIItoUnsignedInteger(_asciiseq_touint32, uint32_t)

_DeclASCIItoInteger(_asciiseq_toint64, int64_t)
_DeclASCIItoUnsignedInteger(_asciiseq_touint64, uint64_t)

#define _ImplASCIItoInteger(fname, type, utype, max_value, min_magnitude, min_value) \
errcode_t fname(asciiseq_t ascii, type *out)                                      \
{                                                                                 \
    /* if (ascii == NULL || out == NULL) */                                       \
    /*    return PURE_ERROR_NULL_POINTER;  */                                     \
                                                                                  \
    int negative = 0;                                                             \
    if (*ascii == '-') {                                                          \
        negative = 1;                                                             \
        ascii++;                                                                  \
    } else if (*ascii == '+') {                                                   \
        ascii++;                                                                  \
    }                                                                             \
                                                                                  \
    if (*ascii == '\0') {                                                         \
        *out = 0;                                                                 \
        return PURE_OK;                                                           \
    }                                                                             \
                                                                                  \
    const utype limit = negative ? (utype)(min_magnitude) : (utype)(max_value);   \
    utype value = 0;                                                              \
                                                                                  \
    while (*ascii >= '0' && *ascii <= '9') {                                      \
        utype digit = (utype)(*ascii - '0');                                      \
        if (value > (limit - digit) / 10)                                         \
            return PURE_ERROR_BUFFER_TOO_SMALL;                                   \
        value = (utype)(value * 10 + digit);                                      \
        ascii++;                                                                  \
    }                                                                             \
                                                                                  \
    if (negative) {                                                               \
        *out = value == (utype)(min_magnitude)                                    \
            ? (type)(min_value)                                                   \
            : (type)-(type)value;                                                 \
    } else {                                                                      \
        *out = (type)value;                                                       \
    }                                                                             \
    return PURE_OK;                                                               \
}

#define _ImplASCIItoUnsignedInteger(fname, type, max_value)                       \
errcode_t fname(asciiseq_t ascii, type *out)                                      \
{                                                                                 \
    if (ascii == NULL || out == NULL)                                             \
        return PURE_ERROR_NULL_POINTER;                                           \
                                                                                  \
    if (*ascii == '-')                                                            \
        return PURE_ERROR_INVALID_ARGUMENT;                                       \
    if (*ascii == '+')                                                            \
        ascii++;                                                                  \
                                                                                  \
    if (*ascii == '\0') {                                                         \
        *out = 0;                                                                 \
        return PURE_OK;                                                           \
    }                                                                             \
                                                                                  \
    type value = 0;                                                               \
    while (*ascii >= '0' && *ascii <= '9') {                                      \
        type digit = (type)(*ascii - '0');                                        \
        if (value > ((type)(max_value) - digit) / 10)                             \
            return PURE_ERROR_BUFFER_TOO_SMALL;                                   \
        value = (type)(value * 10 + digit);                                       \
        ascii++;                                                                  \
    }                                                                             \
    *out = value;                                                                 \
    return PURE_OK;                                                               \
}

#define _DeclASCIIfromInteger(fname, type) \
    errcode_t fname(type integer, ascii_t buffer[], size_t buffer_size);

_DeclASCIIfromInteger(_asciiseq_fromint8, int8_t)
_DeclASCIIfromInteger(_asciiseq_fromuint8, uint8_t)

_DeclASCIIfromInteger(_asciiseq_fromint16, int16_t)
_DeclASCIIfromInteger(_asciiseq_fromuint16, uint16_t)

_DeclASCIIfromInteger(_asciiseq_fromint32, int32_t)
_DeclASCIIfromInteger(_asciiseq_fromuint32, uint32_t)

_DeclASCIIfromInteger(_asciiseq_fromint64, int64_t)
_DeclASCIIfromInteger(_asciiseq_fromuint64, uint64_t)

#define _ImplASCIIfromUnsignedInteger(fname, type)                              \
errcode_t fname(type integer, ascii_t buffer[], size_t buffer_size)             \
{                                                                               \
    if (buffer == NULL)                                                         \
        return PURE_ERROR_NULL_POINTER;                                         \
                                                                                \
    uint64_t value = (uint64_t)integer;                                         \
    char tmp[32];                                                               \
    size_t digits = 0;                                                          \
                                                                                \
    do {                                                                        \
        tmp[digits++] = (char)('0' + (value % 10));                             \
        value /= 10;                                                            \
    } while (value > 0);                                                        \
                                                                                \
    if (buffer_size < digits + 1)                                               \
        return PURE_ERROR_BUFFER_TOO_SMALL;                                     \
                                                                                \
    size_t pos = 0;                                                             \
    while (digits > 0)                                                          \
        buffer[pos++] = tmp[--digits];                                          \
                                                                                \
    buffer[pos] = '\0';                                                         \
    return PURE_OK;                                                             \
}

#define _ImplASCIIfromSignedInteger(fname, type)                                \
errcode_t fname(type integer, ascii_t buffer[], size_t buffer_size)             \
{                                                                               \
    if (buffer == NULL)                                                         \
        return PURE_ERROR_NULL_POINTER;                                         \
                                                                                \
    boolean negative = integer < 0;                                             \
    uint64_t value = negative                                                   \
        ? (uint64_t)(-(integer + 1)) + 1                                        \
        : (uint64_t)integer;                                                    \
                                                                                \
    char tmp[32];                                                               \
    size_t digits = 0;                                                          \
                                                                                \
    do {                                                                        \
        tmp[digits++] = (char)('0' + (value % 10));                             \
        value /= 10;                                                            \
    } while (value > 0);                                                        \
                                                                                \
    size_t needed = digits + (negative ? 1 : 0);                                \
    if (buffer_size < needed + 1)                                               \
        return PURE_ERROR_BUFFER_TOO_SMALL;                                     \
                                                                                \
    size_t pos = 0;                                                             \
    if (negative)                                                               \
        buffer[pos++] = '-';                                                    \
                                                                                \
    while (digits > 0)                                                          \
        buffer[pos++] = tmp[--digits];                                          \
                                                                                \
    buffer[pos] = '\0';                                                         \
    return PURE_OK;                                                             \
}

#endif // ASCII_h