#include "../../Interface/Core/Charseq.h"
#include "../../Interface/Core/String.h"
#include "../../Interface/Core/Array.h"
#include "../../Interface/Core/Char.h"

/* ~ ~ stringarray_t implementations ~ ~ */
__ImplArrayAppend   (string_t, stringarray_append)
__ImplArrayInsertAt (string_t, stringarray_insertat)
__ImplArrayRemoveAt (string_t, stringarray_removeat)

errcode_t
string_appendchar(string_t* string, char ch)
{
	if ( !string )
		return PURE_ERROR_NULL_POINTER;

	if ( !string_sizeoffreebytes(string) )
		return PURE_ERROR_STRING_FULL;

	charseq_t seq = string_charseq(string);
	seq[string->onstack.size++] = ch;
	seq[string->onstack.size] = '\0';

	return PURE_OK;
}

errcode_t
string_appendcharseq(string_t* string, charseq_t seq)
{
	if ( !string || !seq )
		return PURE_ERROR_NULL_POINTER;

	return _string_appendcharseq( string, (byte_t*)seq, charseq_countbytes( seq ) );
}

errcode_t
_string_appendcharseq(string_t* string, byte_t buffer[], uint64_t buffersize)
{
	if ( !string || !buffer )
		return PURE_ERROR_NULL_POINTER;

	if ( buffersize >= UINT32_MAX )
		return PURE_ERROR_STRING_TOO_LONG;

	if ( string_sizeoffreebytes(string) < buffersize )
		return PURE_ERROR_STRING_FULL;

	charseq_t seq = string_charseq(string);
	for ( uint32_t i = 0; i < buffersize; i++ )
		seq[string->onstack.size++] = buffer[i];
	seq[string->onstack.size] = '\0';

	return PURE_OK;
}

errcode_t
string_chop(string_t* string, uint32_t new_size)
{
	if ( !string )
		return PURE_ERROR_NULL_POINTER;

	if ( string->onstack.size < new_size )
		return PURE_ERROR_INVALID_ARGUMENT;

	charseq_t seq = string_charseq(string);

	seq[new_size] = '\0';
	string->onstack.size = new_size;

	return PURE_OK;
}

errcode_t
string_merge(string_t* string1, string_t* string2)
{
	if ( !string1 || !string2 )
		return PURE_ERROR_NULL_POINTER;

	return _string_appendcharseq( string1, (byte_t*)string_charseq( string2 ), string2->onstack.size );
}

void
string_touppercase(string_t* string)
{
	// if ( !string )
	// 	return PURE_ERROR_NULL_POINTER;

	charseq_touppercase( string_charseq( string ) );
	// return PURE_OK;
}

void
string_tolowercase(string_t* string)
{
	// if ( !string )
	// 	return PURE_ERROR_NULL_POINTER;

	charseq_tolowercase( string_charseq( string ) );
	// return PURE_OK;
}

void
string_trimleft(string_t* string)
{
	// if ( !string )
	// 	return PURE_ERROR_NULL_POINTER;

	charseq_trimleft( string_charseq( string ) );
	string->onstack.size = charseq_countbytes( string_charseq( string ) );
	// return PURE_OK;
}

void
string_trimright(string_t* string)
{
	// if ( !string )
	// 	return PURE_ERROR_NULL_POINTER;

	charseq_t seq = string_charseq( string );
	uint64_t size = string->onstack.size;
	while ( size && isspace( seq[size - 1] ) )
		size--;

	seq[size] = '\0';
	string->onstack.size = size;
	// return PURE_OK;
}