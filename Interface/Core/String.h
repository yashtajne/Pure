#ifndef String_h
#define String_h
#include "../../Pure.h"

#include "Error.h"
#include "Memory.h"

/*
String
	- semialloc object
	- inline buffer that can hold 24 bytes
*/
#define STRING_INLINE_BUFFER_SIZE 24

#define empty_string                          \
	(string_t) {                              \
		.characters = { 0 },                  \
		.size = 0,                            \
		.capacity = STRING_INLINE_BUFFER_SIZE \
}

struct _InlineString
{
	size_t size;
	char_t buffer[];
};
typedef struct _InlineString inlinestring_t;
typedef struct _InlineString inlstr_t;


struct _ReadonlyString
{
	union
	{
		const charseq_t charseq;
		const byteseq_t byteseq;
	};
	const size_t size;
};
typedef struct _ReadonlyString rostring_t;
typedef struct _ReadonlyString rostr_t;

#define readonlystring(seq, seqsize) (rostring_t){ .charseq = (seq), .size = (seqsize) }

struct _SimpleString
{
	union
	{
		charseq_t charseq;
		byteseq_t byteseq;
	};
	size_t size;
};
typedef struct _SimpleString simplestring_t;
typedef struct _SimpleString simplestr_t;

#define simplestring(seq, seqsize) (simplestring_t){ .charseq = (seq), .size = (seqsize) }

struct _String
{
	size_t size;
	size_t capacity;
	union
	{
		char_t    inline_buffer[STRING_INLINE_BUFFER_SIZE];
		charseq_t allocated_buffer;
	}
	characters;
};
typedef struct _String string_t;
typedef struct _String str_t;

charseq_t string_getcharseq(string_t* string, err_t* occured);

#define string_sizeoffreebytes(s) \
	(((s)->capacity - (s)->size) > 0 ? ((s)->capacity - (s)->size - 1) : 0)

#define string_isinline(s) \
	((s)->capacity <= STRING_INLINE_BUFFER_SIZE)

#define string_charseq(s) \
	(string_isinline(s) ? (s)->characters.inline_buffer : (s)->characters.allocated_buffer)


static inline boolean string_hasSpaceFor(string_t* string, uint64_t size)
{
	return string_sizeoffreebytes(string) >= size;
}

/*
err_t mempool_initString(struct MemoryPoolHeader*, string_t*, uint32_t capacity);

string_t* mempool_newString    (struct MemoryPoolHeader*, uint32_t capacity, err_t* occured);
err_t     mempool_freeString   (struct MemoryPoolHeader*, string_t*);
err_t     mempool_resizeString (struct MemoryPoolHeader*, string_t**); // <-- yet to implement !!!

err_t poolstring_fromCharseq         (struct MemoryPoolHeader*, string_t* string, charseq_t seq);
err_t poolstring_fromCharseqWithSize (struct MemoryPoolHeader*, string_t* string, charseq_t seq, uint32_t length);
*/

string_t* string_frominteger (uint64_t integer);

err_t  string_appendchar    (string_t* string, char ch);
err_t  string_appendcharseq (string_t* string, charseq_t seq);
err_t _string_appendcharseq (string_t* string, byte_t buffer[], uint64_t buffersize);

err_t string_touppercase (string_t* string);
err_t string_tolowercase (string_t* string);

err_t string_trimleft  (string_t* string);
err_t string_trimright (string_t* string);
// err_t string_trimAll   (string_t* string);

err_t string_chop  (string_t* string, uint32_t new_size);
err_t string_merge (string_t* string1, string_t* string2); // <-- Yet to implement
// err_t string_split (string_t* string, char separator, array_t** out_array);

///
#include "Array.h"

__ImplArray(string_t);
typedef struct _Arrayofstring_t stringarray_t;
typedef struct _Arrayofstring_t strarr_t;

__DeclArrayInsert   (string_t, stringarray_insert)
__DeclArrayInsertAt (string_t, stringarray_insertat)
__DeclArrayRemoveAt (string_t, stringarray_removeat)

// stringarray_t* mempool_newStringArray    (err_t);
// err_t          mempool_freeStringArray   (stringarray_t*);
// err_t          mempool_resizeStringArray (stringarray_t**, uint64_t new_capacity);

// uint64_t sizeof_used_array_t();  // <-- yet to implement
// uint64_t sizeof_whole_array_t(); // <-- yet to implement

// err_t stringarray_merge (array_t* a1, array_t* a2, array_t** out_array); // <-- yet to implement
// err_t stringarray_split (array_t* array, uint64_t index, array_t** out_array); // <-- yet to implement


/*
StringBuilder
	- alloc only object
*/
struct StringBuilder
{
	uint64_t size;
	uint64_t capacity;
	uchar_t  chars[];
};

/*
struct StringBuilder* mempool_allocstringbuilder  (struct MemoryPoolHeader*, uint64_t capacity, err_t* occured);
struct StringBuilder* mempool_newStringBuilder  (struct MemoryPoolHeader*, uint64_t capacity, err_t* occured);
err_t                 mempool_freeStringBuilder (struct MemoryPoolHeader*, struct StringBuilder* string_builder);
*/

err_t stringbuilder_reset  (struct StringBuilder*);

err_t stringbuilder_appendcharseq (struct StringBuilder*, charseq_t);
err_t stringbuilder_appendstring  (struct StringBuilder*, string_t*);

#endif // String_h