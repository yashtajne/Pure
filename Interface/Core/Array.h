#ifndef CoreArray_h
#define CoreArray_h
#include "../../Pure.h"

#include "Error.h"
#include "Memory.h"

#define arrayof(T) struct _Arrayof##T

#define __DeclArrayInsert(T, fname) void fname(arrayof(T)* array, T* element);
#define __DeclArrayInsertAt(T, fname) void fname(arrayof(T)* array, T* element, size_t index);
#define __DeclArrayRemoveAt(T, fname) void fname(arrayof(T)* array, size_t index);

/* ~ ~ Core Array Implementation ~ ~ */

#define __ImplArray(T) \
struct _Arrayof##T     \
{                      \
	size_t  length;    \
	size_t  capacity;  \
	T* elements;       \
}

#define __ImplArrayInsert(T, fname)                 \
void fname(struct _Arrayof##T* array, T* element)   \
{                                                   \
	if ( array->length == array->capacity )         \
	{                                               \
		LastError = PURE_ERROR_ARRAY_FULL;          \
		return;                                     \
	}                                               \
                                                    \
	array->elements[array->length] = *element;      \
	array->length++;                                \
                                                    \
	LastError = PURE_OK;                            \
}

#define __ImplArrayInsertAt(T, fname)                               \
void fname(struct _Arrayof##T* array, T* element, size_t index)     \
{                                                                   \
	if ( index > array->length )                                    \
	{                                                               \
		LastError = PURE_ERROR_OUT_OF_BOUNDS;                       \
		return;                                                     \
	}                                                               \
                                                                    \
	if ( array->length == array->capacity )                         \
	{                                                               \
		LastError = PURE_ERROR_ARRAY_FULL;                          \
		return;                                                     \
	}                                                               \
                                                                    \
	size_t elements_after_current_index = array->length - index;    \
	if ( elements_after_current_index > 0 )                         \
	{                                                               \
		LastError = memory_copy(                                    \
			&array->elements[index],                                \
			&array->elements[(index + 1)],                          \
			(sizeof(T) * (elements_after_current_index))            \
		);                                                          \
		if ( LastError != PURE_OK ) return;                         \
	}                                                               \
                                                                    \
	LastError = memory_copy(                                        \
		element,                                                    \
		&array->elements[index],                                    \
		sizeof(T)                                                   \
	);                                                              \
	if ( LastError != PURE_OK ) return;                             \
                                                                    \
	array->length++;                                                \
	LastError = PURE_OK;                                            \
}

#define __ImplArrayRemoveAt(T, fname)                                     \
void fname(struct _Arrayof##T* array, size_t index)                       \
{                                                                         \
	if ( index >= array->length )                                         \
	{                                                                     \
		LastError = PURE_ERROR_OUT_OF_BOUNDS;                             \
		return;                                                           \
	}                                                                     \
                                                                          \
	size_t elements_after_current_index = (array->length - index) - 1;    \
	if ( elements_after_current_index > 0 )                               \
	{                                                                     \
		LastError = memory_copy(                                          \
			&array->elements[(index + 1)],                                \
			&array->elements[index],                                      \
			sizeof(T) * elements_after_current_index                      \
		);                                                                \
		if ( LastError != PURE_OK ) return;                               \
	}                                                                     \
                                                                          \
	array->length--;                                                      \
	array->elements[array->length] = (T){ 0 };                            \
                                                                          \
	LastError = PURE_OK;                                                  \
}

#endif // CoreArray_h