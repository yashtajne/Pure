#ifndef CoreArray_h
#define CoreArray_h
#include "../../Pure.h"

#include "Error.h"
#include "Memory.h"

struct _Array
{
	uint64_t  length;    // Number of items stored
	uint64_t  capacity;  // Max items it can hold
	uint64_t  type_size; // Size of one element
	byteptr_t elements;  // Actual Elements
};
typedef struct _Array array_t;
typedef struct _Array arr_t;

#define array_at(A, I) \
    ((A)->elements + ((A)->type_size * (I)))

#define array_reset(A) do {  \
	(A)->length = 0;         \
	(A)->elements[0] = '\0'; \
} while (0);

/*
array_t* mempool_newArray    (struct MemoryPoolHeader*, uint64_t sizeof_type, uint64_t capacity, err_t* occured);
err_t    mempool_freeArray   (struct MemoryPoolHeader*, array_t* array);
err_t    mempool_resizeArray (struct MemoryPoolHeader*, array_t** array, uint64_t new_capacity);
*/

voidptr_t array_elementAt(array_t* array, uint64_t index, err_t* occured);

err_t array_insert   (array_t* array, void* element);
err_t array_insertAt (array_t* array, void* element, uint64_t index);
err_t array_removeAt (array_t* array, uint64_t index);

uint64_t sizeof_used_array_t();  // <-- yet to implement
uint64_t sizeof_whole_array_t(); // <-- yet to implement

err_t array_merge (array_t* a1, array_t* a2, array_t** out_array); // <-- yet to implement
err_t array_split (array_t* array, uint64_t index, array_t** out_array); // <-- yet to implement

#define __DeclArrayInsert(T, fname) \
	err_t fname(struct _Arrayof##T* array, T* element);

#define __DeclArrayInsertAt(T, fname) \
	err_t fname(struct _Arrayof##T* array, T* element, uint64_t index);

#define __DeclArrayRemoveAt(T, fname) \
	err_t fname(struct _Arrayof##T* array, uint64_t index);

///
#define arrayof(T) struct _Arrayof##T

#define __ImplArray(T)      \
	struct _Arrayof##T      \
	{                       \
		uint64_t  length;   \
		uint64_t  capacity; \
		T* elements;        \
	}

#define __ImplArrayInsert(T, fname)               \
err_t fname(struct _Arrayof##T* array, T* element)\
{                                                 \
	if ( array->length == array->capacity )       \
		return PURE_ERROR_ARRAY_FULL;             \
                                                  \
	array->elements[array->length] = *element;    \
	array->length++;                              \
                                                  \
	return PURE_OK;                               \
}

#define __ImplArrayInsertAt(T, fname)                              \
err_t fname(struct _Arrayof##T* array, T* element, uint64_t index) \
{                                                                  \
	if ( index > array->length )                                   \
		return PURE_ERROR_OUT_OF_BOUNDS;                           \
                                                                   \
	if ( array->length == array->capacity )                        \
		return PURE_ERROR_ARRAY_FULL;                              \
                                                                   \
	uint64_t elements_after_current_index = array->length - index; \
	if ( elements_after_current_index > 0 )                        \
	{                                                              \
		err_t err = memory_copy(                                   \
			&array->elements[index],                               \
			&array->elements[(index + 1)],                         \
			(sizeof(T) * (elements_after_current_index))           \
		); if ( err ) return err;                                  \
	}                                                              \
                                                                   \
	err_t err = memory_copy(                                       \
		element,                                                   \
		&array->elements[index],                                   \
		sizeof(T)                                                  \
	); if ( err ) return err;                                      \
                                                                   \
	array->length++;                                               \
	return PURE_OK;                                                \
}

#define __ImplArrayRemoveAt(T, fname)                                    \
err_t fname(struct _Arrayof##T* array, uint64_t index)                   \
{                                                                        \
	if ( index >= array->length )                                        \
		return PURE_ERROR_OUT_OF_BOUNDS;                                 \
                                                                         \
	uint64_t elements_after_current_index = (array->length - index) - 1; \
	if ( elements_after_current_index > 0 )                              \
	{                                                                    \
		err_t err = memory_copy(                                         \
			&array->elements[(index + 1)],                               \
			&array->elements[index],                                     \
			sizeof(T) * elements_after_current_index                     \
		); if ( err ) return err;                                        \
	}                                                                    \
                                                                         \
	array->length--;                                                     \
	array->elements[array->length] = (T){ 0 };                           \
                                                                         \
	return PURE_OK;                                                      \
}

#endif // CoreArray_h