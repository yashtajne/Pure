#include "../../Interface/Core/Char.h"
#include "../../Interface/Core/Memory.h"
#include "../../Interface/Core/Error.h"

kvcharseq_t*
kvcharseqarray_find(arrayof(kvcharseq_t)* arr, charseq_t key)
{
	for ( uint64_t i = 0; i < arr->length; i++ )
	{
		if ( charseq_isequals( key, arr->elements[i].key ) )
			return &arr->elements[i];
	}
	return NULL;
}

///

boolean
charseq_iswhitespace(charseq_t seq)
{
	uint64_t seqlen = charseq_countbytes( seq );
	for ( ; seqlen > 0; seqlen-- )
	{
		if ( !isspace(seq[seqlen]) )
			return false;
	}

	return true;
}

boolean
charseq_isequals(charseq_t a, charseq_t b)
{
	while ( *a && *a == *b )
	{
		a++;
		b++;
	}

	return *a == *b;
}

boolean
charseq_startswith(charseq_t charseq, charseq_t prefix)
{
	uint64_t length_of_original = charseq_countbytes( charseq );
	uint64_t length_of_prefix = charseq_countbytes( prefix );

	for ( uint32_t i = 0; i < length_of_prefix - 1; i++ )
	{
		if ( charseq[i] != prefix[i] )
			return false;
	}

	return true;
}

boolean
charseq_endswith(charseq_t charseq, charseq_t suffix)
{
	uint64_t length_of_original = charseq_countbytes( charseq );
	uint64_t length_of_suffix = charseq_countbytes( suffix );

	if ( length_of_original < length_of_suffix )
		return false;

	for ( uint32_t i = 0; i < length_of_suffix; i++ )
	{
		if ( charseq[length_of_original - length_of_suffix + i] != suffix[i] )
			return false;
	}

	return true;
}

void
charseq_touppercase(char* charseq)
{

	while ( *charseq )
	{
		char c = *charseq;

		if (c >= 'a' && c <= 'z')
		{
			*charseq = c - ('a' - 'A');
		}

		charseq++;
	}
}

void
charseq_tolowercase(char* charseq)
{
	while (*charseq)
	{
		char c = *charseq;

		if (c >= 'A' && c <= 'Z')
		{
			*charseq = c + ('a' - 'A');
		}

		charseq++;
	}
}

boolean
charseq_contains(charseq_t this, charseq_t subseq)
{
	if ( *subseq == '\0' )
		return true;

	for ( ; *this != '\0'; this++ )
	{
		charseq_t a = this;
		charseq_t b = subseq;

		while (*a && *b && *a == *b)
		{
			a++;
			b++;
		}

		if ( *b == '\0' )
			return true;
	}

	return false;
}

boolean
charseq_find(charseq_t this, charseq_t subseq, uint64_t* position)
{
	if ( *subseq == '\0' )
		return true;

	for ( charseq_t i = this; *i != '\0'; i++ )
	{
		charseq_t a = i;
		charseq_t b = subseq;

		while ( *a && *b && *a == *b )
		{
			a++;
			b++;
		}

		if ( *b == '\0' )
		{
			*position = i - this;
			return true;
		}
	}

	return false;
}

charseq_t
charseq_firstoccurence(charseq_t seq, char_t c)
{
	while ( *seq != (char)c && *seq != '\0' )
		seq++;

	return (*seq == (char)c) ? (char *)seq : NULL;
}

void
charseq_trimleft(charseq_t seq)
{
	uint64_t i = 0;
	uint64_t seqlen = charseq_countbytes( seq );

	while ( i < seqlen && isspace( seq[i] ) )
		i++;

	if ( !i )
		return;

	uint64_t new_size = seqlen - i;
	if ( !new_size )
	{
		seq[0] = '\0';
		return;
	}

	err_t err = memory_copy( &seq[i], &seq[0], new_size );
	if ( err != PURE_OK )
		return;

	seq[new_size] = '\0';
	return;
}

void
charseq_trimRight(charseq_t seq)
{
	uint64_t new_size = charseq_countbytes( seq );

	while ( new_size > 0 && isspace( seq[new_size - 1] ) )
		new_size--;

	seq[new_size] = '\0';
}