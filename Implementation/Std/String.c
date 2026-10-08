#include "../../Interface/Std/String.h"
#include "../../Interface/Std/Memory.h"
#include "../../Interface/Std/Self.h"

#include "../../Interface/Core/Error.h"
#include "../../Interface/Core/Char.h"

static inline err_t
initString(string_t* string, uint32_t capacity)
{
	if ( !string )
		return PURE_ERROR_NULL_POINTER;

	err_t err;
	*string = empty_string;

	if ( capacity > STRING_INLINE_BUFFER_SIZE )
	{
		charseq_t allocated = NULL;
		err = heap_allocate( capacity, (voidptr_t*)&allocated );
		if ( err ) return err;

		string->characters.allocated_buffer = allocated;
		string->capacity = capacity;
	}

	return PURE_OK;
}

string_t*
string_new(uint32_t capacity, err_t *occured)
{
	err_t err;
	string_t* string = NULL;
	err = heap_allocate( sizeof(string_t), (voidptr_t*)&string );
	if ( err )
	{
		if ( occured ) *occured = err;
		return NULL;
	}

	*string = empty_string;

	if ( capacity > STRING_INLINE_BUFFER_SIZE )
	{
		charseq_t allocated = NULL;
		err = heap_allocate( capacity, (voidptr_t*)&allocated );
		if ( err )
		{
			if ( occured ) *occured = err;
			return string;
		}

		string->characters.allocated_buffer = allocated;
		string->capacity = capacity;
	}

	if ( occured )
		*occured = PURE_OK;

	return string;
}

err_t
string_free(string_t* string)
{
	if ( !string )
		return PURE_ERROR_NULL_POINTER;

	if ( !string_isinline(string) )
	{
		err_t err = heap_deallocate( string->characters.allocated_buffer );
		if ( err ) return err;
	}

	*string = empty_string;
	return PURE_OK;
}

err_t
string_resize(string_t* string, uint32_t new_capacity)
{
	if ( !string )
		return PURE_ERROR_NULL_POINTER;

	if ( !new_capacity )
		return string_free( string );

	err_t err;
	if ( (new_capacity < STRING_INLINE_BUFFER_SIZE) )
	{
		if ( !string_isinline(string) )
		{
			voidptr_t allocated = string->characters.allocated_buffer;
			err = memory_copy( string->characters.allocated_buffer, string->characters.inline_buffer, string->size );
			if ( err ) return err;

			err = heap_deallocate( allocated );
			if ( err ) return err;

			return PURE_OK;
		}
		else return PURE_OK;
	}

	if ( string_isinline( string ) )
	{
		string_t stringtemp;
		uint32_t old_size = string->size;

		err = initString( &stringtemp, new_capacity );
		if ( err ) return err;

		err = memory_copy( string->characters.inline_buffer, string_charseq( &stringtemp ), old_size );
		if ( err ) return err;

		*string = stringtemp;
		string->size = old_size;
		return PURE_OK;
	}

	charseq_t charseq = NULL;
	err = heap_reallocate( string->characters.allocated_buffer, new_capacity, (voidptr_t*)&charseq );
	if ( err ) return err;

	string->characters.allocated_buffer = charseq;
	string->capacity = new_capacity;
	return PURE_OK;
}

err_t
string_ensurecapacity(string_t* string, uint64_t extra)
{
	if ( !string )
		return PURE_ERROR_NULL_POINTER;

	if ( string_hasSpaceFor( string, extra ) )
		return PURE_OK;

	uint64_t needed = (uint64_t)string->size + extra + 1;
	uint64_t grown = (uint64_t)string->capacity * 2;
	if ( grown > needed )
		needed = grown;
	if ( needed > UINT32_MAX )
		return PURE_ERROR_STRING_TOO_LONG;

	return string_resize( string, (uint32_t)needed );
}

err_t
string_fromcharseq(string_t *string, charseq_t seq)
{
	if ( !string || !seq )
		return PURE_ERROR_NULL_POINTER;

	uint64_t length = charseq_countbytes( seq );
	if ( length >= UINT32_MAX )
		return PURE_ERROR_STRING_TOO_LONG;

	return _string_fromcharseq( string, seq, length );
}

err_t
_string_fromcharseq(string_t* string, charseq_t seq, uint32_t length)
{
	if ( !string || !seq )
		return PURE_ERROR_NULL_POINTER;

	*string = empty_string;
	if ( length <= STRING_INLINE_BUFFER_SIZE )
	{
		for ( uint32_t i = 0; i < length; i++ )
			string->characters.inline_buffer[i] = seq[i];
		string->characters.inline_buffer[length] = '\0';
		string->size = length;
		return PURE_OK;
	}

	charseq_t allocated = NULL;
	err_t err = heap_allocate( length + 1, (voidptr_t*)&allocated );
	if ( err ) return err;

	for ( uint32_t i = 0; i < length; i++ )
		allocated[i] = seq[i];
	allocated[length] = '\0';

	string->characters.allocated_buffer = allocated;
	string->size = length;
	string->capacity = length + 1;

	return PURE_OK;
}

__ImplArrayAllocate   (string_t, stringarray_new)
__ImplArrayDeallocate (string_t, stringarray_free)
__ImplArrayReallocate (string_t, stringarray_resize)
__ImplArrayAppend     (string_t, stringarray_append)