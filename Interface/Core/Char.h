#ifndef CoreChar_h
#define CoreChar_h

#include "../../Pure.h"

#define isspace(c) isspace(c)
#define isdigit(c) isdigit(c)
#define isalpha(c) isalphabetic(c)
#define isalnum(c) isalphanumeric(c)

static inline boolean isspace        (char);
static inline boolean isdigit        (char);
static inline boolean isalphabetic   (char);
static inline boolean isalphanumeric (char);

/// inline implementations
static inline boolean
isspace(char character)
{
	return character == ' ' || (unsigned)character - '\t' < 5;
}

static inline boolean
isdigit(char character)
{
	return character >= '0' && character <= '9';
}

static inline boolean
isalphabetic(char character)
{
	return ( (unsigned)character | 32 ) - 'a' < 26;
}

static inline boolean
isalphanumeric(char character)
{
	return isalphabetic(character) || isdigit(character);
}

static inline uint64_t
charseq_countbytes(const charseq_t charseq)
{
	if ( !charseq ) return 0;

	uint64_t count = 0;
	while ( charseq[count] != '\0' )
		count++;

	return count;
}

#endif // Char_h