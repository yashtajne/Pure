#include "../../Interface/Std/String.h"
#include "../../Interface/Std/Memory.h"
#include "../../Interface/Std/Self.h"

#include "../../Interface/Core/Error.h"
#include "../../Interface/Core/Char.h"

static inline void
initString(string_t* string, uint32_t capacity)
{
	if ( !string )
	{
		LastError = PURE_ERROR_NULL_POINTER;
		return;
	}

	*string = emptystring;

	if ( capacity > STRING_MAX_INLINE_SIZE )
	{
		charseq_t allocated = NULL;
		LastError = heap_allocate( capacity, (voidptr_t*)&allocated );
		if ( LastError != PURE_OK ) return;

		string->onheap.charseq = allocated;
		string->capacity = capacity;
	}

	LastError = PURE_OK;
}

string_t*
string_new(uint32_t capacity)
{
	string_t* string = NULL;
	LastError = heap_allocate( sizeof(string_t), (voidptr_t*)&string );
	if ( LastError != PURE_OK )
		return NULL;

	*string = emptystring;

	if ( capacity > STRING_MAX_INLINE_SIZE )
	{
		charseq_t allocated = NULL;
		LastError = heap_allocate( capacity, (voidptr_t*)&allocated );
		if ( LastError != PURE_OK )
		{
			err_t allocation_error = LastError;
			heap_deallocate( string );
			LastError = allocation_error;
			return NULL;
		}

		string->onheap.charseq = allocated;
		string->capacity = capacity;
	}

	LastError = PURE_OK;
	return string;
}

void
string_free(string_t* string)
{
	if ( !string )
	{
		LastError = PURE_ERROR_NULL_POINTER;
		return;
	}

	if ( !string_isinline(string) )
	{
		LastError = heap_deallocate( string->onheap.charseq );
		if ( LastError != PURE_OK ) return;
	}

	*string = emptystring;
	LastError = PURE_OK;
}

void
string_resize(string_t* string, uint32_t new_capacity)
{
	if ( !string )
	{
		LastError = PURE_ERROR_NULL_POINTER;
		return;
	}

	if ( !new_capacity )
	{
		string_free( string );
		return;
	}

	if ( new_capacity <= string->onstack.size )
	{
		LastError = PURE_ERROR_INVALID_ARGUMENT;
		return;
	}

	if ( new_capacity <= STRING_MAX_INLINE_SIZE )
	{
		if ( !string_isinline(string) )
		{
			voidptr_t allocated = string->onheap.charseq;
			LastError = memory_copy( string->onheap.charseq, string->onstack.data, string->onstack.size );
			if ( LastError != PURE_OK ) return;
			string->onstack.data[string->onstack.size] = '\0';

			LastError = heap_deallocate( allocated );
			if ( LastError != PURE_OK ) return;

			string->capacity = STRING_MAX_INLINE_SIZE;
			LastError = PURE_OK;
			return;
		}
		else
		{
			LastError = PURE_OK;
			return;
		}
	}

	if ( string_isinline( string ) )
	{
		string_t stringtemp;
		uint32_t old_size = string->onstack.size;

		initString( &stringtemp, new_capacity );
		if ( LastError != PURE_OK ) return;

		LastError = memory_copy( string->onstack.data, string_charseq( &stringtemp ), old_size );
		if ( LastError != PURE_OK ) return;

		*string = stringtemp;
		string->onstack.size = old_size;
		string_charseq( string )[old_size] = '\0';
		LastError = PURE_OK;
		return;
	}

	charseq_t charseq = NULL;
	LastError = heap_reallocate( string->onheap.charseq, new_capacity, (voidptr_t*)&charseq );
	if ( LastError != PURE_OK ) return;

	string->onheap.charseq = charseq;
	string->capacity = new_capacity;
	LastError = PURE_OK;
}

