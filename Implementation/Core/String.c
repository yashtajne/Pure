#include "../../Interface/Core/String.h"
#include "../../Interface/Core/Array.h"
#include "../../Interface/Core/Char.h"
#include "../../Interface/Core/Self.h"

// stringarray_t implementations
__ImplArrayInsert   (string_t, stringarray_insert)
__ImplArrayInsertAt (string_t, stringarray_insertat)
__ImplArrayRemoveAt (string_t, stringarray_removeat)

void
string_appendchar(string_t* string, char ch)
{
	if ( !string )
	{
		LastError = PURE_ERROR_NULL_POINTER;
		return;
	}

	if ( !string_sizeoffreebytes(string) )
	{
		LastError = PURE_ERROR_STRING_FULL;
		return;
	}

	charseq_t seq = string_charseq(string);
	seq[string->onstack.size++] = ch;
	seq[string->onstack.size] = '\0';

	LastError = PURE_OK;
}

void
string_appendcharseq(string_t* string, charseq_t seq)
{
	if ( !string || !seq )
	{
		LastError = PURE_ERROR_NULL_POINTER;
		return;
	}
	_string_appendcharseq( string, (byte_t*)seq, charseq_countbytes( seq ) );
}

void
_string_appendcharseq(string_t* string, byte_t buffer[], uint64_t buffersize)
{
	if ( !string || !buffer )
	{
		LastError = PURE_ERROR_NULL_POINTER;
		return;
	}

	if ( buffersize >= UINT32_MAX )
	{
		LastError = PURE_ERROR_STRING_TOO_LONG;
		return;
	}

	if ( string_sizeoffreebytes(string) < buffersize )
	{
		LastError = PURE_ERROR_STRING_FULL;
		return;
	}

	charseq_t seq = string_charseq(string);
	for ( uint32_t i = 0; i < buffersize; i++ )
		seq[string->onstack.size++] = buffer[i];
	seq[string->onstack.size] = '\0';

	LastError = PURE_OK;
}

void
string_chop(string_t* string, uint32_t new_size)
{
	if ( !string )
	{
		LastError = PURE_ERROR_NULL_POINTER;
		return;
	}

	if ( string->onstack.size < new_size )
	{
		LastError = PURE_ERROR_INVALID_ARGUMENT;
		return;
	}

	charseq_t seq = string_charseq(string);

	seq[new_size] = '\0';
	string->onstack.size = new_size;

	LastError = PURE_OK;
}

void
string_merge(string_t* string1, string_t* string2)
{
	if ( !string1 || !string2 )
	{
		LastError = PURE_ERROR_NULL_POINTER;
		return;
	}

	_string_appendcharseq( string1, (byte_t*)string_charseq( string2 ), string2->onstack.size );
}

void
string_touppercase(string_t* string)
{
	if ( !string )
	{
		LastError = PURE_ERROR_NULL_POINTER;
		return;
	}

	charseq_touppercase( string_charseq( string ) );
	LastError = PURE_OK;
}

void
string_tolowercase(string_t* string)
{
	if ( !string )
	{
		LastError = PURE_ERROR_NULL_POINTER;
		return;
	}

	charseq_tolowercase( string_charseq( string ) );
	LastError = PURE_OK;
}

void
string_trimleft(string_t* string)
{
	if ( !string )
	{
		LastError = PURE_ERROR_NULL_POINTER;
		return;
	}

	charseq_trimleft( string_charseq( string ) );
	string->onstack.size = charseq_countbytes( string_charseq( string ) );
	LastError = PURE_OK;
}

void
string_trimright(string_t* string)
{
	if ( !string )
	{
		LastError = PURE_ERROR_NULL_POINTER;
		return;
	}

	charseq_t seq = string_charseq( string );
	uint64_t size = string->onstack.size;
	while ( size && isspace( seq[size - 1] ) )
		size--;

	seq[size] = '\0';
	string->onstack.size = size;
	LastError = PURE_OK;
}
