#ifndef StdArray_h
#define StdArray_h

#include "../Core/Array.h"
#include "Memory.h"

array_t* newArray    (uint64_t size_of_type, uint64_t capacity, err_t* occured);
err_t    freeArray   (array_t* array);
err_t    resizeArray (array_t** array, uint64_t new_capacity);

#define heaparray(T, C, E) (newArray(sizeof(T), (C), (E)))

#define __DeclArrayAllocate(T, fname) \
	struct _Arrayof##T* fname(uint64_t capacity);

#define __DeclArrayDeallocate(T, fname) \
	err_t fname(struct _Arrayof##T* array);

#define __DeclArrayReallocate(T, fname) \
	err_t fname(struct _Arrayof##T** array, uint64_t new_capacity);

#define __DeclArrayAppend(T, fname) \
	err_t fname(struct _Arrayof##T** array, T* element);

///

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

#define __ImplArrayDeallocate(T, fname)           \
err_t fname(struct _Arrayof##T* array)            \
{                                                 \
	if ( !array )                                 \
	{                                             \
		Self.LastError = PURE_ERROR_NULL_POINTER; \
		return PURE_ERROR_NULL_POINTER;           \
	}                                             \
                                                  \
	deallocate( array );                          \
	return Self.LastError;                        \
}

#define __ImplArrayReallocate(T, fname)                                        \
err_t fname(struct _Arrayof##T** array, uint64_t new_capacity)                 \
{                                                                              \
	if ( !array || !*array )                                                   \
	{                                                                          \
		Self.LastError = PURE_ERROR_NULL_POINTER;                              \
		return PURE_ERROR_NULL_POINTER;                                        \
	}                                                                          \
                                                                               \
	if (new_capacity > (UINT64_MAX - sizeof(struct _Arrayof##T)) / sizeof(T))  \
	{                                                                          \
		Self.LastError = PURE_ERROR_INVALID_ARGUMENT;                          \
		return PURE_ERROR_INVALID_ARGUMENT;                                    \
	}                                                                          \
                                                                               \
	if (new_capacity == (*array)->capacity)                                    \
		return PURE_OK;                                                        \
                                                                               \
	voidptr_t reallocated = reallocate(                                        \
		*array, sizeof(struct _Arrayof##T) + (sizeof(T) * new_capacity)        \
	);                                                                         \
	if ( Self.LastError != PURE_OK )                                           \
		return Self.LastError;                                                 \
                                                                               \
	*array = (struct _Arrayof##T*)reallocated;                                 \
	(*array)->elements = (T*)((*array) + 1);                                   \
                                                                               \
	if ((*array)->length > new_capacity)                                       \
		(*array)->length = new_capacity;                                       \
                                                                               \
	(*array)->capacity = new_capacity;                                         \
	return PURE_OK;                                                            \
}

#define __ImplArrayAppend(T, fname)                                    \
err_t fname(struct _Arrayof##T** array, T* element)                    \
{                                                                      \
	if (!array || !*array || !element)                                 \
	{                                                                  \
		Self.LastError = PURE_ERROR_NULL_POINTER;                      \
		return PURE_ERROR_NULL_POINTER;                                \
	}                                                                  \
                                                                       \
	if ((*array)->length == (*array)->capacity)                        \
	{                                                                  \
		uint64_t new_capacity =                                        \
			(*array)->capacity == 0 ? 1 : (*array)->capacity * 2;      \
                                                                       \
		if (new_capacity >                                             \
			(UINT64_MAX - sizeof(struct _Arrayof##T)) / sizeof(T))     \
		{                                                              \
			Self.LastError = PURE_ERROR_INVALID_ARGUMENT;              \
			return PURE_ERROR_INVALID_ARGUMENT;                        \
		}                                                              \
                                                                       \
		voidptr_t reallocated = reallocate(                            \
			*array, sizeof(struct _Arrayof##T) +                       \
			(sizeof(T) * new_capacity)                                 \
		);                                                             \
                                                                       \
		if ( Self.LastError != PURE_OK )                               \
			return Self.LastError;                                     \
                                                                       \
		*array = (struct _Arrayof##T*)reallocated;                     \
		(*array)->elements = (T*)((*array) + 1);                       \
		(*array)->capacity = new_capacity;                             \
	}                                                                  \
                                                                       \
	(*array)->elements[(*array)->length] = *element;                   \
	(*array)->length++;                                                \
                                                                       \
	return PURE_OK;                                                    \
}

#endif // StdArray_h
