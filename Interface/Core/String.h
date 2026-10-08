#ifndef CoreString_h
#define CoreString_h
#include "../../Pure.h"

#include "Error.h"
#include "Memory.h"

/*
String
	- semialloc object
	- inline buffer that can hold 24 bytes
*/

/* ~ ~ Simple String ~ ~ */
struct _SimpleString
{
	size_t size;
	union
	{
		byteseq_t byteseq;
		charseq_t charseq;
		asciiseq_t asciiseq;
	};
};
typedef struct _SimpleString simplestring_t;
typedef struct _SimpleString smplstr_t;

#define  simplestring(seq) _simplestring(seq, byteseq_count( seq ))

#define _simplestring(seq, seqsize) _Generic((seq),                     \
	byteseq_t: (simplestring_t){ .byteseq = (seq), .size = (seqsize) }, \
	charseq_t: (simplestring_t){ .charseq = (seq), .size = (seqsize) }  \
)


/* ~ ~ Block String ~ ~ */
#define STRING_MAX_INLINE_SIZE 24

struct _BlockString
{
	size_t size;
	char_t data[STRING_MAX_INLINE_SIZE];
};
typedef struct _BlockString blockstring_t;

struct _String
{
	size_t capacity;
	union
	{
		blockstring_t onstack;
		simplestring_t onheap;
	};
};
typedef struct _String string_t;
typedef struct _String str_t;

#define emptystring                        \
	(string_t) {                           \
		.onstack = { 0 },                  \
		.capacity = STRING_MAX_INLINE_SIZE \
}

string_t* string_frominteger (uint64_t integer);

void  string_appendchar    (string_t* string, char ch);
void  string_appendcharseq (string_t* string, charseq_t seq);
void _string_appendcharseq (string_t* string, byte_t buffer[], uint64_t buffersize);

void string_touppercase (string_t* string);
void string_tolowercase (string_t* string);

void string_trimleft  (string_t* string);
void string_trimright (string_t* string);

void string_chop  (string_t* string, uint32_t new_size);
void string_merge (string_t* string1, string_t* string2); // <-- Yet to implement

#define string_sizeoffreebytes(str) (((str)->capacity - (str)->onstack.size) > 0 ? ((str)->capacity - (str)->onstack.size - 1) : 0)
#define string_hasspacefor(str, size) (string_sizeoffreebytes(str) >= (size))

#define string_isinline(str) _Generic((str),                                      \
	string_t*:       (((string_t*)(str))->capacity <= STRING_MAX_INLINE_SIZE),    \
	simplestring_t*: (boolean)false,                                              \
	default:         (boolean)true                                                \
)

#define string_charseq(str) _Generic((str),                    \
	string_t*:       (string_isinline((string_t*)(str))        \
	                   ? ((string_t*)(str))->onstack.data      \
	                   : ((string_t*)(str))->onheap.charseq),  \
	simplestring_t*: ((simplestring_t*)(str))->charseq,        \
	default:         ((blockstring_t*)(str))->data             \
)

/* ~ ~ String arrays ~ ~ */
#include "Array.h"

__ImplArray(string_t);
typedef struct _Arrayofstring_t stringarray_t;
typedef struct _Arrayofstring_t strarr_t;

__DeclArrayInsert   (string_t, stringarray_insert)
__DeclArrayInsertAt (string_t, stringarray_insertat)
__DeclArrayRemoveAt (string_t, stringarray_removeat)

#endif // CoreString_h
