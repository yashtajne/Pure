#include "../../Interface/Core/ASCII.h"
#include "../../Interface/Core/Int.h"

boolean
ascii_isspace(ascii_t ascii)
{
	return (
		ascii == ' '  || ascii == '\t' || ascii == '\n' ||
		ascii == '\r' || ascii == '\f' || ascii == '\v'
	);
}

_ImplASCIItoInteger(_asciiseq_toint8, int8_t, uint8_t, INT8_MAX, 128u, INT8_MIN)
_ImplASCIItoInteger(_asciiseq_toint16, int16_t, uint16_t,INT16_MAX, 32768u, INT16_MIN)
_ImplASCIItoInteger(_asciiseq_toint32, int32_t, uint32_t,INT32_MAX, 2147483648u, INT32_MIN)
_ImplASCIItoInteger(_asciiseq_toint64, int64_t, uint64_t,INT64_MAX, 9223372036854775808ULL, INT64_MIN)

_ImplASCIItoUnsignedInteger(_asciiseq_touint8, uint8_t, UINT8_MAX)
_ImplASCIItoUnsignedInteger(_asciiseq_touint16, uint16_t, UINT16_MAX)
_ImplASCIItoUnsignedInteger(_asciiseq_touint32, uint32_t, UINT32_MAX)
_ImplASCIItoUnsignedInteger(_asciiseq_touint64, uint64_t, UINT64_MAX)

_ImplASCIIfromSignedInteger(_asciiseq_fromint8, int8_t)
_ImplASCIIfromSignedInteger(_asciiseq_fromint16, int16_t)
_ImplASCIIfromSignedInteger(_asciiseq_fromint32, int32_t)
_ImplASCIIfromSignedInteger(_asciiseq_fromint64, int64_t)

_ImplASCIIfromUnsignedInteger(_asciiseq_fromuint8, uint8_t)
_ImplASCIIfromUnsignedInteger(_asciiseq_fromuint16, uint16_t)
_ImplASCIIfromUnsignedInteger(_asciiseq_fromuint32, uint32_t)
_ImplASCIIfromUnsignedInteger(_asciiseq_fromuint64, uint64_t)

errcode_t
_ascii_frompointer(voidptr_t pointer, ascii_t buffer[], size_t buffer_size)
{
	/*
	 * "0x" + up to 16 hexadecimal digits + '\0'
	 * for a 64-bit address.
	 */
	// if ( !buffer )
	// 	return PURE_ERROR_NULL_POINTER;

	// if ( buffer_size < 19 )
	// 	return PURE_ERROR_BUFFER_TOO_SMALL;

	uintptr_t value = (uintptr_t)pointer;

	buffer[0] = '0';
	buffer[1] = 'x';

	if ( value == 0 )
	{
		buffer[2] = '0';
		buffer[3] = '\0';
		return PURE_OK;
	}

	char tmp[16];
	size_t i = 0;

	while ( value > 0 )
	{
		uint8_t digit = (uint8_t)(value & 0xF);

		if ( digit < 10 )
			tmp[i++] = (char)('0' + digit);
		else
			tmp[i++] = (char)('a' + (digit - 10));

		value >>= 4;
	}

	size_t pos = 2;
	while ( i > 0 )
		buffer[pos++] = tmp[--i];

	buffer[pos] = '\0';
	return PURE_OK;
}