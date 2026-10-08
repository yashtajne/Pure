#include "../../Interface/Core/String.h"
#include "../../Interface/Core/Array.h"
#include "../../Interface/Core/Char.h"

// stringarray_t implementations
__ImplArrayInsert   (string_t, stringarray_insert)
__ImplArrayInsertAt (string_t, stringarray_insertat)
__ImplArrayRemoveAt (string_t, stringarray_removeat)

///

charseq_t
string_getcharseq(string_t* string, err_t* occured)
{
	if ( !string )
	{
		if ( occured ) *occured = PURE_ERROR_NULL_POINTER;
		return NULL;
	}

	if ( occured )
		*occured = PURE_OK;

	if ( !string_isinline( string ) )
		return string->characters.allocated_buffer;
	return string->characters.inline_buffer;
}

err_t
string_appendchar(string_t* string, char ch)
{
	if ( !string )
		return PURE_ERROR_NULL_POINTER;

	if ( !string_sizeoffreebytes(string) )
		return PURE_ERROR_STRING_FULL;

	charseq_t charseq = string_charseq(string);
	charseq[string->size++] = ch;
	charseq[string->size] = '\0';

	return PURE_OK;
}

err_t
string_appendcharseq(string_t* string, charseq_t seq)
{
	if ( !string || !seq )
		return PURE_ERROR_NULL_POINTER;
	return _string_appendcharseq( string, (byte_t*)seq, charseq_countbytes( seq ) );
}

err_t
_string_appendcharseq(string_t* string, byte_t buffer[], uint64_t buffersize)
{
	if ( buffersize >= UINT32_MAX )
		return PURE_ERROR_STRING_TOO_LONG;

	if ( string_sizeoffreebytes(string) < buffersize )
		return PURE_ERROR_STRING_FULL;

	charseq_t charseq = string_charseq(string);
	for ( uint32_t i = 0; i < buffersize; i++ )
		charseq[string->size++] = buffer[i];
	charseq[string->size] = '\0';

	return PURE_OK;
}

err_t
string_chop(string_t* string, uint32_t new_size)
{
	if ( string->size < new_size )
		return PURE_ERROR_INVALID_ARGUMENT;

	charseq_t string_charseq = string_charseq(string);

	string_charseq[new_size] = '\0';
	string->size = new_size;

	return PURE_OK;
}

