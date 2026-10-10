#ifndef CoreArray_h
#define CoreArray_h
#include "../../Pure.h"

#include "Error.h"
#include "Memory.h"

#define arrayof(T) struct _Arrayof##T

#define __DeclArrayAppend(T, fname)   errcode_t fname(arrayof(T)* array, T* element);
#define __DeclArrayInsertAt(T, fname) errcode_t fname(arrayof(T)* array, T* element, size_t index);
#define __DeclArrayRemoveAt(T, fname) errcode_t fname(arrayof(T)* array, size_t index);

#define __ImplArray(T) \
struct _Arrayof##T     \
{                      \
	size_t  length;    \
	size_t  capacity;  \
	T* elements;       \
};

#define __ImplArrayAppend(T, fname)                            \
errcode_t fname(struct _Arrayof##T* array, T* element)         \
{                                                              \
    /* if (array == NULL || element == NULL) */                \
    /*     return PURE_ERROR_NULL_POINTER;   */                \
                                                               \
    if (array->length == array->capacity)                      \
        return PURE_ERROR_ARRAY_FULL;                          \
                                                               \
    array->elements[array->length] = *element;                 \
    array->length++;                                           \
                                                               \
    return PURE_OK;                                            \
}

#define __ImplArrayInsertAt(T, fname)                                      \
errcode_t fname(struct _Arrayof##T* array, T* element, size_t index)       \
{                                                                          \
    /* if (array == NULL || element == NULL) */                            \
    /*     return PURE_ERROR_NULL_POINTER;   */                            \
                                                                           \
    if (index > array->length)                                             \
        return PURE_ERROR_OUT_OF_BOUNDS;                                   \
                                                                           \
    if (array->length == array->capacity)                                  \
        return PURE_ERROR_ARRAY_FULL;                                      \
                                                                           \
    size_t elements_after_current_index = array->length - index;           \
    if (elements_after_current_index > 0) {                                \
        errcode_t error = memory_copy(                                     \
            &array->elements[index],                                       \
            &array->elements[index + 1],                                   \
            sizeof(T) * elements_after_current_index                       \
        );                                                                 \
        if (error != PURE_OK)                                              \
            return error;                                                  \
    }                                                                      \
                                                                           \
    errcode_t error = memory_copy(                                         \
        element,                                                           \
        &array->elements[index],                                           \
        sizeof(T)                                                          \
    );                                                                     \
    if (error != PURE_OK)                                                  \
        return error;                                                      \
                                                                           \
    array->length++;                                                       \
    return PURE_OK;                                                        \
}

#define __ImplArrayRemoveAt(T, fname)                                      \
errcode_t fname(struct _Arrayof##T* array, size_t index)                   \
{                                                                          \
    /* if (array == NULL)                  */                              \
    /*     return PURE_ERROR_NULL_POINTER; */                              \
                                                                           \
    if (index >= array->length)                                            \
        return PURE_ERROR_OUT_OF_BOUNDS;                                   \
                                                                           \
    size_t elements_after_current_index = array->length - index - 1;       \
    if (elements_after_current_index > 0) {                                \
        errcode_t error = memory_copy(                                     \
            &array->elements[index + 1],                                   \
            &array->elements[index],                                       \
            sizeof(T) * elements_after_current_index                       \
        );                                                                 \
        if (error != PURE_OK)                                              \
            return error;                                                  \
    }                                                                      \
                                                                           \
    array->length--;                                                       \
    array->elements[array->length] = (T){ 0 };                             \
                                                                           \
    return PURE_OK;                                                        \
}

#endif // CoreArray_h