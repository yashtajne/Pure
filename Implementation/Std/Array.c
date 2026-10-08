#include "PlatformInc.def"
#include "../../Interface/Std/Self.h"
#include "../../Interface/Std/Array.h"
#include "../../Interface/Std/Memory.h"

array_t*
newArray(uint64_t type_size, uint64_t capacity, err_t* occured)
{
	if ( !type_size || capacity > (UINT64_MAX - sizeof(array_t)) / type_size )
	{
		if ( occured )
			*occured = PURE_ERROR_INVALID_ARGUMENT;
		return NULL;
	}

	voidptr_t allocated = NULL;
	err_t err = heap_allocate( sizeof(array_t) + (type_size * capacity), &allocated );
	if ( err != PURE_OK )
	{
		if ( occured ) *occured = err;
		return NULL;
	}

	array_t* array = (array_t*)allocated;
	array->length = 0;
	array->capacity = capacity;
	array->type_size = type_size;
	array->elements = (byteptr_t)(array + 1);

	if ( occured ) *occured = PURE_OK;
	return allocated;
}

err_t
freeArray(array_t* array)
{
	if ( !array )
		return PURE_ERROR_NULL_POINTER;

	return heap_deallocate( array );
}

err_t
resizeArray(array_t** array, uint64_t capacity)
{
	if ( !array || !*array )
		return PURE_ERROR_NULL_POINTER;

	if ( capacity > (UINT64_MAX - sizeof(array_t)) / (*array)->type_size )
		return PURE_ERROR_INVALID_ARGUMENT;

	if ( capacity == (*array)->capacity )
		return PURE_OK;

	voidptr_t reallocated = NULL;
	err_t err = heap_reallocate( *array, sizeof(array_t) + ((*array)->type_size * capacity), &reallocated );
	if ( err != PURE_OK ) return err;

	*array = (array_t*)reallocated;
	(*array)->elements = (byteptr_t)((*array) + 1);

	if ( (*array)->length > capacity )
		(*array)->length = capacity;

	(*array)->capacity = capacity;
	return PURE_OK;
}