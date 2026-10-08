#ifndef CoreChar_h
#define CoreChar_h
#include "../../Pure.h"
#include "Array.h"

static inline boolean isspace        (char);
static inline boolean isdigit        (char);
static inline boolean isalphabetic   (char);
static inline boolean isalphanumeric (char);

static inline uint64_t charseq_countbytes (char* charseq);

boolean charseq_startswith (charseq_t original, charseq_t prefix);
boolean charseq_endswith   (charseq_t original, charseq_t suffix);

boolean charseq_isequals (charseq_t original, charseq_t compare_with);

boolean charseq_iswhitespace (charseq_t);
boolean charseq_isempty      (charseq_t); // <-- not implemented!!!

void charseq_touppercase (charseq_t);
void charseq_tolowercase (charseq_t);

void charseq_trimleft  (charseq_t);
void charseq_trimright (charseq_t);

boolean charseq_contains (charseq_t, charseq_t subseq);
boolean charseq_find     (charseq_t, charseq_t subseq, uint64_t* position);

// static inline void charseq_replace (charseq_t this, charseq_t target, charseq_t replacement);
// charseq_t charseq_firstSubSequence (charseq_t seq, charseq_t subseq, err_t* occured); <-- not implemented yet!!!
charseq_t charseq_firstoccurence   (charseq_t seq, char_t c);

__ImplArray(kvcharseq_t);
__DeclArrayInsert   (kvcharseq_t, kvcharseq_insert)
__DeclArrayInsertAt (kvcharseq_t, kvcharseq_insertat)
__DeclArrayRemoveAt (kvcharseq_t, kvcharseq_removeat)

kvcharseq_t* kvcharseqarray_find(arrayof(kvcharseq_t)*, charseq_t key);

///
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
