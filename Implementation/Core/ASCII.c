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

errorcode_t
_ascii_tointeger(asciiseq_t ascii, integer_t* outinteger)
{
	if ( !ascii || !outinteger )
		return PURE_ERROR_NULL_POINTER;

	if ( *ascii == '\0' )
	{
		outinteger->i64 = 0;
		return PURE_OK;
	}

	int64_t result = 0;
	int8_t is_positive = 1;

	if ( *ascii == '-' )
	{
		is_positive = -1;
		ascii++;
	}
	else if ( *ascii == '+' )
	{
		ascii++;
	}

	while ( *ascii >= '0' && *ascii <= '9' )
	{
		result = result * 10 + (*ascii - '0');
		ascii++;
	}

	outinteger->i64 = result * is_positive;
	return PURE_OK;
}

/*
err_t
ascii_tointeger(ascii_t ascii[], int64_t* out_integer)
{
	if ( !ascii || !out_integer )
		return PURE_ERROR_NULL_POINTER;

	if ( *ascii == '\0' )
	{
		*out_integer = 0;
		return PURE_OK;
	}

	int64_t result = 0;
	int64_t is_positive = 1;

	if ( *ascii == '-' )
	{
		is_positive = -1;
		ascii++;
	}
	else if ( *ascii == '+' )
	{
		ascii++;
	}

	while ( *ascii >= '0' && *ascii <= '9' )
	{
		result = result * 10 + (*ascii - '0');
		ascii++;
	}

	*out_integer = result * is_positive;
	return PURE_OK;
}
*/

err_t
ascii_frominteger(int64_t integer, ascii_t buffer[], uint8_t buffersize)
{
	if ( !buffer || buffersize == 0 )
		return PURE_ERROR_BUFFER_TOO_SMALL;

	uint64_t needed = integer_countdigits( integer );
	if ( buffersize < needed + 1 )
		return PURE_ERROR_BUFFER_TOO_SMALL;

	uint64_t i = 0;
	if ( integer == 0 )
	{
		buffer[0] = '0';
		buffer[1] = '\0';
		return PURE_OK;
	}

	uint8_t negative = (integer < 0);
	if ( negative )
		integer = -integer;

	char tmp[32];
	while ( integer > 0 )
	{
		tmp[i++] = (char)('0' + (integer % 10));
		integer /= 10;
	}

	uint64_t pos = 0;
	if ( negative )
		buffer[pos++] = '-';

	for ( int64_t k = (int64_t)i - 1; k >= 0; k-- )
		buffer[pos++] = tmp[k];
	buffer[pos] = '\0';

	return PURE_OK;
}

err_t
ascii_frompointer(voidptr_t pointer, ascii_t buffer[], uint8_t buffer_size)
{
	/*
	 * "0x" + up to 16 hexadecimal digits + '\0'
	 * for a 64-bit address.
	 */
	if ( buffer_size < 19 )
		return PURE_ERROR_BUFFER_TOO_SMALL;

	uintptr_t value = (uintptr_t)pointer;

	buffer[0] = '0';
	buffer[1] = 'x';

	if (value == 0)
	{
		buffer[2] = '0';
		buffer[3] = '\0';
		return PURE_OK;
	}

	char tmp[16];
	uint64_t i = 0;

	while (value > 0)
	{
		uint8_t digit = value & 0xF;

		if (digit < 10)
			tmp[i++] = (char)('0' + digit);
		else
			tmp[i++] = (char)('a' + (digit - 10));

		value >>= 4;
	}

	uint64_t pos = 2;

	while (i > 0)
		buffer[pos++] = tmp[--i];

	buffer[pos] = '\0';

	return PURE_OK;
}