/*
err_t
mempool_initString(struct MemoryPoolHeader* header, string_t* string, uint32_t capacity)
{
	if ( !header || !string )
		return PURE_ERROR_NULL_POINTER;

	err_t err;
	*string = empty_string;

	if ( capacity > STRING_INLINE_BUFFER_SIZE )
	{
		charseq_t allocated = NULL;
		err = memorypool_allocate( header, capacity, (voidptr_t*)&allocated );
		if ( err ) return err;

		string->characters.allocated_buffer = allocated;
		string->capacity = capacity;
	}

	return PURE_OK;
}

string_t*
mempool_newString(struct MemoryPoolHeader* header, uint32_t capacity, err_t *occured)
{
	if ( !header )
	{
		if ( occured ) *occured = PURE_ERROR_NULL_POINTER;
		return NULL;
	}

	err_t err;
	string_t* string = NULL;

	err = memorypool_allocate( header, sizeof(string_t), (voidptr_t*)&string );
	if ( err )
	{
		if ( occured ) *occured = err;
		return NULL;
	}

	if ( capacity > STRING_INLINE_BUFFER_SIZE )
	{
		charseq_t allocated = NULL;
		err = memorypool_allocate( header, capacity, (voidptr_t*)&allocated );
		if ( err )
		{
			if ( occured ) *occured = err;
			return string;
		}

		string->characters.allocated_buffer = allocated;
		string->capacity = capacity;
	}

	return string;
}

err_t
mempool_freeString(struct MemoryPoolHeader* header, string_t* string)
{
	if ( !header || !string )
		return PURE_ERROR_NULL_POINTER;

	if ( string->capacity > STRING_INLINE_BUFFER_SIZE )
	{
		err_t err = memorypool_deallocate( header, string->characters.allocated_buffer );
		if ( err ) return err;
	}

	*string = empty_string;
	return PURE_OK;
}

err_t
string_fromInteger(string_t* string, uint64_t integer)
{
	if ( !string ) return PURE_ERROR_NULL_POINTER;

	*string = empty_string;

	err_t err;
	charseq_t string_charseq = string_getcharseq( string, &err );
	if ( err ) return err;

	err = ascii_frominteger( integer, (byte_t*)string_charseq, string->capacity );
	if ( err ) return err;

	string->size = (uint32_t)charseq_countbytes( string_charseq );
	return PURE_OK;
}

err_t
poolstring_fromCharseq(struct MemoryPoolHeader* header, string_t *string, charseq_t seq)
{
	if ( !header || !string || !seq )
		return PURE_ERROR_NULL_POINTER;

	uint64_t length = charseq_countbytes( seq );
	if ( length >= UINT32_MAX )
		return PURE_ERROR_STRING_TOO_LONG;

	return poolstring_fromCharseqWithSize( header, string, seq, length );
}

err_t
poolstring_fromCharseqWithSize(struct MemoryPoolHeader* header, string_t* string, charseq_t seq, uint32_t length)
{
	if ( !header || !string || !seq )
		return PURE_ERROR_NULL_POINTER;

	*string = empty_string;
	if ( string_isinline(string) )
	{
		for ( uint32_t i = 0; i < length; i++ )
			string->characters.inline_buffer[i] = seq[i];
		string->characters.inline_buffer[length] = '\0';
		string->size = length;
		return PURE_OK;
	}

	charseq_t allocated = NULL;
	err_t err = memorypool_allocate( header, length + 1, (voidptr_t*)&allocated );
	if ( err ) return err;

	for ( uint32_t i = 0; i < length; i++ )
		allocated[i] = seq[i];
	allocated[length] = '\0';

	string->characters.allocated_buffer = allocated;
	string->size = length;
	string->capacity = length + 1;

	return PURE_OK;
}
*/
/*
err_t
string_split(string_t* string, char separator, array_t** out_array)
{
	if ( !string || !out_array )
		return PURE_ERROR_NULL_POINTER;

	charseq_t string_charseq = string_get_chars( string );
	if ( !string_charseq )
		return PURE_ERROR_NULL_POINTER;

	err_t err = array_allocateOnHeap( sizeof(string_t), string->size + 1, out_array );
	if ( err != PURE_OK || !*out_array )
		return err;

	uint32_t previous_slice_end = 0;

	for ( uint32_t current_pos = 0; current_pos < string->size; current_pos++ )
	{
		if ( string_charseq[current_pos] != separator )
			continue;

		string_t slice;

		err = string_from_charseq_with_size( &slice, &string_charseq[previous_slice_end], current_pos - previous_slice_end );
		if ( err != PURE_OK )
		{
			array_deallocateFromHeap( *out_array );
			*out_array = NULL;
			return err;
		}

		err = array_insert( *out_array, &slice );
		if ( err != PURE_OK )
		{
			string_free( &slice );
			array_deallocateFromHeap( *out_array );
			*out_array = NULL;
			return err;
		}

		previous_slice_end = current_pos + 1;
	}

	if ( previous_slice_end != string->size )
	{
		string_t slice;

		err = string_from_charseq_with_size( &slice, &string_charseq[previous_slice_end], string->size - previous_slice_end );
		if ( err != PURE_OK )
		{
			array_deallocateFromHeap( *out_array );
			*out_array = NULL;
			return err;
		}

		err = array_insert( *out_array, &slice );
		if ( err != PURE_OK )
		{
			string_free( &slice );
			array_deallocateFromHeap( *out_array );
			*out_array = NULL;
			return err;
		}
	}

	return PURE_OK;
}

err_t
string_append_char(string_t* string, char ch)
{
	if ( !string )
		return PURE_ERROR_NULL_POINTER;

	if ( string->capacity < (string->size + 1) )
		return PURE_ERROR_STRING_FULL;

	charseq_t string_charseq = string_get_chars( string );
	if ( !string_charseq )
		return PURE_ERROR_NULL_POINTER;

	string_charseq[string->size++] = ch;
	string_charseq[string->size] = '\0';

	return PURE_OK;
}

err_t
string_append_charseq(string_t* string, charseq_t seq)
{
	if ( !string || !seq )
		return PURE_ERROR_NULL_POINTER;

	uint64_t seqsize = charseq_countbytes(seq);
	if ( (string->capacity - string->size) < seqsize )
		return PURE_ERROR_STRING_FULL;

	if ( !seqsize )
		return PURE_OK;

	charseq_t string_charseq = string_get_chars( string );
	if ( !string_charseq )
		return PURE_ERROR_NULL_POINTER;

	err_t err = memory_copy( seq, (voidptr_t)(string_charseq + string->size), seqsize );
	if ( err != PURE_OK )
		return err;

	string->size += (uint32_t)seqsize;
	string_charseq[string->size] = '\0';
	return PURE_OK;
}ss
*/