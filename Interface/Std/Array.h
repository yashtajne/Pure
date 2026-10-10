#ifndef StdArray_h
#define StdArray_h

#include "../Core/Array.h"
#include "Memory.h"

#define __DeclArrayAllocate(T, fname) arrayof(T)* fname(uint64_t capacity);

#define __DeclArrayDeallocate(T, fname) void fname(arrayof(T)* array);
#define __DeclArrayReallocate(T, fname) void fname(arrayof(T)** array, uint64_t new_capacity);

/* ~ ~ Array of void pointers ~ ~ */
__ImplArray(voidptr_t)
__DeclArrayAllocate   (voidptr_t, voidptrarray_new)
__DeclArrayReallocate (voidptr_t, voidptrarray_free)
__DeclArrayDeallocate (voidptr_t, voidptrarray_resize)
__DeclArrayAppend (voidptr_t, voidptrarray_append)


/* ~ ~ Std Array Implementation ~ ~ */
#define __ImplArrayAllocate(T, fname)                                          \
struct _Arrayof##T* fname(uint64_t capacity)                                   \
{                                                                              \
	if (capacity > (UINT64_MAX - sizeof(struct _Arrayof##T)) / sizeof(T))      \
	{                                                                          \
		Self.LastError = PURE_ERROR_INVALID_ARGUMENT;                          \
		return NULL;                                                           \
	}                                                                          \
                                                                               \
	voidptr_t allocated = allocate(                                            \
		sizeof(struct _Arrayof##T) + (sizeof(T) * capacity)                    \
	);                                                                         \
	if ( Self.LastError != PURE_OK )                                           \
		return NULL;                                                           \
                                                                               \
	struct _Arrayof##T* array = (struct _Arrayof##T*)allocated;                \
	array->length = 0;                                                         \
	array->capacity = capacity;                                                \
	array->elements = (T*)(array + 1);                                         \
                                                                               \
	Self.LastError = PURE_OK;                                                  \
	return array;                                                              \
}

#define __ImplArrayDeallocate(T, fname)             \
void fname(struct _Arrayof##T* array)               \
{                                                   \
	if ( !array )                                   \
	{                                               \
		Self.LastError = PURE_ERROR_NULL_POINTER;   \
		return;                                     \
	}                                               \
                                                    \
	deallocate( array );                            \
}

#define __ImplArrayReallocate(T, fname)                                        \
void fname(struct _Arrayof##T** array, uint64_t new_capacity)                  \
{                                                                              \
	if ( !array || !*array )                                                   \
	{                                                                          \
		Self.LastError = PURE_ERROR_NULL_POINTER;                              \
		return;                                                                \
	}                                                                          \
                                                                               \
	if (new_capacity > (UINT64_MAX - sizeof(struct _Arrayof##T)) / sizeof(T))  \
	{                                                                          \
		Self.LastError = PURE_ERROR_INVALID_ARGUMENT;                          \
		return;                                                                \
	}                                                                          \
                                                                               \
	if (new_capacity == (*array)->capacity)                                    \
	{                                                                          \
		Self.LastError = PURE_OK;                                              \
		return;                                                                \
	}                                                                          \
                                                                               \
	voidptr_t reallocated = reallocate(                                        \
		*array, sizeof(struct _Arrayof##T) + (sizeof(T) * new_capacity)        \
	);                                                                         \
	if ( Self.LastError != PURE_OK )                                           \
		return;                                                                \
                                                                               \
	*array = (struct _Arrayof##T*)reallocated;                                 \
	(*array)->elements = (T*)((*array) + 1);                                   \
                                                                               \
	if ((*array)->length > new_capacity)                                       \
		(*array)->length = new_capacity;                                       \
                                                                               \
	(*array)->capacity = new_capacity;                                         \
	Self.LastError = PURE_OK;                                                  \
}

/*
// #define __DeclArrayAppend(T, fname) void fname(arrayof(T)** array, T* element);
#define __ImplArrayAppend(T, fname)                                     \
void fname(struct _Arrayof##T** array, T* element)                      \
{                                                                       \
	if (!array || !*array || !element)                                  \
	{                                                                   \
		Self.LastError = PURE_ERROR_NULL_POINTER;                       \
		return;                                                         \
	}                                                                   \
                                                                        \
	if ((*array)->length == (*array)->capacity)                         \
	{                                                                   \
		uint64_t new_capacity =                                         \
			(*array)->capacity == 0 ? 1 : (*array)->capacity * 2;       \
                                                                        \
		if (new_capacity >                                              \
			(UINT64_MAX - sizeof(struct _Arrayof##T)) / sizeof(T))      \
		{                                                               \
			Self.LastError = PURE_ERROR_INVALID_ARGUMENT;               \
			return;                                                     \
		}                                                               \
                                                                        \
		voidptr_t reallocated = reallocate(                             \
			*array, sizeof(struct _Arrayof##T) +                        \
			(sizeof(T) * new_capacity)                                  \
		);                                                              \
                                                                        \
		if ( Self.LastError != PURE_OK )                                \
			return;                                                     \
                                                                        \
		*array = (struct _Arrayof##T*)reallocated;                      \
		(*array)->elements = (T*)((*array) + 1);                        \
		(*array)->capacity = new_capacity;                              \
	}                                                                   \
                                                                        \
	(*array)->elements[(*array)->length] = *element;                    \
	(*array)->length++;                                                 \
                                                                        \
	Self.LastError = PURE_OK;                                           \
}
*/

#endif // StdArray_h