string_t*
string_frominteger(uint64_t integer)
{
	string_t* string = string_new( STRING_MAX_INLINE_SIZE );
	if ( !string )
		return NULL;

	charseq_t seq = string_charseq( string );
	uint32_t position = 0;
	if ( integer == 0 )
		seq[position++] = '0';
	else
	{
		char_t digits[20];
		while ( integer )
		{
			digits[position++] = (char_t)('0' + integer % 10);
			integer /= 10;
		}

		for ( uint32_t i = 0; i < position / 2; i++ )
		{
			char_t digit = digits[i];
			digits[i] = digits[position - i - 1];
			digits[position - i - 1] = digit;
		}

		for ( uint32_t i = 0; i < position; i++ )
			seq[i] = digits[i];
	}

	seq[position] = '\0';
	string->onstack.size = position;
	LastError = PURE_OK;
	return string;
}

void
string_ensurecapacity(string_t* string, uint64_t extra)
{
	if ( !string )
	{
		LastError = PURE_ERROR_NULL_POINTER;
		return;
	}

	if ( string_hasspacefor( string, extra ) )
	{
		LastError = PURE_OK;
		return;
	}

	if ( string->onstack.size >= UINT32_MAX ||
	     extra > UINT32_MAX - string->onstack.size - 1 )
	{
		LastError = PURE_ERROR_STRING_TOO_LONG;
		return;
	}

	uint64_t needed = (uint64_t)string->onstack.size + extra + 1;
	uint64_t grown = (uint64_t)string->capacity * 2;
	if ( grown > needed )
		needed = grown;
	if ( needed > UINT32_MAX )
	{
		LastError = PURE_ERROR_STRING_TOO_LONG;
		return;
	}

	string_resize( string, (uint32_t)needed );
}

void
string_fromcharseq(string_t *string, charseq_t seq)
{
	if ( !string || !seq )
	{
		LastError = PURE_ERROR_NULL_POINTER;
		return;
	}

	uint64_t length = charseq_countbytes( seq );
	if ( length >= UINT32_MAX )
	{
		LastError = PURE_ERROR_STRING_TOO_LONG;
		return;
	}

	_string_fromcharseq( string, seq, length );
}

void
_string_fromcharseq(string_t* string, charseq_t seq, uint32_t length)
{
	if ( !string || !seq )
	{
		LastError = PURE_ERROR_NULL_POINTER;
		return;
	}

	*string = emptystring;
	if ( length < STRING_MAX_INLINE_SIZE )
	{
		for ( uint32_t i = 0; i < length; i++ )
			string->onstack.data[i] = seq[i];
		string->onstack.data[length] = '\0';
		string->onstack.size = length;
		LastError = PURE_OK;
		return;
	}

	charseq_t allocated = NULL;
	LastError = heap_allocate( length + 1, (voidptr_t*)&allocated );
	if ( LastError != PURE_OK ) return;

	for ( uint32_t i = 0; i < length; i++ )
		allocated[i] = seq[i];
	allocated[length] = '\0';

	string->onheap.charseq = allocated;
	string->onstack.size = length;
	string->capacity = length + 1;

	LastError = PURE_OK;
}

stringarray_t*
stringarray_new(uint64_t capacity)
{
	if ( capacity > (UINT64_MAX - sizeof(stringarray_t)) / sizeof(string_t) )
	{
		LastError = PURE_ERROR_INVALID_ARGUMENT;
		return NULL;
	}

	voidptr_t allocated = allocate( sizeof(stringarray_t) + sizeof(string_t) * capacity );
	if ( LastError != PURE_OK )
		return NULL;

	stringarray_t* array = (stringarray_t*)allocated;
	array->length = 0;
	array->capacity = capacity;
	array->elements = (string_t*)(array + 1);
	LastError = PURE_OK;
	return array;
}

__ImplArrayDeallocate (string_t, stringarray_free)
__ImplArrayReallocate (string_t, stringarray_resize)
__ImplArrayAppend     (string_t, stringarray_append)
