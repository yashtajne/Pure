#include "../../Interface/Core/Array.h"
#include "../../Interface/Core/Memory.h"

voidptr_t
array_elementAt(array_t* array, uint64_t index, err_t* occured)
{
	if ( !array )
	{
		if ( occured ) *occured = PURE_ERROR_NULL_POINTER;
		return NULL;
	}

	if ( index >= array->length )
	{
		if ( occured ) *occured = PURE_ERROR_OUT_OF_BOUNDS;
		return NULL;
	}

	if ( occured ) *occured = PURE_OK;
	return array->elements + (array->type_size * index);
}

/*
array_t*
mempool_newArray(struct MemoryPoolHeader* header, uint64_t type_size, uint64_t capacity, err_t* occured)
{
	if ( !header )
	{
		if ( occured ) *occured = PURE_ERROR_NULL_POINTER;
		return NULL;
	}

	if ( !type_size || capacity > (UINT64_MAX - sizeof(array_t)) / type_size )
	{
		if ( occured )
			*occured = PURE_ERROR_INVALID_ARGUMENT;
		return NULL;
	}

	voidptr_t allocated = NULL;
	err_t err = memorypool_allocate( header, sizeof(array_t) + (type_size * capacity), &allocated );
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
mempool_freeArray(struct MemoryPoolHeader* header, array_t* array)
{
	if ( !header || !array )
		return PURE_ERROR_NULL_POINTER;
	return memorypool_deallocate( header, array );
}

err_t
mempool_resizeArray(struct MemoryPoolHeader* header, array_t** array, uint64_t capacity)
{
	if ( !header || !array || !*array )
		return PURE_ERROR_NULL_POINTER;

	if ( capacity > (UINT64_MAX - sizeof(array_t)) / (*array)->type_size )
		return PURE_ERROR_INVALID_ARGUMENT;

	if ( capacity == (*array)->capacity )
		return PURE_OK;

	err_t err = memorypool_reallocate( header, sizeof(array_t) + ((*array)->type_size * capacity), (voidptr_t*)array );
	if ( err != PURE_OK )
		return err;

	if ( (*array)->length > capacity )
		(*array)->length = capacity;

	(*array)->capacity = capacity;
	return PURE_OK;
}
*/

err_t
array_insert(array_t* array, void* element)
{
	if ( !array || !element )
		return PURE_ERROR_NULL_POINTER;

	if ( array->length == array->capacity )
		return PURE_ERROR_ARRAY_FULL;

	err_t err = memory_copy( element, &array->elements[array->length * array->type_size], array->type_size );
	if ( err != PURE_OK )
		return err;

	array->length++;
	return PURE_OK;
}

err_t
array_insertAt(array_t* array, void* element, uint64_t index)
{
	if ( !array || !element )
		return PURE_ERROR_NULL_POINTER;

	if ( index > array->length )
		return PURE_ERROR_OUT_OF_BOUNDS;

	if ( array->length == array->capacity )
		return PURE_ERROR_ARRAY_FULL;

	uint64_t elements_after_current_index = array->length - index;
	if ( elements_after_current_index > 0 )
	{
		err_t err = memory_copy(
			&array->elements[index * array->type_size],
			&array->elements[(index + 1) * array->type_size],
			(array->type_size * (elements_after_current_index))
		);
		if ( err != PURE_OK )
			return err;
	}

	err_t err = memory_copy(
		element,
		&array->elements[index * array->type_size],
		array->type_size
	);
	if ( err != PURE_OK )
		return err;

	array->length++;
	return PURE_OK;
}

err_t
array_removeAt(array_t* array, uint64_t index)
{
	if ( !array )
		return PURE_ERROR_NULL_POINTER;

	if ( index >= array->length )
		return PURE_ERROR_OUT_OF_BOUNDS;

	uint64_t elements_after_current_index = (array->length - index) - 1;
	if ( elements_after_current_index > 0 )
	{
		err_t err = memory_copy(
			&array->elements[(index + 1) * array->type_size],
			&array->elements[index * array->type_size],
			array->type_size * elements_after_current_index
		);
		if ( err != PURE_OK )
			return err;
	}

	array->length--;
	array->elements[array->length * array->type_size] = '\0';

	return PURE_OK;
}