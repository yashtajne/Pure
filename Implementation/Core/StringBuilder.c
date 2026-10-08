#include "../../Interface/Core/String.h"
#include "../../Interface/Core/Char.h"

err_t
stringbuilder_reset(struct StringBuilder* sb)
{
	if ( !sb )
		return PURE_ERROR_NULL_POINTER;

	sb->size = 0;
	sb->chars[0] = '\0';

	return PURE_OK;
}

err_t
stringbuilder_append(struct StringBuilder* sb, charseq_t seq)
{
	if ( !sb || !seq )
		return PURE_ERROR_NULL_POINTER;

	uint64_t size_of_sequence = charseq_countbytes( seq );
	if ( size_of_sequence >= (sb->capacity - sb->size) )
		return PURE_ERROR_STRING_BUILDER_FULL;

	err_t err = memory_copy( seq, &sb->chars[sb->size], size_of_sequence );
	if ( err != PURE_OK )
		return err;

	sb->size += size_of_sequence;
	sb->chars[sb->size] = '\0';

	return PURE_OK;
}